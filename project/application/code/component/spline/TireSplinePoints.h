#pragma once
#include "component/IComponent.h"

/// stl
#include <deque>

/// spline
#include "component/spline/SplinePoints.h"

/// math
#include "math/MyEasing.h"
#include "math/Vector3.h"

/// <summary>
/// Splineの制御点を管理するコンポーネント
/// </summary>
struct TireSplinePoints
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const TireSplinePoints& c);
    friend void from_json(const nlohmann::json& j, TireSplinePoints& c);

public:
    TireSplinePoints();
    ~TireSplinePoints() override;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

public:
    /// <summary>
    /// タイアスプラインの制御点
    /// </summary>
    struct ControlPoint {
        OriGine::Vec3f position = OriGine::Vec3f(0.0f, 0.0f, 0.0f);
        float alpha             = 1.0f;
    };

public:
    SplineCommonSettings commonSettings;

    int32_t capacity = 100;

    OriGine::EaseType speedIntensityEaseType =
        OriGine::EaseType::EaseInOutSine;

    float startMoveFactor = 1.0f;

    float minSpeedFactor = 0.1f;
    float maxSpeedFactor = 1.0f;

    float thresholdBankAngle = 0.f; // この角度を超えたらバンクとして、効果を適用する
    float minBankFactor      = 1.0f;
    float maxBankFactor      = 1.0f;
    float groundedFactor     = 1.0f; // 着地時の効果倍率
    float gearupFactor       = 1.0f; // ギアアップ時の効果倍率

    std::deque<ControlPoint> points;

public:
    /// <summary>
    /// 制御点を追加する(capacityを超過した場合,popfrontされる)
    /// </summary>
    /// <param name="_point"></param>
    void PushPoint(const OriGine::Vec3f& _point, float _alpha = 1.f);
};
