#include "PlayerAheadCollisionReactionSystem.h"

/// ECS
// component
#include "component/collision/collider/SphereCollider.h"
#include "component/collision/CollisionPushBackInfo.h"
#include "component/physics/Rigidbody.h"
#include "component/player/PlayerEffectControlParam.h"
#include "component/player/PlayerStatus.h"
#include "component/player/State/PlayerState.h"

/// util
#include "component/player/PlayerMoveUtils.h"

/// math
#include "MathEnv.h"
#include "MyEasing.h"

using namespace OriGine;

PlayerAheadCollisionReactionSystem::PlayerAheadCollisionReactionSystem() : ISystem(SystemCategory::Collision) {}

void PlayerAheadCollisionReactionSystem::Initialize() {}
void PlayerAheadCollisionReactionSystem::Finalize() {}

void PlayerAheadCollisionReactionSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    EntityHandle playerHandle                    = GetUniqueEntity("Player");
    PlayerState* playerState                     = GetComponent<PlayerState>(playerHandle);
    SphereCollider* playerCollider               = GetComponent<SphereCollider>(playerHandle);
    Rigidbody* playerRigidbody                   = GetComponent<Rigidbody>(playerHandle);
    PlayerStatus* playerStatus                   = GetComponent<PlayerStatus>(playerHandle);
    Transform* playerTransform                   = GetComponent<Transform>(playerHandle);
    PlayerEffectControlParam* effectControlParam = GetComponent<PlayerEffectControlParam>(playerHandle);

    if (!playerState || !playerStatus || !playerCollider || !playerRigidbody || !playerTransform || !effectControlParam) {
        LOG_WARN("PlayerState or Player SphereCollider component is not found. PlayerAheadCollisionReactionSystem will not work.");
        return;
    }

    auto state = playerState->GetStateEnum();
    if (state != PlayerMoveState::FALL_DOWN && state != PlayerMoveState::JUMP) {
        return;
    }

    SphereCollider* collider            = GetComponent<SphereCollider>(_handle);
    CollisionPushBackInfo* pushBackInfo = GetComponent<CollisionPushBackInfo>(_handle);

    if (!collider || !pushBackInfo) {
        return;
    }

    float radiusDiff       = collider->GetWorldRadius() - playerCollider->GetWorldRadius();
    float penetrationDepth = 0.f;
    Vec3f collNormal       = Vec3f();
    for (const auto& [handle, info] : pushBackInfo->GetCollisionInfoMap()) {
        if (info.pushBackType == CollisionPushBackType::None || PlayerMoveUtils::IsHitGround(info.collFaceNormal)) {
            continue;
        }
        float dotVN = playerRigidbody->GetVelocity().normalize().dot(info.collFaceNormal);

        // どれくらい平行に動いているか (1.0 = 完全に平行, 0.0 = 完全に垂直)
        float parallelFactor                       = 1.f - std::fabs(dotVN);
        PlayerMoveUtils::WallContactResult contact = PlayerMoveUtils::EvaluateWallContact(parallelFactor, playerStatus);
        if (contact == PlayerMoveUtils::WallContactResult::WallHit) {
            continue;
        }

        // 衝突の深さ
        // 押し戻し量を深度を深度として扱う
        if (penetrationDepth < info.collVec.length()) {
            collNormal       = info.collFaceNormal.normalize();
            penetrationDepth = info.collVec.length();
        }
    }

    // wallRunの時の処理
    float maxAngle = effectControlParam->GetAheadCollisionTiltAngle();

    float currAngle  = effectControlParam->GetAheadCollisionCurrentAngle();
    float afterAngle = 0.f;
    float tiltSpeed  = effectControlParam->GetAheadCollisionTiltSpeed() * Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime(playerRigidbody->GetLocalDeltaTimeName());
    float deltaAngle = 0.f;

    Vec3f wallFrontDir = PlayerMoveUtils::ComputeWallRunDirection(playerRigidbody->GetVelocity(), collNormal);
    bool isRightWall   = PlayerMoveUtils::IsWallRight(wallFrontDir, collNormal);

    if (penetrationDepth >= OriGine::kEpsilon) {
        float t = std::clamp(penetrationDepth / radiusDiff, 0.f, 1.f);
        // wallRunの時の処理
        afterAngle = std::lerp(currAngle, isRightWall ? maxAngle : -maxAngle, EaseOutCubic(t));
    }
    deltaAngle = std::clamp(afterAngle - currAngle, -tiltSpeed, tiltSpeed);
    currAngle += deltaAngle;
    effectControlParam->SetAheadCollisionCurrentAngle(currAngle);

    Quaternion angleOffset = Quaternion::RotateAxisAngle(axisZ, currAngle);
    playerTransform->rotate *= angleOffset;
}
