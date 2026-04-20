#include "OnCollisionModifierTargetSystem.h"

/// stl
#include <cfloat>
#include <unordered_set>

/// engine
#include "scene/Scene.h"

/// ECS
// component
#include "component/collision/collider/AABBCollider.h"
#include "component/collision/collider/ObbCollider.h"
#include "component/collision/collider/SphereCollider.h"
#include "component/physics/Rigidbody.h"
#include "component/SpeedModifiers.h"
// system
#include "system/Movement/ApplySpeedModifiers.h"

/// event
#include "event/AddForceEvent.h"

using namespace OriGine;

OnCollisionModifierTargetSystem::OnCollisionModifierTargetSystem() : ISystem(SystemCategory::Collision) {}
OnCollisionModifierTargetSystem::~OnCollisionModifierTargetSystem() {}

void OnCollisionModifierTargetSystem::Initialize() {}
void OnCollisionModifierTargetSystem::Finalize() {}

void OnCollisionModifierTargetSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    auto speedModifiers = GetComponent<SpeedModifiers>(_handle);
    if (!speedModifiers) {
        return;
    }

    // コライダーから衝突状態を収集
    auto& sphereColliders = GetComponents<SphereCollider>(_handle);
    auto& aabbColliders   = GetComponents<AABBCollider>(_handle);
    auto& obbColliders    = GetComponents<OBBCollider>(_handle);

    // 衝突状態ごとにエンティティを分類
    std::unordered_set<EntityHandle> enterEntities;
    std::unordered_set<EntityHandle> exitEntities;

    auto collectStates = [&](auto& _colliders) {
        for (auto& collider : _colliders) {
            for (const auto& [otherHandle, state] : collider.GetCollisionStateMap()) {
                if (state == CollisionState::Enter) {
                    enterEntities.insert(otherHandle);
                } else if (state == CollisionState::Exit) {
                    exitEntities.insert(otherHandle);
                }
            }
        }
    };
    collectStates(sphereColliders);
    collectStates(aabbColliders);
    collectStates(obbColliders);

    const bool isWhileColliding = (speedModifiers->GetTriggerMode() == SpeedModifierTriggerMode::WhileColliding);

    // --- Enter 処理 ---
    for (const auto& otherHandle : enterEntities) {
        // WhileCollidingモードで既にModifierが存在する場合はスキップ
        if (isWhileColliding) {
            auto key = std::make_pair(_handle, otherHandle);
            if (activeModifiers_.count(key) > 0) {
                continue;
            }
        }

        auto rigidbodyComp = GetComponent<Rigidbody>(otherHandle);
        if (!rigidbodyComp) {
            continue;
        }

        EntityHandle createdEntity = CreateEntity("SpeedModifier");
        ComponentHandle modifier   = AddComponent<SpeedModifiers>(createdEntity);
        auto* modifierComp         = GetComponent<SpeedModifiers>(modifier);
        if (modifierComp) {
            *modifierComp = *speedModifiers;
            modifierComp->SetTargetRigidbodyHandle(rigidbodyComp->GetHandle());
            modifierComp->SetBeforeSpeed(rigidbodyComp->GetVelocity().length());

            // WhileCollidingモードではdurationを無限にしてHoldフェーズに留まらせる
            if (isWhileColliding) {
                modifierComp->SetAdditiveDuration(FLT_MAX);
                modifierComp->SetMultiplierDuration(FLT_MAX);
                modifierComp->SetIsAutoDestroyed(true);

                activeModifiers_[std::make_pair(_handle, otherHandle)] = createdEntity;
            }

            // systemに登録
            GetScene()->GetSystem(nameof<ApplySpeedModifiers>())->AddEntity(createdEntity);
        }
    }

    // --- Exit 処理 (WhileCollidingモードのみ) ---
    if (isWhileColliding) {
        for (const auto& otherHandle : exitEntities) {
            auto key = std::make_pair(_handle, otherHandle);
            auto it  = activeModifiers_.find(key);
            if (it == activeModifiers_.end()) {
                continue;
            }

            // ModifierエンティティのタイマーをFadeOutフェーズに移行させる
            auto& modifiers = GetComponents<SpeedModifiers>(it->second);
            for (auto& mod : modifiers) {
                mod.SetAdditiveTimer(mod.GetAdditiveDuration());
                mod.SetMultiplierTimer(mod.GetMultiplierDuration());
            }

            activeModifiers_.erase(it);
        }
    }
}
