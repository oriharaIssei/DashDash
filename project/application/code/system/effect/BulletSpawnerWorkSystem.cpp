#include "BulletSpawnerWorkSystem.h"

/// engine
#include "Engine.h"
#include "scene/SceneFactory.h"

/// ECS
#include "component/gimmick/BulletSpawner.h"
#include "component/physics/Rigidbody.h"
#include "component/transform/Transform.h"
#include "ECS/HandleAssignMode.h"

using namespace OriGine;

void BulletSpawnerWorkSystem::Initialize() {}

void BulletSpawnerWorkSystem::Finalize() {
    entities_.clear();
}

void BulletSpawnerWorkSystem::UpdateEntity(EntityHandle _handle) {
    const float deltaTime = Engine::GetInstance()->GetDeltaTime();

    auto& spawners = GetComponents<BulletSpawner>(_handle);
    if (spawners.empty()) {
        return;
    }

    SceneFactory factory;

    for (auto& spawner : spawners) {
        if (!spawner.IsActive()) {
            continue;
        }

        if (spawner.templateTypeName_.empty()) {
            continue;
        }

        // スポーン数を取得
        int32_t spawnCount = spawner.emitter_.Update(deltaTime);

        // ワールド原点を更新
        spawner.emitter_.SetParent(GetComponent<Transform>(spawner.emitter_.GetParentHandle()));
        spawner.emitter_.UpdateWorldOriginPos();

        // Spawner の Emitter の parent Transform からワールド回転を取得
        Quaternion spawnerRotation = Quaternion::Identity();
        if (auto* parentTransform = spawner.emitter_.GetParent()) {
            spawnerRotation = parentTransform->CalculateWorldRotate();
        }

        for (int32_t i = 0; i < spawnCount; ++i) {
            Entity* spawned = factory.BuildEntityFromTemplate(
                GetScene(),
                spawner.templateTypeName_,
                HandleAssignMode::GenerateNew);

            if (!spawned) {
                continue;
            }
            spawned->SetShouldSave(false);

            // スポーン位置を反映
            auto* transform = GetComponent<Transform>(spawned->GetHandle());
            if (transform) {
                transform->translate = spawner.emitter_.GetInterpolatedOriginPos(i, spawnCount) + spawner.emitter_.GetSpawnPos();
            }

            // Rigidbody の Velocity を Spawner の回転で回転させる
            auto* rigidbody = GetComponent<Rigidbody>(spawned->GetHandle());
            if (rigidbody) {
                Vec3f rotatedVelocity = spawnerRotation.RotateVector(rigidbody->GetVelocity());
                rigidbody->SetVelocity(rotatedVelocity);
            }
        }

        // 時間切れなら非アクティブ化
        if (spawner.emitter_.IsExpired()) {
            spawner.emitter_.Deactivate();
        }
    }
}
