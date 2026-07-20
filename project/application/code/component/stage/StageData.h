#pragma once
#include "component/IComponent.h"

/// <summary>
/// ステージに関するデータを管理するコンポーネント
/// </summary>
class StageData
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const StageData& component);
    friend void from_json(const nlohmann::json& j, StageData& component);

public:
    StageData()           = default;
    ~StageData() override = default;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// エディタ用編集UI
    /// </summary>
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;

private:
    int32_t stageNumber_ = 1; // ステージ番号
    int32_t difficulty_  = 1; // 難易度

    float clearTime_ = 0.0f; // クリアタイム
public:
    int32_t GetStageNumber() const { return stageNumber_; }
    void SetStageNumber(int32_t _stageNumber) { stageNumber_ = _stageNumber; }

    int32_t GetDifficulty() const { return difficulty_; }
    void SetDifficulty(int32_t _difficulty) { difficulty_ = _difficulty; }

    float GetClearTime() const { return clearTime_; }
    void SetClearTime(float _clearTime) { clearTime_ = _clearTime; }
};
