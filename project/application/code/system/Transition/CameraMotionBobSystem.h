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
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
