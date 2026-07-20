#pragma once
#include "system/ISystem.h"

/// <summary>
/// GoalPositionの位置に移動させるシステム
/// </summary>
class TakeToGoalPosition
    : public OriGine::ISystem {
public:
    TakeToGoalPosition();
    ~TakeToGoalPosition() override;
    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize() override;

protected:
    /// <summary>
    /// "GoalPosition"エンティティの位置を、対象エンティティのTransformとPointLightへ反映する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
