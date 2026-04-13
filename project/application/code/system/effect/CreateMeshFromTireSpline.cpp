#include "CreateMeshFromTireSpline.h"

#include "EffectConfig.h"

/// math
#include <Vector2.h>
#include <Vector3.h>

using namespace OriGine;

/// <summary>
/// スプラインセグメント情報
/// </summary>
struct TireSplineSegment {
    OriGine::Vec3f p0;
    OriGine::Vec3f p1;

    OriGine::Vec3f dir;
    OriGine::Vec3f up;
    OriGine::Vec3f right;

    float prevLengthRatio;
    float lengthRatio;

    float minWidthHalf;
    float maxWidthHalf;

    OriGine::Vec2f uv[4];

    float alpha0; // p0 側
    float alpha1; // p1 側
};
static TireSplineSegment BuildTireSplineSegment(
    const TireSplinePoints::ControlPoint& p0,
    const TireSplinePoints::ControlPoint& p1,
    float prevLength,
    float totalLength,
    float allLength,
    const SplineCommonSettings& commonSettings) {
    TireSplineSegment seg{};

    seg.p0 = p0.position;
    seg.p1 = p1.position;

    seg.alpha0 = p0.alpha;
    seg.alpha1 = p1.alpha;

    seg.dir   = Vec3f(seg.p1 - seg.p0).normalize();
    seg.up    = axisY;
    seg.right = seg.dir.cross(seg.up).normalize();

    float prevRatio, ratio;
    if (commonSettings.isUvLoopEnable) {
        prevRatio = prevLength / commonSettings.uvLoopLength;
        ratio     = totalLength / commonSettings.uvLoopLength;
    } else {
        prevRatio = prevLength / allLength;
        ratio     = totalLength / allLength;
    }

    seg.prevLengthRatio = prevRatio;
    seg.lengthRatio     = ratio;

    const int uvEase   = static_cast<int>(commonSettings.uvEaseType);
    const float uvPrev = EasingFunctions[uvEase](prevRatio);
    const float uvNow  = EasingFunctions[uvEase](ratio);

    seg.uv[0] = {commonSettings.startUv[X], std::lerp(commonSettings.startUv[Y], commonSettings.endUv[Y], uvPrev)};
    seg.uv[3] = {commonSettings.endUv[X], std::lerp(commonSettings.startUv[Y], commonSettings.endUv[Y], uvNow)};
    seg.uv[1] = {seg.uv[3][X], seg.uv[0][Y]};
    seg.uv[2] = {seg.uv[0][X], seg.uv[3][Y]};

    const int widthEase = static_cast<int>(commonSettings.widthEaseType);
    seg.minWidthHalf =
        std::lerp(commonSettings.startWidth, commonSettings.endWidth,
            EasingFunctions[widthEase](prevRatio))
        * EffectConfig::Spline::kHalfMultiplier;

    seg.maxWidthHalf =
        std::lerp(commonSettings.startWidth, commonSettings.endWidth,
            EasingFunctions[widthEase](ratio))
        * EffectConfig::Spline::kHalfMultiplier;

    return seg;
}

static void AppendPlaneSegment(
    std::vector<TextureColorVertexData>& vertices,
    std::vector<uint32_t>& indices,
    const TireSplineSegment& seg,
    const OriGine::Vec3f& axis,
    const OriGine::Vec3f& normal,
    bool isFirst) {
    const Vec3f p0L = seg.p0 - axis * seg.minWidthHalf;
    const Vec3f p0R = seg.p0 + axis * seg.minWidthHalf;
    const Vec3f p1L = seg.p1 - axis * seg.maxWidthHalf;
    const Vec3f p1R = seg.p1 + axis * seg.maxWidthHalf;

    const Vec4f color0(1.f, 1.f, 1.f, seg.alpha0);
    const Vec4f color1(1.f, 1.f, 1.f, seg.alpha1);

    if (isFirst) {
        vertices.push_back({Vec4f(p0L, 1.f), seg.uv[0], normal, color0});
        vertices.push_back({Vec4f(p0R, 1.f), seg.uv[1], normal, color0});
    }

    vertices.push_back({Vec4f(p1L, 1.f), seg.uv[2], normal, color1});
    vertices.push_back({Vec4f(p1R, 1.f), seg.uv[3], normal, color1});

    const uint32_t base = static_cast<uint32_t>(vertices.size() - 4);
    indices.insert(indices.end(), {base + 0, base + 1, base + 2,
                                      base + 1, base + 3, base + 2});
}

CreateMeshFromTireSpline::CreateMeshFromTireSpline() : ISystem(SystemCategory::Effect) {}
CreateMeshFromTireSpline::~CreateMeshFromTireSpline() {}

void CreateMeshFromTireSpline::Initialize() {}
void CreateMeshFromTireSpline::Finalize() {}

void CreateMeshFromTireSpline::UpdateEntity(OriGine::EntityHandle _handle) {
    auto splineComp        = GetComponent<TireSplinePoints>(_handle);
    auto planeRendererComp = GetComponent<PlaneRenderer>(_handle);
    if (!splineComp || !planeRendererComp) {
        return;
    }

    if (splineComp->points.size() < EffectConfig::TireSpline::kMinPoints) {
        planeRendererComp->SetIsRender(false);
        return;
    }

    planeRendererComp->SetIsRender(true);
    planeRendererComp->SetInstancing(false); // インスタンシングは使用しない
    if (splineComp->commonSettings.isCrossMesh) {
        CreateCrossPlaneMesh(planeRendererComp, splineComp);
    } else {
        CreateLinePlaneMesh(planeRendererComp, splineComp);
    }
}

void CreateMeshFromTireSpline::CreateLinePlaneMesh(
    PlaneRenderer* renderer,
    TireSplinePoints* spline) {
    std::vector<TextureColorVertexData> vertices;
    std::vector<uint32_t> indices;

    const int32_t segmentCount =
        static_cast<int32_t>(spline->points.size() - 1);

    const float allLength =
        spline->commonSettings.segmentLength * spline->capacity;

    float totalLength = 0.f;
    float prevTotal   = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const auto& p0 = spline->points[i];
        const auto& p1 = spline->points[i + 1];

        prevTotal = totalLength;
        totalLength += Vec3f(p1.position - p0.position).length();

        const auto seg = BuildTireSplineSegment(
            p0, p1,
            prevTotal, totalLength,
            allLength,
            spline->commonSettings);

        AppendPlaneSegment(
            vertices, indices,
            seg,
            seg.right,
            seg.up,
            i == 0);
    }

    TextureColorMesh mesh;
    mesh.Initialize((UINT)vertices.size(), (UINT)indices.size());
    mesh.SetVertexData(vertices);
    mesh.SetIndexData(indices);

    renderer->SetMeshGroup({mesh});
    renderer->GetMeshGroup()->at(0).TransferData();
}

void CreateMeshFromTireSpline::CreateCrossPlaneMesh(
    PlaneRenderer* renderer,
    TireSplinePoints* spline) {
    std::vector<TextureColorVertexData> vertical;
    std::vector<TextureColorVertexData> horizontal;
    std::vector<uint32_t> indices;

    const int32_t segmentCount = static_cast<int32_t>(spline->points.size() - 1);

    float allLength = 0.f;
    for (int32_t i = 0; i < segmentCount; ++i) {
        allLength += Vec3f(spline->points[i + 1].position - spline->points[i].position).length();
    }

    float totalLength = 0.f;
    float prevTotal   = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const auto& p0 = spline->points[i];
        const auto& p1 = spline->points[i + 1];

        prevTotal = totalLength;
        totalLength += Vec3f(p1.position - p0.position).length();

        const auto seg = BuildTireSplineSegment(
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
