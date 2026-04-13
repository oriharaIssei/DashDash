#pragma once

/// stl
#include <array>
#include <optional>
#include <string>
#include <unordered_map>

/// externals
#include <nlohmann/json.hpp>

/// math
#include <cstdint>

/// <summary>
/// ステージのクリアタイムランキングデータ
/// </summary>
struct ClearTimeRanking {
    static constexpr size_t kTopClearTimeCount = 3;
    std::array<std::optional<float>, kTopClearTimeCount> times;
};

inline void to_json(nlohmann::json& j, const ClearTimeRanking& ranking) {
    j = nlohmann::json::object();
    for (size_t i = 0; i < ClearTimeRanking::kTopClearTimeCount; ++i) {
        if (ranking.times[i].has_value()) {
            j["Time" + std::to_string(i + 1)] = ranking.times[i].value();
        } else {
            j["Time" + std::to_string(i + 1)] = nullptr;
        }
    }
}
inline void from_json(const nlohmann::json& j, ClearTimeRanking& ranking) {
    for (size_t i = 0; i < ClearTimeRanking::kTopClearTimeCount; ++i) {
        std::string key = "Time" + std::to_string(i + 1);
        if (j.contains(key) && !j[key].is_null()) {
            ranking.times[i] = j[key].get<float>();
        } else {
            ranking.times[i] = std::nullopt;
        }
    }
}

/// <summary>
/// ステージの進行状況データとランキング情報
/// </summary>
struct StageProgress {
    bool isCleared = false; // クリアしたかどうか
    ClearTimeRanking ranking;
};

/// <summary>
/// Playerの進行状況を管理するストア
/// </summary>
class PlayerProgressStore {
private:
public:
    static PlayerProgressStore* GetInstance() {
        static PlayerProgressStore instance;

        if (!instance.isInitialized_) {
            instance.Initialize();
            instance.isInitialized_ = true;
        }

        return &instance;
    }

    void Initialize();
    void Finalize();

    /// <summary>
    /// ステージプレイ開始時の処理
    /// </summary>
    /// <param name="_stage"></param>
    void StagePlayStart(int32_t _stage) {
        lastPlayStageNumber_ = _stage;
    }
    /// <summary>
    /// ステージクリア時の処理
    /// </summary>
    /// <param name="_stageNum">ステージ番号</param>
    /// <param name="_clearTime">クリアした時間</param>
    void StageCleared(int32_t _stageNum, float _clearTime);

    /// <summary>
    /// 進行状況データの読み込み
    /// </summary>
    void LoadProgressData();
    /// <summary>
    /// 進行状況データの保存
    /// </summary>
    void SaveProgressData();

private:
    PlayerProgressStore()                                      = default;
    ~PlayerProgressStore()                                     = default;
    PlayerProgressStore(const PlayerProgressStore&)            = delete;
    PlayerProgressStore& operator=(const PlayerProgressStore&) = delete;

private:
    bool isInitialized_                    = false;
    const std::string kProgressDataFolder_ = "ProgressData/";
    const char* kProgressDataFileName_     = "PlayerProgressData.json";

    int32_t lastPlayStageNumber_ = -1; // 最後にプレイしたステージ番号
    float lastPlayClearTime_     = 0.0f; // 最後にクリアした時間
    int32_t lastInRanking_       = 0; // 最後にランキング入りした順位（0 ならランキング外）

    std::unordered_map<int32_t, StageProgress> progressDataMap_; // 進行状況データリスト(clearTimeはbestTimeを残す)

public:
    const std::unordered_map<int32_t, StageProgress>& GetProgressDataMap() { return progressDataMap_; }

    bool HasProgressData(int32_t _stageNumber) const {
        return progressDataMap_.find(_stageNumber) != progressDataMap_.end();
    }

    StageProgress& GetProgressData(int32_t _stageNumber) {
        return progressDataMap_.at(_stageNumber);
    }

    int32_t GetLastPlayStageNumber() const { return lastPlayStageNumber_; }
    float GetLastPlayStageProgressData() const {
        return lastPlayClearTime_;
    }
};
