#include "PlayerMoveSystem.h"

/// engine
#include "Engine.h"

// component
#include "component/player/PlayerInput.h"
#include "component/player/state/IPlayerMoveState.h"
#include "component/player/state/PlayerState.h"

void PlayerMoveSystem::Initialize() {}
void PlayerMoveSystem::Finalize() {}

void PlayerMoveSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    PlayerState* state = GetComponent<PlayerState>(_handle);

    const float deltaTime = OriGine::Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("Player");
    auto moveState        = state->GetPlayerMoveState();
    if (moveState) {
        moveState->Update(deltaTime);
    }
}
