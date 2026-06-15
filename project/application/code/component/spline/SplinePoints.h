#pragma once
#include "component/IComponent.h"

/// stl
#include <deque>

/// math
#include "math/MyEasing.h"
#include "math/Vector3.h"

/// <summary>
/// Spline共通設定
/// </summary>
struct SplineCommonSettings {
    bool isCrossMesh    = true;
    bool isUvLoopEnable = false;

    float uvLoopLength = 1.0f;

    OriGine::EaseType widthEaseType = OriGine::EaseType::EaseInOutSine;
    OriGine::EaseType uvEaseType    = OriGine::EaseType::EaseInOutSine;

    OriGine::EntityHandle playerEntityHandle = OriGine::EntityHandle();
    int32_t segmentDivide                    = 16;

    OriGine::Vec2f startUv = {0.0f, 0.0f};
    OriGine::Vec2f endUv   = {1.0f, 1.0f};

    OriGine::Vec3f upVector = {0.0f, 1.0f, 0.0f};

    float segmentLength = 0.5f;
    float startWidth    = 0.2f;
    float endWidth      = 0.2f;

    float fadeoutTime  = 0.5f;
    float fadeoutTimer = 0.f;
};

/// <summary>
/// Splineの制御点を管理するコンポーネント
/// </summary>
class SplinePoints
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const SplinePoints& c);
    friend void from_json(const nlohmann::json& j, SplinePoints& c);

public:
    SplinePoints();
    ~SplinePoints() override;

    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;
    void Finalize() override;

    /// <summary>
    /// 制御点を追加する(capacityを超過した場合,popfrontされる)
    /// </summary>
    /// <param name="_point"></param>
    void PushPoint(const OriGine::Vec3f& _pos);

public:
    SplineCommonSettings& GetCommonSettings() { return commonSettings; }
    const SplineCommonSettings& GetCommonSettings() const { return commonSettings; }

    int32_t GetCapacity() const { return capacity; }
    void SetCapacity(int32_t _capacity) { capacity = _capacity; }

    int32_t GetSegmentDivide() const { return segmentDivide; }
    void SetSegmentDivide(int32_t _segmentDivide) { segmentDivide = _segmentDivide; }

    std::deque<OriGine::Vec3f>& GetPoints() { return points; }
    const std::deque<OriGine::Vec3f>& GetPoints() const { return points; }

private:
    SplineCommonSettings commonSettings;

    int32_t capacity      = 100;
    int32_t segmentDivide = 16;

    std::deque<OriGine::Vec3f> points;
};
