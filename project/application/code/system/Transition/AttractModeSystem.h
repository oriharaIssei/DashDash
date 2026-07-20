#pragma once

#include "system/ISystem.h"

/// stl
#include <memory>

/// state
#include "state/AttractModeStates.h"

/// <summary>
/// アトラクションモードのシステム
/// (未入力時間が一定以上になったら、Title画面などのSubSceneを起動する)
/// </summary>
class AttractModeSystem
    : public OriGine::ISystem {
public:
    AttractModeSystem();
    ~AttractModeSystem() override = default;

    void Initialize() override;
    void Finalize() override;

private:
    /// <summary>
    /// 状態マシン(currentState_)を毎フレーム更新し、遷移条件を満たしていれば状態を切り替える。
    /// </summary>
    void Update() override;
    /// <summary>
    /// 状態(currentState_)の遷移に合わせてEnter/Exitアニメーションを再生し、
    /// 表示要否(ShouldRender)に応じてSpriteの描画有無を切り替える。
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

private:
    // 未入力状態が何秒続いたらアトラクションモードを開始するか
    static constexpr float kAttractModeThreshold_     = 30.0f;
    // シーン初期化中とみなす猶予時間
    static constexpr float kSceneInitializationDelay_ = 1.0f;

    AttractActiveState activeState_;
    AttractIdleState idleState_;
    AttractInitializingState initializingState_;

    // 現在の状態
    IAttractModeState* currentState_  = nullptr;
    // 1フレーム前の状態(Enter/Exitアニメーションの判定に使用)
    IAttractModeState* previousState_ = nullptr;
};
