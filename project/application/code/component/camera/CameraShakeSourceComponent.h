#pragma once

#include "component/IComponent.h"

/// math
#include "math/Vector3.h"

/// <summary>
/// シェイクの種類
/// </summary>
enum class ShakeSourceType {
    SinCurve, // サインカーブ
    Noise, // ノイズ
    Spring // バネ
};

/// <summary>
/// カメラシェイクの発生源コンポーネント
/// </summary>
struct CameraShakeSourceComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const CameraShakeSourceComponent& _c);
    friend void from_json(const nlohmann::json& _j, CameraShakeSourceComponent& _c);

public:
    CameraShakeSourceComponent();
    ~CameraShakeSourceComponent() override;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

    void StartShake();
    void StopShake();

public:
    /// <summary>
    /// シェイクのパラメータを設定する構造体
    /// </summary>
    struct ShakeParameters {
        float amplitude    = 1.0f; // シェイクの強さ
        float frequency    = 1.0f; // シェイクの速さ (Hz)
        float dampingRatio = 0.3f; // 減衰比 (0<ζ<1 で振動する、1で臨界減衰)
    };

public:
    bool isActive                = true; // シェイクの有効無効
    ShakeSourceType type         = ShakeSourceType::SinCurve; // シェイクの種類
    int32_t cameraTransformIndex = -1; // シェイクを適用するカメラのTransformコンポーネントインデックス

    OriGine::Vec3<ShakeParameters> axisParameters = {
        {1.0f, 1.0f}, // X軸のパラメータ
        {1.0f, 1.0f}, // Y軸のパラメータ
        {1.0f, 1.0f} // Z軸のパラメータ
    };

    bool isLoop       = true; // シェイクのループ有無
    float duration    = 1.0f; // シェイクの継続時間
    float elapsedTime = 0.0f; // 経過時間

    // Spring 用の状態 (ランタイムのみ・シリアライズ不要)
    OriGine::Vec3f springPosition = OriGine::Vec3f(0.0f, 0.0f, 0.0f); // バネの現在変位
    OriGine::Vec3f springVelocity = OriGine::Vec3f(0.0f, 0.0f, 0.0f); // バネの現在速度
};
