#include "CreateSpriteFromTimer.h"

/// engine
#include "Engine.h"
#include "scene/Scene.h"
#include "winApp/WinApp.h"

/// ECS
// component
#include "component/animation/SpriteAnimation.h"
#include "component/renderer/Sprite.h"
#include "component/TimerComponent.h"
#include "component/TimerForSpriteComponent.h"
// system
#include "system/render/SpriteRenderSystem.h"
#include "system/SystemRunner.h"

using namespace OriGine;

CreateSpriteFromTimer::CreateSpriteFromTimer()
    : ISystem(OriGine::SystemCategory::Initialize) {}
CreateSpriteFromTimer::~CreateSpriteFromTimer() {}

void CreateSpriteFromTimer::Initialize() {}
void CreateSpriteFromTimer::Finalize() {}

void CreateSpriteFromTimer::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& timerForSpriteComponents   = GetComponents<TimerForSpriteComponent>(_handle);

    if (timerForSpriteComponents.empty()) {
        return; // タイマーコンポーネントがない場合は何もしない
    }

    for (auto& comp : timerForSpriteComponents) {
        // Sprite用のEntityを作成
        EntityHandle spriteEntityId   = CreateEntity("TimerForSprite_Sprites", false);
        OriGine::Entity* spriteEntity = GetEntity(spriteEntityId);
        spriteEntity->SetShouldSave(false); // セーブしない

        // sprite の生成と配置
        CreateSprites(spriteEntityId, &comp);
    }
}

void CreateSpriteFromTimer::CreateSprites(OriGine::EntityHandle _handle, TimerForSpriteComponent* _forSpriteComp) {
    // Sprite用のEntityを作成
    GetScene()->GetSystemRunnerRef()->GetSystemRef<SpriteRenderSystem>()->AddEntity(_handle);

    _forSpriteComp->SetSpritesEntityHandle(_handle);

    GetComponentArray<SpriteRenderer>()->RegisterEntity(_handle);

    _forSpriteComp->RelocationSprites(GetScene());
}
