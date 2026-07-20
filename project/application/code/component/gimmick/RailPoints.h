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
class RailPoints
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const RailPoints& _c);
    friend void from_json(const nlohmann::json& _j, RailPoints& _c);

public:
    RailPoints();
    ~RailPoints() override;
    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;
    void Finalize() override;

public:
    std::deque<OriGine::Vec3f>& GetPoints() { return points; }
    const std::deque<OriGine::Vec3f>& GetPoints() const { return points; }
    void SetPoints(const std::deque<OriGine::Vec3f>& _points) { points = _points; }

    float GetRadius() const { return radius; }
    void SetRadius(float _radius) { radius = _radius; }

    float GetCollisionRadiusOffset() const { return collisionRadiusOffset; }
    void SetCollisionRadiusOffset(float _collisionRadiusOffset) { collisionRadiusOffset = _collisionRadiusOffset; }

    int32_t GetSegmentDivide() const { return segmentDivide; }
    void SetSegmentDivide(int32_t _segmentDivide) { segmentDivide = _segmentDivide; }

    const std::string& GetTexturePath() const { return texturePath; }
    void SetTexturePath(const std::string& _texturePath) { texturePath = _texturePath; }

    size_t GetTextureIndex() const { return textureIndex; }
    void SetTextureIndex(size_t _textureIndex) { textureIndex = _textureIndex; }

    float GetTotalLength() const { return totalLength; }
    void SetTotalLength(float _totalLength) { totalLength = _totalLength; }

private:
    std::deque<OriGine::Vec3f> points; // レールの制御点リスト
    float radius                = 0.5f; // レールの表示・当たり判定に使う半径
    float collisionRadiusOffset = 0.0f; // 衝突判定用の半径オフセット (正の値で半径を大きく、負の値で小さくする)
    int32_t segmentDivide       = 8; // Catmull-Romスプラインのセグメントあたりの分割数
    std::string texturePath     = ""; // レール描画に使用するテクスチャのパス
    size_t textureIndex         = 0; // テクスチャリソースのインデックス
    float totalLength           = 0.0f; // 制御点から算出したレール全体の長さ
};
