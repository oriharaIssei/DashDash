#pragma once
#include "system/ISystem.h"

/// <summary>
/// CameraMotionBob でカメラを揺らすシステム
/// </summary>
class CameraMotionBobSystem
    : public OriGine::ISystem {
public:
    CameraMotionBobSystem();
    ~CameraMotionBobSystem() override = default;

    void Initialize() override;
    void Finalize() override;

protected:
    /// <summary>
    /// PlayerのギアレベルとCameraMotionBobの閾値を比較し、カメラシェイクの有効/無効と
    /// 振幅・周波数(ギアレベルに応じて線形補間)をCameraShakeSourceComponentへ反映する。
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
