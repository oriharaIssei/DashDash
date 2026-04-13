#include "PlayerOnCollision.h"

/// engine
#include "messageBus/MessageBus.h"

// event
#include "event/GamefailedEvent.h"
#include "event/PlayerExplosionEffectEvent.h"
#include "event/RestartEvent.h"

/// ECS
// component
#include "component/collision/collider/SphereCollider.h"
#include "component/collision/CollisionPushBackInfo.h"
#include "component/physics/Rigidbody.h"

#include "component/player/PlayerStatus.h"
#include "component/player/state/PlayerState.h"

#include "component/gimmick/Obstacle.h"
#include "component/gimmick/ObstacleShieldComponent.h"
#include "component/gimmick/RailPoints.h"
#include "component/gimmick/WallRunnableComponent.h"

#include "component/FollowTransformComponent.h"
#include "component/gimmick/TimeScaleEffectComponent.h"

#include "component/TimerComponent.h"

#include "component/material/Material.h"

/// util
#include "component/player/PlayerMoveUtils.h"

/// math
#include "math/MathEnv.h"

using namespace OriGine;

namespace {
/// 壁・床への衝突でゲームオーバーになる法線方向速度の閾値（m/s）
constexpr float kCrashNormalSpeedThreshold = 40.0f;
} // namespace

void PlayerOnCollision::Initialize() {}
void PlayerOnCollision::Finalize() {}

void PlayerOnCollision::UpdateEntity(OriGine::EntityHandle _handle) {
    auto* state          = GetComponent<PlayerState>(_handle);
    auto* status         = GetComponent<PlayerStatus>(_handle);
    auto* pushBackInfo   = GetComponent<CollisionPushBackInfo>(_handle);
    auto* rigidbody      = GetComponent<Rigidbody>(_handle);
    auto* sphereCollider = GetComponent<SphereCollider>(_handle);

    if (state == nullptr || pushBackInfo == nullptr || rigidbody == nullptr || sphereCollider == nullptr) {
        return;
    }

    const auto& collisionStateMap = sphereCollider->GetCollisionStateMap();
    const auto& collisionInfoMap  = pushBackInfo->GetCollisionInfoMap();

    // 毎フレーム、地面・壁との衝突状態をリセット
    bool isPreOnGround            = state->IsOnGround(); // 前フレームの地面接触状態を保持
    EntityHandle wallEntityHandle = state->GetWallEntityIndex();
    state->OffCollisionGround();
    state->OffCollisionWall();
    state->OffCollisionRail();

    for (auto& [entityId, collisionState] : collisionStateMap) {
        OriGine::Entity* collEnt = GetEntity(entityId);
        if (!collEnt) {
            continue;
        }

        // ゴール と 衝突したか
        if (collEnt->GetDataType().find("Goal") != std::string::npos) {
            // 時間を更新しないようにする
            OriGine::EntityHandle timerEntityHandle = GetUniqueEntity("Timer");
            auto* timer                             = GetComponent<TimerComponent>(timerEntityHandle);
            if (timer) {
                timer->SetStarted(false);
            }

            // ゴールと衝突した場合は、ゴールに到達したと判断する
            state->GetStateFlagRef().CurrentRef().SetFlag(PlayerStateFlag::IS_GOAL);
            continue;
        }

        // レールポイント と 衝突したか
        {
            RailPoints* railPoints = GetComponent<RailPoints>(entityId);
            if (railPoints && status->CanRideRail()) {
                state->OnCollisionRail(entityId);
                continue;
            }
        }

        // シールド と 衝突したか
        {
            ObstacleShieldComponent* shield = GetComponent<ObstacleShieldComponent>(entityId);
            if (shield) {
                if (!state->HasShield()) {
                    // FollowTransformComponent で プレイヤーに追従させる
                    FollowTransformComponent* follow = GetComponent<FollowTransformComponent>(entityId);
                    if (follow) {
                        follow->SetTarget(_handle);
                    }
                    // シールドと衝突した場合は、シールドを持っていると判断する
                    state->OnCollisionShield(entityId);
                }
            }
        }

        // 障害物 と 衝突したか
        {
            Obstacle* obstacle = GetComponent<Obstacle>(entityId);
            if (obstacle) {

                // シールドを持っている場合はペナルティを受けるが、ゲームオーバーにはならない
                if (state->HasShield()) {
                    float penaltyTime       = obstacle->GetPenaltyTime();
                    float invincibilityTime = obstacle->GetInvincibilityTimeOnCollision();
                    state->OnCollisionObstacle(penaltyTime, invincibilityTime);

                    // シールドは消す
                    state->ClearHasShieldFlag();
                    GetScene()->AddDeleteEntity(state->GetShieldEntityHandle());

                } else if (state->GetInvincibilityTime() <= 0.f) {
                    auto* transform = GetComponent<Transform>(_handle);
                    if (transform) {
                        PlayerExplosionEffectEvent event{};
                        event.position = transform->GetWorldTranslate();
                        MessageBus::GetInstance()->Emit<PlayerExplosionEffectEvent>(event);
                    }

                    // シールドを持っていない場合はゲームオーバー
                    // 上記Evedntの分Delay
                    EntityHandle failedSceneEntity = GetUniqueEntity("GameFailedScene");

                    // ゲームオーバーシーンが存在する場合はゲームオーバーシーンへ、存在しない場合はリスタートへ遷移する
                    if (failedSceneEntity.IsValid()) {
                        MessageBus::GetInstance()->EmitDelayed<GamefailedEvent>(GamefailedEvent(), 1.5f);
                    } else {
                        MessageBus::GetInstance()->EmitDelayed<RestartEvent>(RestartEvent(), 1.5f);
                    }

                    // プレイヤーを削除
                    GetScene()->AddDeleteEntity(_handle);
                }

                continue;
            }
        }

        // タイムスケールエフェクト と 衝突したか
        {
            TimeScaleEffectComponent* timeScaleEffect = GetComponent<TimeScaleEffectComponent>(entityId);
            if (timeScaleEffect) {
                // 当たり判定を消す。
                SphereCollider* collider = GetComponent<SphereCollider>(entityId);
                if (collider) {
                    collider->SetActive(false);
                }
                // scaleEffectを有効化
                timeScaleEffect->SetActive(true);

                Material* material = GetComponent<Material>(entityId);
                if (material) {
                    material->color_[A] = 0.f; // 透明にする
                }
            }
        }

        // 衝突情報を取得(壁の処理にはInfoがないとできない)
        auto infoItr = collisionInfoMap.find(entityId);
        if (infoItr == collisionInfoMap.end()) {
            continue;
        }

        const CollisionPushBackInfo::Info& info = infoItr->second;
        // 壁、床と 衝突したか
        if (info.pushBackType != CollisionPushBackType::PushBack) {
            continue;
        }

        OriGine::Vec3f collisionNormal = info.collFaceNormal.normalize();

        if (PlayerMoveUtils::IsHitGround(collisionNormal)) {
            // 上方向に衝突した場合は、地面にいると判断する
            if (!isPreOnGround) {
                // 前フレームは地面に居なかった → 着地した瞬間
                state->OnJustLanded();
            }
            state->OnCollisionGround();

            status->ResetWallRunInterval();

            OriGine::Vec3f acceleration = rigidbody->GetAcceleration();

            // Y軸の加速度を0にする
            acceleration[Y] = 0.f;
            rigidbody->SetAcceleration(acceleration);

            rigidbody->SetVelocity(Y, 0.f);
        } else {
            // 壁と衝突した場合
            // 壁走り可能コンポーネントの確認

            bool allowWallRun = true;

            WallRunnableComponent* wallRunnable = GetComponent<WallRunnableComponent>(entityId);
            if (!wallRunnable) {
                allowWallRun = false; // WallRunnableComponentがなければ壁走り不可
            } else {
                // 衝突法線が許可された方向かチェック
                if (!wallRunnable->IsNormalAllowed(collisionNormal)) {
                    allowWallRun = false;
                }
            }

            // 衝突法線とプレイヤーの速度ベクトルのなす角が閾値以上かチェック
            if (!allowWallRun) {
                // 法線方向の速度成分が閾値以上ならクラッシュ → ゲームオーバー
                float normalSpeed = std::fabs(rigidbody->GetVelocity().dot(collisionNormal));
                if (normalSpeed >= kCrashNormalSpeedThreshold) {
                    auto* transform = GetComponent<Transform>(_handle);
                    if (transform) {
                        PlayerExplosionEffectEvent event{};
                        event.position = transform->GetWorldTranslate();
                        MessageBus::GetInstance()->Emit<PlayerExplosionEffectEvent>(event);
                    }

                    EntityHandle failedSceneEntity = GetUniqueEntity("GameFailedScene");

                    // ゲームオーバーシーンが存在する場合はゲームオーバーシーンへ、存在しない場合はリスタートへ遷移する
                    if (failedSceneEntity.IsValid()) {
                        MessageBus::GetInstance()->EmitDelayed<GamefailedEvent>(GamefailedEvent(), 1.5f);
                    } else {
                        MessageBus::GetInstance()->EmitDelayed<RestartEvent>(RestartEvent(), 1.5f);
                    }

                    GetScene()->AddDeleteEntity(_handle);
                    return;
                }
                continue;
            }

            float dotVN = rigidbody->GetVelocity().normalize().dot(collisionNormal);

            // どれくらい平行に動いているか (1.0 = 完全に平行, 0.0 = 完全に垂直)
            float parallelFactor = 1.f - std::fabs(dotVN);

            bool isFirstCollision = true;

            if (entityId == wallEntityHandle) {
                auto collisionStateItr = collisionStateMap.find(entityId);
                if (collisionStateItr != collisionStateMap.end()) {
                    if (collisionStateItr->second == CollisionState::Stay) {
                        isFirstCollision = false;
                    }
                }
            }

            if (isFirstCollision) {
                PlayerMoveUtils::WallContactResult wallContactResult = PlayerMoveUtils::EvaluateWallContact(parallelFactor, status);
                switch (wallContactResult) {
                case PlayerMoveUtils::WallContactResult::WallRun:
                    state->OnCollisionWall(collisionNormal, entityId);
                    break;
                default:
                    break;
                }
            } else {
                // 前フレームから引き続き壁と接触している場合は、ウィリー状態を維持するかどうかを判断する
                state->OnCollisionWall(collisionNormal, wallEntityHandle);
            }
        }
    }
}
