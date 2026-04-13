#pragma once

#include "system/ISystem.h"

/// ECS
// component
#include "component/spline/TireSplinePoints.h"

/// math
#include "math/Vector3.h"

/// <summary>
/// タイヤ跡生成コンテキスト
/// </summary>
struct GenerateContext {
    OriGine::Vec3f position;
    float speed;
    float alpha;
    float segmentLength;
};

/// <summary>
/// タイヤ跡生成システム
/// </summary>
class TireTrailGenerateSystem
    : public OriGine::ISystem {
public:
    TireTrailGenerateSystem();
    ~TireTrailGenerateSystem() override;

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
    /// <summary>
    /// 生成コンテキストを構築する
    /// </summary>
    /// <param name="spline"></param>
    /// <param name="out"></param>
    /// <returns></returns>
    bool BuildGenerateContext(
        TireSplinePoints& spline,
        GenerateContext& out);

    /// <summary>
    /// 最低限の制御点数を確保する
    /// </summary>
    void EnsureMinimumControlPoints(
        TireSplinePoints& spline,
        const GenerateContext& ctx);

    /// <summary>
    /// 新しい制御点を追加する
    /// </summary>
    void AppendNewPoints(
        TireSplinePoints& spline,
        const GenerateContext& ctx);

    /// <summary>
    /// 制御点を再サンプリングする
    /// </summary>
    /// <param name="spline"></param>
    void ResamplePoints(TireSplinePoints& spline);

    /// <summary>
    /// フェードアウト処理を更新する
    /// </summary>
    /// <param name="spline"></param>
    /// <param name="entity"></param>
    void UpdateFadeOut(
        TireSplinePoints& _spline,
        OriGine::EntityHandle _entity);
};
