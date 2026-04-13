#pragma once
#include "component/IComponent.h"

/// math
#include "math/Vector3.h"
#include "MyEasing.h"

/// <summary>
/// Playerの速度によるカメラの揺れを表すコンポーネント
/// </summary>
struct CameraMotionBob
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const CameraMotionBob& _c);
    friend void from_json(const nlohmann::json& _j, CameraMotionBob& _c);

public:
    CameraMotionBob();
    ~CameraMotionBob() override;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

public:
    int32_t cameraShakeSourceComponentId = -1; ///< カメラシェイクソースコンポーネントID
    int32_t thresholdGearLevel           = 3; ///< 揺れを開始するギアレベル

    OriGine::Vec3f minAmplitude = OriGine::Vec3f(0.02f, 0.02f, 0.0f); ///< 最小振幅
    OriGine::Vec3f maxAmplitude = OriGine::Vec3f(0.1f, 0.1f, 0.0f); ///< 最大振幅
    OriGine::Vec3f minFrequency = OriGine::Vec3f(10.0f, 20.0f, 0.0f); ///< 最小周波数
    OriGine::Vec3f maxFrequency = OriGine::Vec3f(30.0f, 50.0f, 0.0f); ///< 最大周波数
};
