#pragma once

/// parent
#include "system/render/base/BaseRenderSystem.h"

/// engine
#include "directX12/ShaderManager.h"
#include "component/renderer/LineRenderer.h"

/// ECS
// component
#include "component/gimmick/PathController.h"

/// <summary>
/// PathController の制御点とパスラインをデバッグ用に描画するシステム。
/// 非 Release ビルドでのみ登録される。
/// </summary>
class PathControllerRenderingSystem
    : public OriGine::BaseRenderSystem {
public:
    /// <summary>
    /// バッファ初期化時の最大頂点・インデックス数
    /// </summary>
    static const int32_t kDefaultVertexCount_;

public:
    PathControllerRenderingSystem();
    ~PathControllerRenderingSystem() override;

    void Initialize() override;
    void Update() override;
    void Finalize() override;

private:
    void CreatePSO() override;
    void StartRender() override;

    /// <summary>
    /// PathController の情報からラインメッシュを生成する
    /// </summary>
    void CreateRenderMesh();

    /// <summary>
    /// 描画コマンドを発行する
    /// </summary>
    void RenderCall();

    void Rendering() override;
    bool ShouldSkipRender() const override;

private:
    OriGine::PipelineStateObj* pso_ = nullptr;

    OriGine::ComponentArray<PathController>* pathControllers_ = nullptr;

    std::unique_ptr<OriGine::LineRenderer> pathRenderer_;
    std::vector<OriGine::Mesh<OriGine::ColorVertexData>>::iterator pathMeshItr_;
};
