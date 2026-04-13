#include "GetClearTime.h"

/// app
#include "manager/PlayerProgressStore.h"

/// component
#include "component/TimerComponent.h"

GetClearTime::GetClearTime() : ISystem(OriGine::SystemCategory::Initialize) {}
GetClearTime::~GetClearTime() {}

void GetClearTime::Initialize() {}
void GetClearTime::Finalize() {}

void GetClearTime::UpdateEntity(OriGine::EntityHandle _handle) {
    auto timerComp = GetComponent<TimerComponent>(_handle);
    if (timerComp == nullptr) {
        return;
    }

    PlayerProgressStore* progressStore = PlayerProgressStore::GetInstance();
    float clearTime                    = progressStore->GetLastPlayStageProgressData();

    timerComp->SetMaxTime(clearTime);
    timerComp->SetCurrentTime(clearTime);
}
