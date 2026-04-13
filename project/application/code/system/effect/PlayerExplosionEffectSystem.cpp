#include "PlayerExplosionEffectSystem.h"

/// engine
#include "component/physics/Rigidbody.h"
#include "messageBus/MessageBus.h"
#include "scene/SceneFactory.h"

/// ECS
// component
#include "component/animation/TransformRateAnimation.h"
#include "component/camera/CameraShakeSourceComponent.h"
#include "component/transform/Transform.h"

/// math
#include "MathEnv.h"

/// util
#include "myRandom/MyRandom.h"

using namespace OriGine;

namespace {
constexpr float kMinScatterSpeed    = 10.0f;
constexpr float kMaxScatterSpeed    = 30.0f;
constexpr float kMinRotateSpeed     = kPi;        // 最低回転速度（rad/s）= 180°/s
constexpr float kMaxRotateSpeed     = kTau * 2.0f; // 最大回転速度（rad/s）= 720°/s
constexpr float kFullRotationRange  = kTau;        // 初期回転のランダム範囲（0 ~ 2π）
} // namespace

PlayerExplosionEffectSystem::PlayerExplosionEffectSystem() : ISystem(SystemCategory::Effect) {}
PlayerExplosionEffectSystem::~PlayerExplosionEffectSystem() {}

void PlayerExplosionEffectSystem::Initialize() {
    auto shared                                         = shared_from_this();
    std::weak_ptr<PlayerExplosionEffectSystem> weakSelf = shared;
    eventSubscriptionId_ =
        MessageBus::GetInstance()->Subscribe<PlayerExplosionEffectEvent>(
            [weakSelf](const PlayerExplosionEffectEvent& _event) {
                if (auto self = weakSelf.lock()) {
                    self->pendingEvents_.push_back(_event);
                }
            });
}

void PlayerExplosionEffectSystem::Finalize() {
    MessageBus::GetInstance()->Unsubscribe<PlayerExplosionEffectEvent>(eventSubscriptionId_);
}

void PlayerExplosionEffectSystem::Update() {
    SceneFactory factory = SceneFactory();
    MyRandom::Float speedRand(kMinScatterSpeed, kMaxScatterSpeed);
    MyRandom::Float dirRand(-1.0f, 1.0f);
    MyRandom::Float rotateInitRand(0.0f, kFullRotationRange);
    MyRandom::Float rotateSpeedRand(kMinRotateSpeed, kMaxRotateSpeed);

    // パーツにランダムな初期回転とランダムな回転速度を設定するヘルパー
    auto applyRandomSpin = [&](EntityHandle _handle) {
        Transform* trans = GetComponent<Transform>(_handle);
        if (trans) {
            trans->rotate = Quaternion::FromEulerAngles(
                rotateInitRand.Get(), rotateInitRand.Get(), rotateInitRand.Get());
        }

        TransformRateAnimation* rateAnim = GetComponent<TransformRateAnimation>(_handle);
        if (rateAnim) {
            // 各軸にランダムな符号付き回転速度を設定
            Vec3f angularVelocity(
                rotateSpeedRand.Get() * (dirRand.Get() < 0.0f ? -1.0f : 1.0f),
                rotateSpeedRand.Get() * (dirRand.Get() < 0.0f ? -1.0f : 1.0f),
                rotateSpeedRand.Get() * (dirRand.Get() < 0.0f ? -1.0f : 1.0f));
            rateAnim->SetRotateRate({angularVelocity, {}});
            rateAnim->SetIsPlay(true);
        }
    };

    for (const auto& event : pendingEvents_) {
        Entity* expEnt = factory.BuildEntityFromTemplate(GetScene(), "ExplosionEffect");
        if (expEnt) {
            Transform* expTrans = GetComponent<Transform>(expEnt->GetHandle());
            expTrans->translate = event.position;
        }
        Entity* expSmokeEnt = factory.BuildEntityFromTemplate(GetScene(), "ExplosionSmokeEffect");
        if (expSmokeEnt) {
            Transform* smokeTrans = GetComponent<Transform>(expSmokeEnt->GetHandle());
            smokeTrans->translate = event.position;
        }

        // Tireをランダムな方向に飛ばす
        Entity* playerTireEnt = factory.BuildEntityFromTemplate(GetScene(), "PlayerPartsTire");
        if (playerTireEnt) {
            Transform* tireTrans = GetComponent<Transform>(playerTireEnt->GetHandle());
            if (tireTrans) {
                tireTrans->translate = event.position;
            }

            Rigidbody* rb = GetComponent<Rigidbody>(playerTireEnt->GetHandle());
            if (rb) {
                float speed = speedRand.Get();
                Vec3f dir(dirRand.Get(), 0.0f, dirRand.Get());
                dir = dir.normalize() * speed;
                rb->SetVelocity(X, dir[X]);
                rb->SetVelocity(Z, dir[Z]);
            }

            applyRandomSpin(playerTireEnt->GetHandle());
        }

        // マフラーは左右3個ずつ
        for (int32_t i = 0; i < 6; ++i) {
            Entity* mufflerEnt = factory.BuildEntityFromTemplate(GetScene(), "PlayerPartsMufflerL");
            if (!mufflerEnt) {
                continue;
            }

            Transform* mufflerTrans = GetComponent<Transform>(mufflerEnt->GetHandle());
            if (mufflerTrans) {
                mufflerTrans->translate = event.position;
            }

            Rigidbody* rb = GetComponent<Rigidbody>(mufflerEnt->GetHandle());
            if (rb) {
                float speed = speedRand.Get();
                Vec3f dir(dirRand.Get(), 0.0f, dirRand.Get());
                dir = dir.normalize() * speed;
                rb->SetVelocity(0, dir[X]);
                rb->SetVelocity(2, dir[Z]);
            }

            applyRandomSpin(mufflerEnt->GetHandle());
        }

        // カメラシェイク
        constexpr int32_t kExplosionCameraShakeSourceIndex = 2; // ExplosionEffectのCameraShakeSourceComponentは0番目のTransformにアタッチされている
        EntityHandle gameCamera                            = GetUniqueEntity("GameCamera");
        CameraShakeSourceComponent* shakeSource            = GetComponent<CameraShakeSourceComponent>(gameCamera, kExplosionCameraShakeSourceIndex);
        if (shakeSource) {
            shakeSource->StartShake();
        }
    }
    pendingEvents_.clear();
}
