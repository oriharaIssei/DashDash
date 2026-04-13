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

    seg.dir   = rawDir.normalize(); // 進行方向
    seg.up    = settings.upVector;
    seg.right = seg.up.cross(seg.dir).normalize();
    seg.up    = seg.dir.cross(seg.right).normalize();

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

    seg.uv[0] = {settings.startUv[X], std::lerp(settings.startUv[Y], settings.endUv[Y], uvPrev)};
    seg.uv[3] = {settings.endUv[X], std::lerp(settings.startUv[Y], settings.endUv[Y], uvNow)};
    seg.uv[1] = {seg.uv[3][X], seg.uv[0][Y]};
    seg.uv[2] = {seg.uv[0][X], seg.uv[3][Y]};

    int widthEase    = static_cast<int>(settings.widthEaseType);
    seg.minWidthHalf = std::lerp(settings.startWidth, settings.endWidth, EasingFunctions[widthEase](prevRatio)) * EffectConfig::Spline::kHalfMultiplier;
    seg.maxWidthHalf = std::lerp(settings.startWidth, settings.endWidth, EasingFunctions[widthEase](ratio)) * EffectConfig::Spline::kHalfMultiplier;

    return seg;
}

static void AppendPlaneSegment(
    std::vector<TextureColorVertexData>& vertices,
    std::vector<uint32_t>& indices,
    const SplineSegment& seg,
    const OriGine::Vec3f& axis,
    const OriGine::Vec3f& normal,
    bool isFirst) {
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

    uint32_t base = static_cast<uint32_t>(vertices.size() - 4);
    indices.insert(indices.end(), {base + 0, base + 2, base + 1,
                                      base + 1, base + 2, base + 3});
}
}

CreateMeshFromSpline::CreateMeshFromSpline() : ISystem(OriGine::SystemCategory::Effect) {}
CreateMeshFromSpline::~CreateMeshFromSpline() {}

void CreateMeshFromSpline::Initialize() {}
void CreateMeshFromSpline::Finalize() {}

void CreateMeshFromSpline::UpdateEntity(OriGine::EntityHandle _handle) {
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
        if (splinePointsComp.commonSettings.isCrossMesh) {
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

    const int32_t segmentCount = static_cast<int32_t>(spline->points.size() - 1);
    float allLength            = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        allLength += Vec3f(spline->points[i + 1] - spline->points[i]).length();
    }

    float totalLength = 0.f;
    float prevTotal   = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const auto& p0 = spline->points[i];
        const auto& p1 = spline->points[i + 1];

        prevTotal = totalLength;
        totalLength += Vec3f(p1 - p0).length();

        const auto seg = BuildSplineSegment(
            p0, p1, prevTotal, totalLength, allLength, spline->commonSettings);

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

    const int32_t segmentCount = static_cast<int32_t>(spline->points.size() - 1);

    float allLength = 0.f;
    for (int32_t i = 0; i < segmentCount; ++i) {
        allLength += Vec3f(spline->points[i + 1] - spline->points[i]).length();
    }

    float totalLength = 0.f;
    float prevTotal   = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const auto& p0 = spline->points[i];
        const auto& p1 = spline->points[i + 1];

        prevTotal = totalLength;
        totalLength += Vec3f(p1 - p0).length();

        const auto seg = BuildSplineSegment(
            p0, p1, prevTotal, totalLength, allLength, spline->commonSettings);

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
