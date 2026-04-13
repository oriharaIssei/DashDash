#pragma once
#include "system/ISystem.h"

/// ECS
// component
#include "component/renderer/primitive/PlaneRenderer.h"
#include "component/spline/TireSplinePoints.h"

/// <summary>
/// TireSplinePointsの制御点からメッシュを生成するシステム
/// </summary>
class CreateMeshFromTireSpline
    : public OriGine::ISystem {
public:
    CreateMeshFromTireSpline();
    ~CreateMeshFromTireSpline() override;

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

    void CreateCrossPlaneMesh(OriGine::PlaneRenderer* _planeRendererComp, TireSplinePoints* _splinePointsComp);
    void CreateLinePlaneMesh(OriGine::PlaneRenderer* _planeRendererComp, TireSplinePoints* _splinePointsComp);
};
