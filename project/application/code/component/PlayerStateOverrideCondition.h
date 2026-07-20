#pragma once
#include "component/IComponent.h"

/// ECS
// component
#include "component/player/State/PlayerState.h"

/// util
#include "EnumBitMask.h"

/// <summary>
/// 入力をオーバーライドする条件コンポーネント
/// </summary>
class PlayerStateOverrideCondition
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const PlayerStateOverrideCondition& c);
    friend void from_json(const nlohmann::json& j, PlayerStateOverrideCondition& c);

public:
    PlayerStateOverrideCondition();
    ~PlayerStateOverrideCondition() override;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    /// <summary>
    /// エディタ用編集UI
    /// </summary>
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _handle, const ::std::string& _parentLabel) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

public:
    const EnumBitmask<PlayerMoveState>& GetOverrideConditions() const { return overrideConditions; }
    void SetOverrideConditions(const EnumBitmask<PlayerMoveState>& _overrideConditions) { overrideConditions = _overrideConditions; }

private:
    /// 入力をオーバーライドする条件
    EnumBitmask<PlayerMoveState> overrideConditions;
};
