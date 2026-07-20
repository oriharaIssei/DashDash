#pragma once
#include "system/ISystem.h"

/// ECS
// component
#include "component/renderer/primitive/PlaneRenderer.h"
#include "component/spline/SplinePoints.h"

/// <summary>
/// スプラインからメッシュを生成するシステム
/// </summary>
class CreateMeshFromSpline
    : public OriGine::ISystem {
public:
    CreateMeshFromSpline();
    ~CreateMeshFromSpline() override;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

private:
    /// <summary>
    /// SplinePointsを持つEntityごとに、設定に応じてクロス状/ライン状のメッシュを生成する
    /// </summary>
    /// <param name="_handle">対象のエンティティハンドル</param>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

    /// <summary>
    /// スプラインに沿って縦横2枚のプレーンを十字に交差させたメッシュを生成する
    /// </summary>
    /// <param name="_planeRendererComp">メッシュの出力先となるPlaneRenderer</param>
    /// <param name="_splinePointsComp">元になるスプライン点群</param>
    void CreateCrossPlaneMesh(OriGine::PlaneRenderer* _planeRendererComp, SplinePoints* _splinePointsComp);
    /// <summary>
    /// スプラインに沿って1枚のプレーンを帯状に連結したメッシュを生成する
    /// </summary>
    /// <param name="_planeRendererComp">メッシュの出力先となるPlaneRenderer</param>
    /// <param name="_splinePointsComp">元になるスプライン点群</param>
    void CreateLinePlaneMesh(OriGine::PlaneRenderer* _planeRendererComp, SplinePoints* _splinePointsComp);
};
