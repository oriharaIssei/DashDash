#include "CreateMeshFromSpline.h"

#include "EffectConfig.h"

/// math
#include "math/MathEnv.h"
#include <math/MyEasing.h>
#include <math/Spline.h>
#include <math/Vector3.h>

using namespace OriGine;

namespace {
/// <summary>
/// スプラインセグメント情報
/// </summary>
struct SplineSegment {
    OriGine::Vec3f p0;
    OriGine::Vec3f p1;
    OriGine::Vec3f dir;
    OriGine::Vec3f right;
    OriGine::Vec3f up;

    float prevLengthRatio;
    float lengthRatio;

    float minWidthHalf;
    float maxWidthHalf;

    OriGine::Vec2f uv[4];
};

// スプラインの1区間(p0->p1)について、進行方向・法線・幅・UVを計算しSplineSegmentとして返す
static SplineSegment BuildSplineSegment(
    const OriGine::Vec3f& p0,
    const OriGine::Vec3f& p1,
    float prevLength,
    float totalLength,
    float allLength,
    const SplineCommonSettings& settings) {
    SplineSegment seg;

    seg.p0 = p0;
    seg.p1 = p1;

    Vec3f rawDir = p1 - p0;

    // 進行方向(dir)を軸とした正規直交基底を作る。設定のupVectorはあくまで「おおよその上方向」でしかなく
    // dirと直交しているとは限らないため、up→rightを求めた後にright→upを取り直して直交化している
    // (この2段階のcrossにより、upVectorがどんな向きでもdir/right/upが必ず直交する)
    seg.dir   = rawDir.normalize(); // 進行方向
    seg.up    = settings.upVector;
    seg.right = seg.up.cross(seg.dir).normalize();
    seg.up    = seg.dir.cross(seg.right).normalize();

    // UV・幅の補間に使う進行度(0〜1)を求める。
    // ループ有効時はuvLoopLengthごとにUVを繰り返させたいので「一定距離」で割り、
    // 無効時はスプライン全体で0〜1に収めたいので「全長」で割る
    float prevRatio, ratio;
    if (settings.isUvLoopEnable) {
        prevRatio = prevLength / settings.uvLoopLength;
        ratio     = totalLength / settings.uvLoopLength;
    } else {
        prevRatio = prevLength / allLength;
        ratio     = totalLength / allLength;
    }

    seg.prevLengthRatio = prevRatio;
    seg.lengthRatio     = ratio;

    const int uvEase   = static_cast<int>(settings.uvEaseType);
    const float uvPrev = EasingFunctions[uvEase](prevRatio);
    const float uvNow  = EasingFunctions[uvEase](ratio);

    // 区間を四角形として貼るため、四隅のUVを求める。
    // U(横)は帯の左右端なのでstartUv[X]/endUv[X]で固定し、V(縦)だけを進行度で補間する。
    // uv[0]=始端左, uv[1]=始端右, uv[2]=終端左, uv[3]=終端右 の並びで、[1]と[2]は[0]/[3]の成分の組み合わせで求まる
    seg.uv[0] = {settings.startUv[X], std::lerp(settings.startUv[Y], settings.endUv[Y], uvPrev)};
    seg.uv[3] = {settings.endUv[X], std::lerp(settings.startUv[Y], settings.endUv[Y], uvNow)};
    seg.uv[1] = {seg.uv[3][X], seg.uv[0][Y]};
    seg.uv[2] = {seg.uv[0][X], seg.uv[3][Y]};

    int widthEase    = static_cast<int>(settings.widthEaseType);
    seg.minWidthHalf = std::lerp(settings.startWidth, settings.endWidth, EasingFunctions[widthEase](prevRatio)) * EffectConfig::Spline::kHalfMultiplier;
    seg.maxWidthHalf = std::lerp(settings.startWidth, settings.endWidth, EasingFunctions[widthEase](ratio)) * EffectConfig::Spline::kHalfMultiplier;

    return seg;
}

// SplineSegment1区間分の頂点・インデックスをプレーンメッシュとして追加する
// isFirstがtrueのときのみ始端側の頂点も追加し、2区間目以降は直前に追加した終端頂点を始端として再利用する
static void AppendPlaneSegment(
    std::vector<TextureColorVertexData>& vertices,
    std::vector<uint32_t>& indices,
    const SplineSegment& seg,
    const OriGine::Vec3f& axis,
    const OriGine::Vec3f& normal,
    bool isFirst) {
    // 始点/終点それぞれで、幅方向(axis)に半分の幅だけ左右へ振って帯の四隅を求める
    OriGine::Vec3f p0L = seg.p0 - axis * seg.minWidthHalf;
    OriGine::Vec3f p0R = seg.p0 + axis * seg.minWidthHalf;
    OriGine::Vec3f p1L = seg.p1 - axis * seg.maxWidthHalf;
    OriGine::Vec3f p1R = seg.p1 + axis * seg.maxWidthHalf;

    if (isFirst) {
        vertices.push_back({OriGine::Vec4f(p0L, 1), seg.uv[0], normal, kWhite});
        vertices.push_back({OriGine::Vec4f(p0R, 1), seg.uv[1], normal, kWhite});
    }

    vertices.push_back({OriGine::Vec4f(p1L, 1), seg.uv[2], normal, kWhite});
    vertices.push_back({OriGine::Vec4f(p1R, 1), seg.uv[3], normal, kWhite});

    // 直前に積んだ4頂点(始端左右・終端左右)を先頭として、四角形を2枚の三角形に分割する。
    // 2区間目以降は始端頂点を追加していないが、前区間の終端頂点がそのまま4頂点前から並ぶため同じ計算で参照できる
    uint32_t base = static_cast<uint32_t>(vertices.size() - 4);
    indices.insert(indices.end(), {base + 0, base + 2, base + 1,
                                      base + 1, base + 2, base + 3});
}
}

CreateMeshFromSpline::CreateMeshFromSpline() : ISystem(OriGine::SystemCategory::Effect) {}
CreateMeshFromSpline::~CreateMeshFromSpline() {}

void CreateMeshFromSpline::Initialize() {}
void CreateMeshFromSpline::Finalize() {}

void CreateMeshFromSpline::UpdateEntity(const OriGine::EntityHandle& _handle) {
    auto planeRendererComp = GetComponent<PlaneRenderer>(_handle);
    if (planeRendererComp == nullptr) {
        return;
    }

    auto& splinePointsComps = GetComponents<SplinePoints>(_handle);
    if (splinePointsComps.empty()) {
        return;
    }

    planeRendererComp->SetIsCulling(true);
    planeRendererComp->SetInstancing(false); // インスタンシングは使用しない
    for (auto& splinePointsComp : splinePointsComps) {
        if (splinePointsComp.GetCommonSettings().isCrossMesh) {
            CreateCrossPlaneMesh(planeRendererComp, &splinePointsComp);
        } else {
            CreateLinePlaneMesh(planeRendererComp, &splinePointsComp);
        }
    }
}

void CreateMeshFromSpline::CreateLinePlaneMesh(
    PlaneRenderer* renderer,
    SplinePoints* spline) {
    std::vector<TextureColorVertexData> vertices;
    std::vector<uint32_t> indices;

    const int32_t segmentCount = static_cast<int32_t>(spline->GetPoints().size() - 1);
    float allLength            = 0.f;

    // UV/幅の補間比率を求めるため、スプライン全長を事前に計算しておく
    for (int32_t i = 0; i < segmentCount; ++i) {
        allLength += Vec3f(spline->GetPoints()[i + 1] - spline->GetPoints()[i]).length();
    }

    float totalLength = 0.f;
    float prevTotal   = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const auto& p0 = spline->GetPoints()[i];
        const auto& p1 = spline->GetPoints()[i + 1];

        // 区間の始点/終点までの累積距離を更新
        prevTotal = totalLength;
        totalLength += Vec3f(p1 - p0).length();

        const auto seg = BuildSplineSegment(
            p0, p1, prevTotal, totalLength, allLength, spline->GetCommonSettings());

        AppendPlaneSegment(
            vertices, indices,
            seg,
            seg.right, // 横方向に幅
            seg.up, // 法線
            i == 0);
    }

    TextureColorMesh mesh;
    mesh.Initialize((UINT)vertices.size(), (UINT)indices.size());
    mesh.SetVertexData(vertices);
    mesh.SetIndexData(indices);

    renderer->SetMeshGroup({mesh});
    renderer->GetMeshGroup()->at(0).TransferData();
}

void CreateMeshFromSpline::CreateCrossPlaneMesh(
    PlaneRenderer* renderer,
    SplinePoints* spline) {
    std::vector<TextureColorVertexData> vertical;
    std::vector<TextureColorVertexData> horizontal;
    std::vector<uint32_t> indices;

    const int32_t segmentCount = static_cast<int32_t>(spline->GetPoints().size() - 1);

    float allLength = 0.f;
    for (int32_t i = 0; i < segmentCount; ++i) {
        allLength += Vec3f(spline->GetPoints()[i + 1] - spline->GetPoints()[i]).length();
    }

    float totalLength = 0.f;
    float prevTotal   = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const auto& p0 = spline->GetPoints()[i];
        const auto& p1 = spline->GetPoints()[i + 1];

        prevTotal = totalLength;
        totalLength += Vec3f(p1 - p0).length();

        const auto seg = BuildSplineSegment(
            p0, p1, prevTotal, totalLength, allLength, spline->GetCommonSettings());

        // 縦面(right軸を幅方向・up軸を法線)と横面(up軸を幅方向・right軸を法線)を別々の頂点列として積み上げ、十字に交差させる
        AppendPlaneSegment(vertical, indices, seg, seg.right, seg.up, i == 0);
        AppendPlaneSegment(horizontal, indices, seg, seg.up, seg.right, i == 0);
    }

    TextureColorMesh vMesh, hMesh;
    vMesh.Initialize(static_cast<UINT>(vertical.size()), static_cast<UINT>(indices.size()));
    vMesh.SetVertexData(vertical);
    vMesh.SetIndexData(indices);

    hMesh.Initialize(static_cast<UINT>(horizontal.size()), static_cast<UINT>(indices.size()));
    hMesh.SetVertexData(horizontal);
    hMesh.SetIndexData(indices);

    renderer->SetMeshGroup({vMesh, hMesh});
    renderer->GetMeshGroup()->at(0).TransferData();
    renderer->GetMeshGroup()->at(1).TransferData();
}
