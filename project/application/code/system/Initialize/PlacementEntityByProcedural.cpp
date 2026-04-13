#include "PlacementEntityByProcedural.h"

/// engine
#include "Engine.h"
#include "scene/SceneFactory.h"
#include "scene/SceneJsonRegistry.h"
#include "asset/AssetSystem.h"
// asset
#include "asset/TextureAsset.h"
// directX12
#include "directX12/DxDevice.h"
#include "directX12/DxFence.h"
#include "directX12/ShaderManager.h"

/// ECS
// component
#include "component/PointPlacementParams.h"
#include "component/transform/Transform.h"

using namespace OriGine;

PlacementEntityByProcedural::PlacementEntityByProcedural() : ISystem(SystemCategory::Initialize) {}
PlacementEntityByProcedural::~PlacementEntityByProcedural() {}

void PlacementEntityByProcedural::Initialize() {
    dxCommand_ = std::make_unique<DxCommand>();
    dxCommand_->Initialize("main", "main");

    CreatePSO();
}

void PlacementEntityByProcedural::Finalize() {
    pso_ = nullptr;

    dxCommand_->Finalize();
}

void PlacementEntityByProcedural::CreatePSO() {
    constexpr const char* psoKey = "PlacementPointProcess.CS";

    if (pso_) {
        return; // PSOが既に作成されている場合は何もしない
    }

    ShaderManager* shaderManager = ShaderManager::GetInstance();
    DxDevice* dxDevice           = Engine::GetInstance()->GetDxDevice();

    if (shaderManager->IsRegisteredPipelineStateObj(psoKey)) {
        pso_ = shaderManager->GetPipelineStateObj(psoKey);

        outputPointsBuffIndex_ = 0;
        paramBuffIndex_        = 1;
        pointDensityBuffIndex_ = 2;
        pointVolumeBuffIndex_  = 3;
        pointMaskBuffIndex_    = 4;

        return; // PSOが既に登録されている場合はそれを使用
    }

    // PSOが登録されていない場合は新規に作成

    /// ==========================================
    // Shader 読み込み
    /// ==========================================
    shaderManager->LoadShader(psoKey, kShaderDirectory, L"cs_6_0");

    ShaderInfo shaderInfo{};
    shaderInfo.csKey = psoKey;

#pragma region "ROOT_PARAMETER"
    D3D12_ROOT_PARAMETER rootParameters[5]{};
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[0].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    outputPointsBuffIndex_             = shaderInfo.pushBackRootParameter(rootParameters[0]);

    rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].Descriptor.ShaderRegister = 0; // b0
    paramBuffIndex_                             = shaderInfo.pushBackRootParameter(rootParameters[1]);

    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[2].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pointDensityBuffIndex_             = shaderInfo.pushBackRootParameter(rootParameters[2]);

    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[3].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pointVolumeBuffIndex_              = shaderInfo.pushBackRootParameter(rootParameters[3]);

    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParameters[4].ParameterType    = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    pointMaskBuffIndex_                = shaderInfo.pushBackRootParameter(rootParameters[4]);

    D3D12_DESCRIPTOR_RANGE outputDescriptorRange[1]            = {};
    outputDescriptorRange[0].BaseShaderRegister                = 0; // u0
    outputDescriptorRange[0].NumDescriptors                    = 1;
    outputDescriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    outputDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    shaderInfo.SetDescriptorRange2Parameter(
        outputDescriptorRange, 1, outputPointsBuffIndex_);

    D3D12_DESCRIPTOR_RANGE pointDensityDescriptorRange[1]            = {};
    pointDensityDescriptorRange[0].BaseShaderRegister                = 0; // t0
    pointDensityDescriptorRange[0].NumDescriptors                    = 1;
    pointDensityDescriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    pointDensityDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    shaderInfo.SetDescriptorRange2Parameter(
        pointDensityDescriptorRange, 1, pointDensityBuffIndex_);

    D3D12_DESCRIPTOR_RANGE pointVolumeDescriptorRange[1]            = {};
    pointVolumeDescriptorRange[0].BaseShaderRegister                = 1; // t1
    pointVolumeDescriptorRange[0].NumDescriptors                    = 1;
    pointVolumeDescriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    pointVolumeDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    shaderInfo.SetDescriptorRange2Parameter(
        pointVolumeDescriptorRange, 1, pointVolumeBuffIndex_);

    D3D12_DESCRIPTOR_RANGE pointMaskDescriptorRange[1]            = {};
    pointMaskDescriptorRange[0].BaseShaderRegister                = 2; // t2
    pointMaskDescriptorRange[0].NumDescriptors                    = 1;
    pointMaskDescriptorRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    pointMaskDescriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    shaderInfo.SetDescriptorRange2Parameter(
        pointMaskDescriptorRange, 1, pointMaskBuffIndex_);

#pragma endregion

    ///=================================================
    /// Sampler
    D3D12_STATIC_SAMPLER_DESC staticSampler = {};
    staticSampler.Filter                    = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイナリニアフィルタ
    // 0 ~ 1 の間をリピート
    staticSampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

    staticSampler.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
    staticSampler.MinLOD           = 0;
    staticSampler.MaxLOD           = D3D12_FLOAT32_MAX;
    staticSampler.ShaderRegister   = 0;
    staticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    shaderInfo.pushBackSamplerDesc(staticSampler);
    /// Sampler
    ///=================================================

    /// ==========================================
    // PSOの作成
    /// ==========================================
    pso_ = shaderManager->CreatePso(psoKey, shaderInfo, dxDevice->device_);
}

void PlacementEntityByProcedural::UpdateEntity(OriGine::EntityHandle _handle) {
    auto& commandList = dxCommand_->GetCommandList();

    auto* entityTransform         = GetComponent<Transform>(_handle);
    auto& pointPlacementParamsVec = GetComponents<PointPlacementParams>(_handle);

    StartCS();
    for (auto& pointPlacementParams : pointPlacementParamsVec) {
        // パラメータバッファの設定
        pointPlacementParams.GetParamData().ConvertToBuffer();
        pointPlacementParams.GetParamData().SetForComputeRootParameter(
            dxCommand_->GetCommandList(),
            static_cast<uint32_t>(paramBuffIndex_));

        // 出力バッファの設定
        dxCommand_->ResourceBarrier(
            pointPlacementParams.GetOutputPoints().GetBufferResource().GetResource(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        pointPlacementParams.GetOutputPoints().SetForComputeRootParameter(
            dxCommand_->GetCommandList(),
            static_cast<uint32_t>(outputPointsBuffIndex_));

        // テクスチャの設定
        commandList->SetComputeRootDescriptorTable(
            static_cast<uint32_t>(pointDensityBuffIndex_),
            AssetSystem::GetInstance()->GetManager<TextureAsset>()->GetAsset(pointPlacementParams.GetPointDensityTextureIndex()).srv.GetGpuHandle());
        commandList->SetComputeRootDescriptorTable(
            static_cast<uint32_t>(pointVolumeBuffIndex_),
            AssetSystem::GetInstance()->GetManager<TextureAsset>()->GetAsset(pointPlacementParams.GetPointVolumeTextureIndex()).srv.GetGpuHandle());
        commandList->SetComputeRootDescriptorTable(
            static_cast<uint32_t>(pointMaskBuffIndex_),
            AssetSystem::GetInstance()->GetManager<TextureAsset>()->GetAsset(pointPlacementParams.GetPointMaskTextureIndex()).srv.GetGpuHandle());

        // Dispatch
        uint32_t res = pointPlacementParams.GetParamData().openData_.placementResolution;

        uint32_t groupX = (res + 31) / 32;
        uint32_t groupY = (res + 31) / 32;

        commandList->Dispatch(groupX, groupY, 1);

        ExecuteCS();

        // 出力バッファの要素数を更新
        pointPlacementParams.GetOutputPoints().ConvertFromBuffer(dxCommand_.get());

        // Entityを配置
        const std::string& originalEntityFileName = pointPlacementParams.GetOriginalEntityFileName();
        SceneFactory factory                      = {};
        Scene* scene                              = GetScene();

        const Vec3f& defaultScale = pointPlacementParams.GetDefaultScale();

        if (!originalEntityFileName.empty()) {
            for (const auto& point : pointPlacementParams.GetOutputPoints().GetOpenData()) {
                Entity* cloneEntity = factory.BuildEntityFromTemplate(scene, originalEntityFileName);
                if (!cloneEntity) {
                    if (!SceneJsonRegistry::GetInstance()->LoadEntityTemplate(pointPlacementParams.GetOriginalEntityDirectory(), originalEntityFileName)) {
                        LOG_ERROR("Failed to load entity template from file '{}/{}'.", pointPlacementParams.GetOriginalEntityDirectory(), originalEntityFileName);
                        break;
                    }
                    cloneEntity = factory.BuildEntityFromTemplate(scene, originalEntityFileName);
                }
                cloneEntity->SetShouldSave(false); // 配置したエンティティは保存しない
                EntityHandle clonedHandle = cloneEntity->GetHandle();

                Transform* transform = GetComponent<Transform>(clonedHandle);
                if (!transform) {
                    LOG_ERROR("Cloned entity hasn't TransformComponent.");
                    break;
                }

                transform->scale = Vec3f(point.volume, point.volume, point.volume) * defaultScale;

                transform->translate = point.pos;

                transform->parent = entityTransform;

                transform->UpdateMatrix();
            }
        }
    }
}

void PlacementEntityByProcedural::StartCS() {
    if (!pso_) {
        LOG_ERROR("PSO is not created for SkinningAnimationSystem");
        return;
    }

    ID3D12DescriptorHeap* ppHeaps[] = {
        Engine::GetInstance()->GetSrvHeap()->GetHeap().Get()};
    dxCommand_->GetCommandList()->SetDescriptorHeaps(1, ppHeaps);

    dxCommand_->GetCommandList()->SetPipelineState(pso_->pipelineState.Get());
    dxCommand_->GetCommandList()->SetComputeRootSignature(pso_->rootSignature.Get());
}

void PlacementEntityByProcedural::ExecuteCS() {
    HRESULT hr;
    DxFence* fence = Engine::GetInstance()->GetDxFence();

    // コマンドの受付終了 -----------------------------------
    hr = dxCommand_->Close();
    if (FAILED(hr)) {
        LOG_ERROR("Failed to close command list. HRESULT: {}", std::to_string(hr));
        assert(false);
    }
    //----------------------------------------------------

    ///===============================================================
    /// コマンドリストの実行
    ///===============================================================
    dxCommand_->ExecuteCommand();
    ///===============================================================

    ///===============================================================
    /// コマンドリストの実行を待つ
    ///===============================================================
    UINT64 fenceVal = fence->Signal(dxCommand_->GetCommandQueue());
    fence->WaitForFence(fenceVal);
    ///===============================================================

    ///===============================================================
    /// リセット
    ///===============================================================
    dxCommand_->CommandReset();
    ///===============================================================
}
