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

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

    void CreateCrossPlaneMesh(OriGine::PlaneRenderer* _planeRendererComp, SplinePoints* _splinePointsComp);
    void CreateLinePlaneMesh(OriGine::PlaneRenderer* _planeRendererComp, SplinePoints* _splinePointsComp);
};
