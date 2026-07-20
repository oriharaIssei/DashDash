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

    /// <summary>
    /// ラインレンダラーとメッシュバッファを初期化する
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 毎フレームの更新。描画不要な場合はスキップし、それ以外はメッシュ生成と描画を行う
    /// </summary>
    void Update() override;
    /// <summary>
    /// ラインレンダラーとコマンドを解放する
    /// </summary>
    void Finalize() override;

private:
    /// <summary>
    /// デバッグライン描画用のPSO（パイプラインステートオブジェクト）を生成する
    /// </summary>
    void CreatePSO() override;
    /// <summary>
    /// 描画コマンドリストにPSO・ルートシグネチャ・トポロジ・カメラ用バッファを設定する
    /// </summary>
    void StartRender() override;

    /// <summary>
    /// PathController の情報からラインメッシュを生成する
    /// </summary>
    void CreateRenderMesh();

    /// <summary>
    /// 描画コマンドを発行する
    /// </summary>
    void RenderCall();

    /// <summary>
    /// 描画処理本体。StartRender と RenderCall を順に実行する
    /// </summary>
    void Rendering() override;
    /// <summary>
    /// 描画をスキップすべきか判定する（PathController が存在しない場合は true）
    /// </summary>
    /// <returns>スキップすべきなら true</returns>
    bool ShouldSkipRender() const override;

private:
    OriGine::PipelineStateObj* pso_ = nullptr;

    OriGine::ComponentArray<PathController>* pathControllers_ = nullptr;

    std::unique_ptr<OriGine::LineRenderer> pathRenderer_;
    std::vector<OriGine::Mesh<OriGine::ColorVertexData>>::iterator pathMeshItr_;
};
