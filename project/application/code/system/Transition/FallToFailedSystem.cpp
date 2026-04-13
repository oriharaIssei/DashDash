#include "FallToFailedSystem.h"

/// engine
#include "messageBus/MessageBus.h"

/// event
#include "event/GamefailedEvent.h"

void FallToFailedSystem::OnFall() {
    MessageBus::GetInstance()->Emit<GamefailedEvent>(GamefailedEvent());
}
