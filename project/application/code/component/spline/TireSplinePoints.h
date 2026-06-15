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
class TireSplinePoints
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const TireSplinePoints& c);
    friend void from_json(const nlohmann::json& j, TireSplinePoints& c);

public:
    TireSplinePoints();
    ~TireSplinePoints() override;

    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;
    void Finalize() override;

public:
    /// <summary>
    /// タイアスプラインの制御点
    /// </summary>
    struct ControlPoint {
        OriGine::Vec3f position = OriGine::Vec3f(0.0f, 0.0f, 0.0f);
        float alpha             = 1.0f;
    };

    /// <summary>
    /// 制御点を追加する(capacityを超過した場合,popfrontされる)
    /// </summary>
    /// <param name="_point"></param>
    void PushPoint(const OriGine::Vec3f& _point, float _alpha = 1.f);

public:
    SplineCommonSettings& GetCommonSettings() { return commonSettings; }
    const SplineCommonSettings& GetCommonSettings() const { return commonSettings; }

    int32_t GetCapacity() const { return capacity; }
    void SetCapacity(int32_t _capacity) { capacity = _capacity; }

    OriGine::EaseType GetSpeedIntensityEaseType() const { return speedIntensityEaseType; }
    void SetSpeedIntensityEaseType(OriGine::EaseType _speedIntensityEaseType) { speedIntensityEaseType = _speedIntensityEaseType; }

    float GetStartMoveFactor() const { return startMoveFactor; }
    void SetStartMoveFactor(float _startMoveFactor) { startMoveFactor = _startMoveFactor; }

    float GetMinSpeedFactor() const { return minSpeedFactor; }
    void SetMinSpeedFactor(float _minSpeedFactor) { minSpeedFactor = _minSpeedFactor; }

    float GetMaxSpeedFactor() const { return maxSpeedFactor; }
    void SetMaxSpeedFactor(float _maxSpeedFactor) { maxSpeedFactor = _maxSpeedFactor; }

    float GetThresholdBankAngle() const { return thresholdBankAngle; }
    void SetThresholdBankAngle(float _thresholdBankAngle) { thresholdBankAngle = _thresholdBankAngle; }

    float GetMinBankFactor() const { return minBankFactor; }
    void SetMinBankFactor(float _minBankFactor) { minBankFactor = _minBankFactor; }

    float GetMaxBankFactor() const { return maxBankFactor; }
    void SetMaxBankFactor(float _maxBankFactor) { maxBankFactor = _maxBankFactor; }

    float GetGroundedFactor() const { return groundedFactor; }
    void SetGroundedFactor(float _groundedFactor) { groundedFactor = _groundedFactor; }

    float GetGearupFactor() const { return gearupFactor; }
    void SetGearupFactor(float _gearupFactor) { gearupFactor = _gearupFactor; }

    std::deque<ControlPoint>& GetPoints() { return points; }
    const std::deque<ControlPoint>& GetPoints() const { return points; }

private:
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
};
