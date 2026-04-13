#pragma once

#include "component/IComponent.h"

/// stl
#include <deque>
#include <string>

/// math
#include "math/Vector3.h"
#include <cstdint>

/// <summary>
/// Railの制御点を管理するコンポーネント
/// </summary>
struct RailPoints
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const RailPoints& _c);
    friend void from_json(const nlohmann::json& _j, RailPoints& _c);

public:
    RailPoints();
    ~RailPoints() override;
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

public:
    std::deque<OriGine::Vec3f> points; // レールの制御点リスト
    float radius                = 0.5f;
    float collisionRadiusOffset = 0.0f; // 衝突判定用の半径オフセット (正の値で半径を大きく、負の値で小さくする)
    int32_t segmentDivide       = 8; // Catmull-Romスプラインのセグメントあたりの分割数
    std::string texturePath     = "";
    size_t textureIndex         = 0;
    float totalLength           = 0.0f;
};
