#include "PlayerProgressStore.h"

/// stl
#include <algorithm>

#include <fstream>

/// engine
#define RESOURCE_DIRECTORY
#include "EngineInclude.h"

/// util
#include "myFileSystem/MyFileSystem.h"

void PlayerProgressStore::Initialize() {
    progressDataMap_.clear();
}

void PlayerProgressStore::Finalize() {
    progressDataMap_.clear();
}

void PlayerProgressStore::StageCleared(int32_t _stageNum, float _clearTime) {
    lastPlayStageNumber_ = _stageNum;

    lastPlayClearTime_ = _clearTime;

    // 進行状況データを更新する
    auto itr = progressDataMap_.find(_stageNum);
    if (itr != progressDataMap_.end()) {
        StageProgress& existingData = itr->second;
        existingData.isCleared      = true;

        // 既存のデータがある場合、クリアタイムを比較して更新する
        bool isInRanking = false;
        int rankingIndex = 1; // 順位は1から始まる
        for (auto& timeEntry : existingData.ranking.times) {
            if (!timeEntry || _clearTime < *timeEntry) {
                timeEntry      = _clearTime;
                lastInRanking_ = rankingIndex;
                isInRanking    = true;
                break;
            }
            ++rankingIndex;
        }
        if (isInRanking) {
            SaveProgressData();
        }
        return;
    }

    // データが存在しない場合、新しいデータを追加する
    progressDataMap_.emplace(_stageNum, StageProgress());
    StageProgress& newData   = progressDataMap_.at(_stageNum);
    newData.isCleared        = true;
    newData.ranking.times[0] = _clearTime;

    SaveProgressData();
}

void PlayerProgressStore::LoadProgressData() {
    std::string filePath = kApplicationResourceDirectory + "/" + kProgressDataFolder_ + kProgressDataFileName_;
    std::ifstream ifs(filePath);
    if (!ifs) {
        return;
    }
    nlohmann::json jsonData;
    ifs >> jsonData;
    ifs.close();
    auto& stagesJson = jsonData["Stages"];
    // ステージごとの進行状況データを読み込む
    for (const auto& stageData : stagesJson) {
        StageProgress progressData;
        int32_t stageNum       = stageData["StageNumber"];
        progressData.isCleared = stageData["IsCleared"];
        progressData.ranking   = stageData["ClearTimeRanking"].get<ClearTimeRanking>();
        progressDataMap_.emplace(stageNum, progressData);
    }
}

void PlayerProgressStore::SaveProgressData() {
    nlohmann::json jsonData = nlohmann::json::object();
    jsonData["StageCount"]  = progressDataMap_.size();
    jsonData["Stages"]      = nlohmann::json::array();
    auto& stagesJson        = jsonData["Stages"];
    // ステージごとの進行状況データを読み込む
    for (const auto& [stageNumber, progressData] : progressDataMap_) {
        nlohmann::json stageData      = nlohmann::json::object();
        stageData["StageNumber"]      = stageNumber;
        stageData["IsCleared"]        = progressData.isCleared;
        stageData["ClearTimeRanking"] = progressData.ranking;
        stagesJson.push_back(stageData);
    }

    // JSONファイルに書き込み
    std::string filePath = kApplicationResourceDirectory + "/" + kProgressDataFolder_ + kProgressDataFileName_;
    myFs::CreateFolder(kApplicationResourceDirectory + "/" + kProgressDataFolder_);
    myFs::DeleteMyFile(filePath); // 古いファイルを削除してから保存
    std::ofstream ofs(filePath);
    if (!ofs) {
        return;
    }
    ofs << std::setw(4) << jsonData << std::endl;
    ofs.close();
}
