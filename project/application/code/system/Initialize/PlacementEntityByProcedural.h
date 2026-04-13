#pragma once
#include "system/ISystem.h"

/// engine
// directX12
#include "directX12/DxCommand.h"
#include "directX12/PipelineStateObj.h"

/// <summary>
/// プロシージャルによるEntity配置システム
/// </summary>
class PlacementEntityByProcedural
    : public OriGine::ISystem {
public:
    PlacementEntityByProcedural();
    ~PlacementEntityByProcedural() override;

    void Initialize() override;
    void Finalize() override;

    void CreatePSO();

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

    void StartCS();
    void ExecuteCS();

private:
    OriGine::PipelineStateObj* pso_ = nullptr;

    std::unique_ptr<OriGine::DxCommand> dxCommand_ = nullptr;

    size_t outputPointsBuffIndex_ = 0;
    size_t paramBuffIndex_        = 0;
    size_t pointDensityBuffIndex_ = 0;
    size_t pointVolumeBuffIndex_  = 0;
    size_t pointMaskBuffIndex_    = 0;
};
