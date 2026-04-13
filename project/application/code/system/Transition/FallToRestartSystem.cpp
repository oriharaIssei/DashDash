#include "FallToRestartSystem.h"

/// engine
#include "messageBus/MessageBus.h"

/// event
#include "event/RestartEvent.h"

void FallToRestartSystem::OnFall() {
    MessageBus::GetInstance()->Emit<RestartEvent>(RestartEvent());
}
