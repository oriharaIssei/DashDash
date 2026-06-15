#pragma once
#include "component/IComponent.h"

/// math
#include "math/Vector3.h"
#include "MyEasing.h"

/// <summary>
/// Playerの速度によるカメラの揺れを表すコンポーネント
/// </summary>
class CameraMotionBob
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const CameraMotionBob& _c);
    friend void from_json(const nlohmann::json& _j, CameraMotionBob& _c);

public:
    CameraMotionBob();
    ~CameraMotionBob() override;

    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;
    void Finalize() override;

public:
    int32_t GetCameraShakeSourceComponentId() const { return cameraShakeSourceComponentId_; }
    void SetCameraShakeSourceComponentId(int32_t _id) { cameraShakeSourceComponentId_ = _id; }

    int32_t GetThresholdGearLevel() const { return thresholdGearLevel_; }
    void SetThresholdGearLevel(int32_t _level) { thresholdGearLevel_ = _level; }

    const OriGine::Vec3f& GetMinAmplitude() const { return minAmplitude_; }
    const OriGine::Vec3f& GetMaxAmplitude() const { return maxAmplitude_; }
    const OriGine::Vec3f& GetMinFrequency() const { return minFrequency_; }
    const OriGine::Vec3f& GetMaxFrequency() const { return maxFrequency_; }

private:
    int32_t cameraShakeSourceComponentId_ = -1; ///< カメラシェイクソースコンポーネントID
    int32_t thresholdGearLevel_           = 3; ///< 揺れを開始するギアレベル

    OriGine::Vec3f minAmplitude_ = OriGine::Vec3f(0.02f, 0.02f, 0.0f); ///< 最小振幅
    OriGine::Vec3f maxAmplitude_ = OriGine::Vec3f(0.1f, 0.1f, 0.0f); ///< 最大振幅
    OriGine::Vec3f minFrequency_ = OriGine::Vec3f(10.0f, 20.0f, 0.0f); ///< 最小周波数
    OriGine::Vec3f maxFrequency_ = OriGine::Vec3f(30.0f, 50.0f, 0.0f); ///< 最大周波数
};
