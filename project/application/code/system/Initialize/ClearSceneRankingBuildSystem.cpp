#include "ClearSceneRankingBuildSystem.h"

/// app
#include "manager/PlayerProgressStore.h"

/// ECS
// component
#include "component/TimerComponent.h"

using namespace OriGine;

ClearSceneRankingBuildSystem::ClearSceneRankingBuildSystem() : ISystem(SystemCategory::Initialize) {}
ClearSceneRankingBuildSystem::~ClearSceneRankingBuildSystem() {}

void ClearSceneRankingBuildSystem::Initialize() {}
void ClearSceneRankingBuildSystem::Finalize() {}

void ClearSceneRankingBuildSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    int32_t stageNumber          = 0;
    StageProgress* stageProgress = nullptr;

    { // プレイしたステージの進行状況データを取得
        PlayerProgressStore* progressStore = PlayerProgressStore::GetInstance();
        stageNumber                        = progressStore->GetLastPlayStageNumber();
        if (progressStore->HasProgressData(stageNumber)) {
            stageProgress = &progressStore->GetProgressData(stageNumber);
        }
    }

    // 進行状況データがない場合は処理しない
    if (stageProgress == nullptr) {
        return;
    }

    // Timerコンポーネントを取得して、ランキングタイムをセットする(3つ)
    if (TimerComponent* timerComp = GetComponent<TimerComponent>(_handle)) {
        for (size_t i = 0; i < ClearTimeRanking::kTopClearTimeCount; ++i) {
            if (stageProgress->ranking.times[i].has_value()) {
                timerComp->SetMaxTime(stageProgress->ranking.times[i].value());
                timerComp->SetCurrentTime(stageProgress->ranking.times[i].value());
            }
        }
    }
}
