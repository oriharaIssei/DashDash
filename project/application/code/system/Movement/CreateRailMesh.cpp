#include "CreateRailMesh.h"

/// ECS
// component
#include "component/renderer/primitive/SphereRenderer.h"

/// math
#include "math/MathEnv.h"
#include "math/Spline.h"
#include "math/Vector3.h"

using namespace OriGine;

namespace {
/// <summary>
/// セグメント間のスムーズな方向ベクトルを計算する
/// </summary>
/// <param name="_points">制御点配列</param>
/// <param name="_index">対象インデックス</param>
/// <returns>方向ベクトル</returns>
static Vec3f CalculateSegmentDirection(const std::deque<Vec3f>& _points, size_t _index) {
    const size_t count = _points.size();
    if (count < 2) {
        return Vec3f(0.f, 0.f, 1.f);
    }

    Vec3f dir(0.f, 0.f, 0.f);

    // 前方向への差分
    if (_index < count - 1) {
        Vec3f diff = _points[_index + 1] - _points[_index];
        dir        = dir + diff.normalize();
    }
    // 後方向への差分
    if (_index > 0) {
        Vec3f diff = _points[_index] - _points[_index - 1];
        dir        = dir + diff.normalize();
    }

    float len = dir.length();
    if (len < kEpsilon) {
        return Vec3f(0.f, 0.f, 1.f);
    }
    return dir / len;
}

/// <summary>
/// 任意の方向ベクトルから直交座標系を構築する
/// </summary>
/// <param name="_forward">前方向ベクトル</param>
/// <param name="_outRight">出力: 右方向ベクトル</param>
/// <param name="_outUp">出力: 上方向ベクトル</param>
static void BuildOrthonormalBasis(const Vec3f& _forward, Vec3f& _outRight, Vec3f& _outUp) {
    // 上方向ヒントとしてY軸を使う
    Vec3f up(0.f, 1.f, 0.f);

    // forwardがほぼY軸と平行な場合はZ軸をヒントとして使用
    if (std::abs(_forward[Y]) > 1.f - kEpsilon) {
        up = Vec3f(0.f, 0.f, 1.f);
    }

    _outRight = up.cross(_forward).normalize();
    _outUp    = _forward.cross(_outRight).normalize();
}

} // namespace

CreateRailMesh::CreateRailMesh() : ISystem(SystemCategory::Effect) {}
CreateRailMesh::~CreateRailMesh() {}

void CreateRailMesh::Initialize() {}
void CreateRailMesh::Finalize() {}

void CreateRailMesh::UpdateEntity(EntityHandle _handle) {
    auto cylinderRenderer = GetComponent<CylinderRenderer>(_handle);
    if (cylinderRenderer == nullptr) {
        return;
    }

    auto railPointsComp = GetComponent<RailPoints>(_handle);
    if (!railPointsComp || (railPointsComp->points.size() < 2)) {
        return;
    }

    CreateCylinderMeshFromRail(cylinderRenderer, railPointsComp);

    const Vec3f& firstPoint = railPointsComp->points.front();
    const Vec3f& lastPoint  = railPointsComp->points.back();

    auto firstPointRenderer = GetComponent<SphereRenderer>(_handle, 0);
    if (!firstPointRenderer) {
        return;
    }
    firstPointRenderer->GetTransform().translate = firstPoint;

    auto lastPointRenderer = GetComponent<SphereRenderer>(_handle, 1);
    if (!lastPointRenderer) {
        return;
    }
    lastPointRenderer->GetTransform().translate = lastPoint;
}

void CreateRailMesh::CreateCylinderMeshFromRail(
    CylinderRenderer* _renderer,
    const RailPoints* _railPoints) {

    const auto& originalPoints  = _railPoints->points;
    const float radius          = _railPoints->radius;
    const int32_t segmentDivide = _railPoints->segmentDivide;

    // CylinderRendererのPrimitiveから分割数を取得
    const auto& cylinderPrimitive   = _renderer->GetPrimitive();
    const uint32_t _radialDivisions = cylinderPrimitive.radialDivisions;

    // 最低2点が必要
    if (originalPoints.size() < 2) {
        return;
    }

    // 4点以上ならCatmull-Romスプラインで補間、それ未満なら直線接続
    std::deque<Vec3f> points;
    if (originalPoints.size() >= 4) {
        // 端点を複製して追加することで、最初と最後の制御点も補間範囲に含める
        // Catmull-Romは4点(p0,p1,p2,p3)でp1-p2間を補間するため
        std::deque<Vec3f> extendedPoints = originalPoints;
        extendedPoints.push_front(originalPoints.front()); // 最初の点を複製
        extendedPoints.push_back(originalPoints.back()); // 最後の点を複製
        points = CatmullRomSpline(extendedPoints, segmentDivide);
    } else {
        points = originalPoints;
    }

    const size_t pointCount   = points.size();
    const size_t segmentCount = pointCount - 1;

    // 円周上の角度ステップ
    const float angleStep = kTau / static_cast<float>(_radialDivisions);

    // 頂点数: 各制御点ごとに円周上に(_radialDivisions + 1)個
    // インデックス数: セグメント数 × 円周分割数 × 6 (2つの三角形)
    const uint32_t vertexCount = static_cast<uint32_t>(pointCount * (_radialDivisions + 1));
    const uint32_t indexCount  = static_cast<uint32_t>(segmentCount * _radialDivisions * 6);

    std::vector<TextureColorVertexData> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(vertexCount);
    indices.reserve(indexCount);

    // 累積長さを計算してUV座標に使用
    float totalLength = 0.f;
    for (size_t i = 0; i < segmentCount; ++i) {
        Vec3f diff = points[i + 1] - points[i];
        totalLength += diff.length();
    }

    float accumulatedLength = 0.f;

    // ==============================
    // 頂点生成
    // ==============================
    for (size_t p = 0; p < pointCount; ++p) {
        // V座標の計算
        float v = 0.f;
        if (p > 0) {
            Vec3f diff = points[p] - points[p - 1];
            accumulatedLength += diff.length();
        }
        if (totalLength > 0.001f) {
            v = accumulatedLength / totalLength;
        }

        // このポイントでの方向ベクトルと直交座標系を計算
        Vec3f forward = CalculateSegmentDirection(points, p);
        Vec3f right, up;
        BuildOrthonormalBasis(forward, right, up);

        // 円周上の頂点を生成
        for (uint32_t r = 0; r <= _radialDivisions; ++r) {
            float angle = angleStep * static_cast<float>(r);
            float cosA  = std::cos(angle);
            float sinA  = std::sin(angle);

            // ローカル座標での位置オフセット
            Vec3f localOffset = right * (cosA * radius) + up * (sinA * radius);
            Vec3f pos         = points[p] + localOffset;

            // 法線 (外側を向く)
            Vec3f normal = localOffset.normalize();

            // UV座標
            float u = static_cast<float>(r) / static_cast<float>(_radialDivisions);

            TextureColorVertexData vertex{};
            vertex.pos      = Vec4f(pos, 1.f);
            vertex.texCoord = Vec2f(u, v);
            vertex.normal   = normal;
            vertex.color    = kWhite;

            vertices.emplace_back(vertex);
        }
    }

    // ==============================
    // インデックス生成
    // ==============================
    const uint32_t ringVertexCount = _radialDivisions + 1;

    for (size_t s = 0; s < segmentCount; ++s) {
        uint32_t baseIndex = static_cast<uint32_t>(s * ringVertexCount);

        for (uint32_t r = 0; r < _radialDivisions; ++r) {
            uint32_t i0 = baseIndex + r;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = i0 + ringVertexCount;
            uint32_t i3 = i2 + 1;

            // 三角形1
            indices.push_back(i0);
            indices.push_back(i2);
            indices.push_back(i1);

            // 三角形2
            indices.push_back(i1);
            indices.push_back(i2);
            indices.push_back(i3);
        }
    }

    // ==============================
    // メッシュの設定
    // ==============================
    TextureColorMesh mesh;
    mesh.Initialize(static_cast<UINT>(vertices.size()), static_cast<UINT>(indices.size()));
    mesh.SetVertexData(vertices);
    mesh.SetIndexData(indices);

    _renderer->SetMeshGroup({mesh});
    _renderer->GetMeshGroup()->at(0).TransferData();
}
