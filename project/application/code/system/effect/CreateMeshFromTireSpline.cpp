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
// タイヤスプラインの1区間(p0->p1)について、進行方向・法線・幅・UV・アルファを計算しTireSplineSegmentとして返す
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

    // 帯を張るための直交基底を作る。dir(進行方向)とup(ワールド上方向)の外積で
    // 進行方向に対して真横を向くrightが得られ、これが帯の幅方向になる。
    // upをワールド固定にしているのは、タイヤ痕は常に地面に貼り付いていてほしいため
    seg.dir   = Vec3f(seg.p1 - seg.p0).normalize();
    seg.up    = axisY;
    seg.right = seg.dir.cross(seg.up).normalize();

    // UVのV座標をどう割り当てるかの分岐。
    // ループ有効時は「固定長ごとに1周」させるため一定長で割る。比率が1を超えてもよく、
    // テクスチャのラップによって痕の長さに関わらず模様の密度が一定に保たれる。
    // 無効時は全長で割って0〜1に正規化するので、テクスチャが痕全体に1回だけ引き伸ばされる
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

    // U(横)は帯の左右端で固定、V(縦)だけを進行度で補間する。
    // まず対角のuv[0](始端左)とuv[3](終端右)を求め、残る2隅はその成分の組み合わせで作る。
    // uv[1]=始端右, uv[2]=終端左 となり、4隅が矩形を成す
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

// TireSplineSegment1区間分の頂点・インデックスをプレーンメッシュとして追加する
// isFirstがtrueのときのみ始端側の頂点も追加し、2区間目以降は直前に追加した終端頂点を始端として再利用する
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

    // 今追加した終端2頂点と、その直前にある始端2頂点(初回はこの関数で追加、2回目以降は
    // 前区間の終端頂点)の計4つで1枚の四角形を張る。末尾から4つ遡ればその先頭になる。
    // 頂点を共有することで区間の継ぎ目に隙間が出ず、頂点数も半分で済む
    const uint32_t base = static_cast<uint32_t>(vertices.size() - 4);
    indices.insert(indices.end(), {base + 0, base + 1, base + 2,
                                      base + 1, base + 3, base + 2});
}

CreateMeshFromTireSpline::CreateMeshFromTireSpline() : ISystem(SystemCategory::Effect) {}
CreateMeshFromTireSpline::~CreateMeshFromTireSpline() {}

void CreateMeshFromTireSpline::Initialize() {}
void CreateMeshFromTireSpline::Finalize() {}

void CreateMeshFromTireSpline::UpdateEntity(const OriGine::EntityHandle& _handle) {
    auto splineComp        = GetComponent<TireSplinePoints>(_handle);
    auto planeRendererComp = GetComponent<PlaneRenderer>(_handle);
    if (!splineComp || !planeRendererComp) {
        return;
    }

    // メッシュを構成するのに必要な制御点数が無い場合は非表示にする
    if (splineComp->GetPoints().size() < EffectConfig::TireSpline::kMinPoints) {
        planeRendererComp->SetIsRender(false);
        return;
    }

    planeRendererComp->SetIsRender(true);
    planeRendererComp->SetInstancing(false); // インスタンシングは使用しない
    if (splineComp->GetCommonSettings().isCrossMesh) {
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
        static_cast<int32_t>(spline->GetPoints().size() - 1);

    // タイヤの軌跡は一定間隔(segmentLength)で点が積まれる想定のため、実測ではなく容量から全長を概算する
    const float allLength =
        spline->GetCommonSettings().segmentLength * spline->GetCapacity();

    float totalLength = 0.f;
    float prevTotal   = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const auto& p0 = spline->GetPoints()[i];
        const auto& p1 = spline->GetPoints()[i + 1];

        prevTotal = totalLength;
        totalLength += Vec3f(p1.position - p0.position).length();

        const auto seg = BuildTireSplineSegment(
            p0, p1,
            prevTotal, totalLength,
            allLength,
            spline->GetCommonSettings());

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

    const int32_t segmentCount = static_cast<int32_t>(spline->GetPoints().size() - 1);

    float allLength = 0.f;
    for (int32_t i = 0; i < segmentCount; ++i) {
        allLength += Vec3f(spline->GetPoints()[i + 1].position - spline->GetPoints()[i].position).length();
    }

    float totalLength = 0.f;
    float prevTotal   = 0.f;

    for (int32_t i = 0; i < segmentCount; ++i) {
        const auto& p0 = spline->GetPoints()[i];
        const auto& p1 = spline->GetPoints()[i + 1];

        prevTotal = totalLength;
        totalLength += Vec3f(p1.position - p0.position).length();

        const auto seg = BuildTireSplineSegment(
            p0, p1, prevTotal, totalLength, allLength, spline->GetCommonSettings());

        // 縦面(right軸を幅方向・up軸を法線)と横面(up軸を幅方向・right軸を法線)を別々の頂点列として積み上げ、十字に交差させる
        // 幅方向と法線を入れ替えて2回呼ぶことで、同じ区間に直交する2枚の板が張られる。
        // 十字にするのは、1枚だけだと真横から見たときに厚みが無く痕が消えてしまうため
        // NOTE: verticalとhorizontalは常に同じ数だけ頂点が増えるため、両者のbaseは一致する。
        //       その結果この共有indicesには同一の三角形が2回ずつ積まれ、
        //       各メッシュが同じ面を二重に描画している(見た目は変わらないが無駄がある)
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
