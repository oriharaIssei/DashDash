#include "PathControllerRenderingSystem.h"

/// engine
#include "Engine.h"
#include "camera/CameraManager.h"
#include "directX12/DxDevice.h"

/// ECS
#include "component/transform/Transform.h"

/// math
#include "math/MathEnv.h"

using namespace OriGine;

//-------------------------------------------------------
// 定数
//-------------------------------------------------------

const int32_t PathControllerRenderingSystem::kDefaultVertexCount_ = 16384;

/// 制御点マーカーの腕の半長
static constexpr float kPointMarkerHalfArm = 0.3f;

/// 1制御点あたりの頂点数（3軸 × 両端 = 6）
static constexpr int32_t kVertexPerPoint   = 6;
/// 1セグメントあたりの頂点数
static constexpr int32_t kVertexPerSegment = 2;

/// 制御点の色：黄色
static constexpr Vec4f kPointColor   = {1.0f, 1.0f, 0.0f, 1.0f};
/// ラインの色：白
static constexpr Vec4f kLineColor    = {1.0f, 1.0f, 1.0f, 1.0f};

//-------------------------------------------------------
// 内部ヘルパー
//-------------------------------------------------------
namespace {

/// <summary>
/// 制御点のXYZ軸クロスマーカーをメッシュに追加する
/// </summary>
void AddPointMarker(
    OriGine::Mesh<OriGine::ColorVertexData>& _mesh,
    const Vec3f& _pos,
    const Vec4f& _color,
    float _halfArm) {

    const Vec3f offsets[3] = {axisX * _halfArm, axisY * _halfArm, axisZ * _halfArm};

    for (const auto& offset : offsets) {
        uint32_t base = static_cast<uint32_t>(_mesh.vertexes_.size());
        _mesh.vertexes_.push_back({Vec4f(_pos - offset, 1.f), _color});
        _mesh.vertexes_.push_back({Vec4f(_pos + offset, 1.f), _color});
        _mesh.indexes_.push_back(base);
        _mesh.indexes_.push_back(base + 1);
    }
}

/// <summary>
/// 2点間のラインをメッシュに追加する
/// </summary>
void AddLine(
    OriGine::Mesh<OriGine::ColorVertexData>& _mesh,
    const Vec3f& _from,
    const Vec3f& _to,
    const Vec4f& _color) {

    uint32_t base = static_cast<uint32_t>(_mesh.vertexes_.size());
    _mesh.vertexes_.push_back({Vec4f(_from, 1.f), _color});
    _mesh.vertexes_.push_back({Vec4f(_to,   1.f), _color});
    _mesh.indexes_.push_back(base);
    _mesh.indexes_.push_back(base + 1);
}

} // namespace

//-------------------------------------------------------
// システム本体
//-------------------------------------------------------

PathControllerRenderingSystem::PathControllerRenderingSystem() : BaseRenderSystem() {}
PathControllerRenderingSystem::~PathControllerRenderingSystem() = default;

void PathControllerRenderingSystem::Initialize() {
    BaseRenderSystem::Initialize();

    pathControllers_ = GetComponentArray<PathController>();

    pathRenderer_ = std::make_unique<LineRenderer>(std::vector<Mesh<ColorVertexData>>());
    pathRenderer_->Initialize(nullptr, EntityHandle());
    pathRenderer_->GetMeshGroup()->push_back(Mesh<ColorVertexData>());
    pathRenderer_->GetMeshGroup()->back().Initialize(
        static_cast<UINT>(kDefaultVertexCount_),
        static_cast<UINT>(kDefaultVertexCount_));

    pathMeshItr_ = pathRenderer_->GetMeshGroup()->begin();
}

void PathControllerRenderingSystem::Update() {
    if (ShouldSkipRender()) {
        return;
    }
    CreateRenderMesh();
    Rendering();
}

void PathControllerRenderingSystem::Finalize() {
    if (pathRenderer_) {
        pathRenderer_->Finalize();
    }
    dxCommand_->Finalize();
}

void PathControllerRenderingSystem::CreateRenderMesh() {
    // 前フレームのデータをクリア
    for (auto& mesh : *pathRenderer_->GetMeshGroup()) {
        mesh.vertexes_.clear();
        mesh.indexes_.clear();
    }
    pathMeshItr_ = pathRenderer_->GetMeshGroup()->begin();

    if (!pathControllers_) {
        return;
    }

    for (auto& slot : pathControllers_->GetSlots()) {
        Entity* entity = GetEntity(slot.owner);
        if (!entity) {
            continue;
        }

        PathController* pathCtrl = GetComponent<PathController>(slot.owner);
        if (!pathCtrl || !pathCtrl->isActive) {
            continue;
        }

        const auto& pts     = pathCtrl->points;
        const int32_t n     = static_cast<int32_t>(pts.size());
        if (n < 1) {
            continue;
        }

        // 各制御点に必要な頂点数：クロスマーカー(6) + ライン端点(2) ≒ 8N
        const int32_t needed = n * (kVertexPerPoint + kVertexPerSegment);

        // バッファ容量が足りない場合は新しいメッシュを追加
        if (pathMeshItr_->GetIndexCapacity() < needed) {
            pathMeshItr_->TransferData();
            ++pathMeshItr_;
            if (pathMeshItr_ == pathRenderer_->GetMeshGroup()->end()) {
                pathRenderer_->GetMeshGroup()->push_back(Mesh<ColorVertexData>());
                pathRenderer_->GetMeshGroup()->back().Initialize(
                    static_cast<UINT>(kDefaultVertexCount_),
                    static_cast<UINT>(kDefaultVertexCount_));
                pathMeshItr_ = --pathRenderer_->GetMeshGroup()->end();
            }
        }

        auto& mesh = *pathMeshItr_;

        // --- 制御点マーカー ---
        for (int32_t i = 0; i < n; ++i) {
            AddPointMarker(mesh, pts[i], kPointColor, kPointMarkerHalfArm);
        }

        // --- 制御点間のライン ---
        for (int32_t i = 0; i < n - 1; ++i) {
            AddLine(mesh, pts[i], pts[i + 1], kLineColor);
        }
    }

    pathMeshItr_->TransferData();
}

void PathControllerRenderingSystem::RenderCall() {
    auto& commandList = dxCommand_->GetCommandList();
    pathRenderer_->GetTransformBuff().SetForRootParameter(commandList, 0);

    for (auto& mesh : *pathRenderer_->GetMeshGroup()) {
        if (mesh.indexes_.empty()) {
            continue;
        }
        commandList->IASetVertexBuffers(0, 1, &mesh.GetVertexBufferView());
        commandList->IASetIndexBuffer(&mesh.GetIndexBufferView());
        commandList->DrawIndexedInstanced(static_cast<UINT>(mesh.indexes_.size()), 1, 0, 0, 0);
    }
}

void PathControllerRenderingSystem::Rendering() {
    StartRender();
    RenderCall();
}

bool PathControllerRenderingSystem::ShouldSkipRender() const {
    return !pathControllers_ || pathControllers_->IsEmpty();
}

void PathControllerRenderingSystem::CreatePSO() {
    ShaderManager* shaderManager = ShaderManager::GetInstance();
    DxDevice* dxDevice           = Engine::GetInstance()->GetDxDevice();

    shaderManager->LoadShader("ColoredVertex.VS");
    shaderManager->LoadShader("ColoredVertex.PS", kShaderDirectory, L"ps_6_0");

    ShaderInfo shaderInfo{};
    shaderInfo.vsKey = "ColoredVertex.VS";
    shaderInfo.psKey = "ColoredVertex.PS";

    D3D12_ROOT_PARAMETER rootParameter[2]{};
    rootParameter[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameter[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameter[0].Descriptor.ShaderRegister = 0;
    shaderInfo.pushBackRootParameter(rootParameter[0]);

    rootParameter[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameter[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameter[1].Descriptor.ShaderRegister = 1;
    shaderInfo.pushBackRootParameter(rootParameter[1]);

    D3D12_INPUT_ELEMENT_DESC inputElementDesc{};
    inputElementDesc.SemanticName      = "POSITION";
    inputElementDesc.Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    shaderInfo.pushBackInputElementDesc(inputElementDesc);

    inputElementDesc.SemanticName      = "COLOR";
    inputElementDesc.Format            = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    shaderInfo.pushBackInputElementDesc(inputElementDesc);

    shaderInfo.topologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    shaderInfo.changeCullMode(D3D12_CULL_MODE_NONE);

    pso_ = shaderManager->CreatePso("PathControllerLine", shaderInfo, dxDevice->device_);
}

void PathControllerRenderingSystem::StartRender() {
    auto& commandList = dxCommand_->GetCommandList();
    commandList->SetGraphicsRootSignature(pso_->rootSignature.Get());
    commandList->SetPipelineState(pso_->pipelineState.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    CameraManager::GetInstance()->SetBufferForRootParameter(GetScene(), commandList, 1);
}
