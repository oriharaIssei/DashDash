#include "TutorialColliderOnCollision.h"

/// ECS
// component
#include "component/animation/SpriteAnimation.h"
#include "component/collision/collider/AABBCollider.h"
#include "component/renderer/Sprite.h"

using namespace OriGine;

TutorialColliderOnCollision::TutorialColliderOnCollision() : ISystem(OriGine::SystemCategory::Collision) {}
TutorialColliderOnCollision::~TutorialColliderOnCollision() {}

void TutorialColliderOnCollision::Initialize() {
    playerEntityHandle_ = OriGine::EntityHandle();
}

void TutorialColliderOnCollision::Finalize() {
    playerEntityHandle_ = OriGine::EntityHandle();
}

void TutorialColliderOnCollision::Update() {
    playerEntityHandle_ = GetUniqueEntity("Player");
    ISystem::Update();
}

void TutorialColliderOnCollision::UpdateEntity(OriGine::EntityHandle _handle) {
    if (!_handle.IsValid()) {
        return;
    }

    int32_t colliderIndex  = 0;
    bool playerIsColliding = false;

    // 衝突したコライダーを探す
    auto& aabbColliders = GetComponents<AABBCollider>(_handle);
    for (auto& aabbCollider : aabbColliders) {
        for (const auto& [handle, state] : aabbCollider.GetCollisionStateMap()) {
            if (handle != playerEntityHandle_) {
                continue;
            }
            playerIsColliding = true;

            // 衝突開始時,終了時にそれぞれのSpriteアニメーションを再生
            auto sprite = GetComponent<SpriteRenderer>(_handle);
            if (sprite != nullptr) {
                // 衝突開始時にSpriteのアニメーションを再生
                if (state == CollisionState::Enter) {
                    auto spriteAnimation = GetComponent<SpriteAnimation>(_handle);
                    if (spriteAnimation != nullptr) {
                        spriteAnimation->PlayColorAnimation();
                    }
                } else if (state == CollisionState::Exit) { // 衝突終了時にSpriteのアニメーションを再生
                    auto spriteAnimation = GetComponent<SpriteAnimation>(_handle, 1);
                    if (spriteAnimation != nullptr) {
                        spriteAnimation->PlayColorAnimation();
                    }
                }
            }
            break;
        }
        if (playerIsColliding) {
            break;
        }
        ++colliderIndex;
    }
}
