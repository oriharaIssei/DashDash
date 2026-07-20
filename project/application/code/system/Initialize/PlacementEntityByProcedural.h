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

    /// <summary>
    /// コマンドリストの生成とPSOの作成を行う
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// PSO・コマンドリストを解放する
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 点群配置用ComputeShaderのPSO（パイプラインステートオブジェクト）を生成する。
    /// 既に登録済みの場合はそれを再利用する。
    /// </summary>
    void CreatePSO();

protected:
    /// <summary>
    /// PointPlacementParamsを持つエンティティに対して点群配置のComputeShaderを実行し、
    /// 出力された各点にエンティティテンプレートを複製・配置する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

    /// <summary>
    /// ComputeShaderの実行準備（ディスクリプタヒープ・PSO・ルートシグネチャの設定）を行う
    /// </summary>
    void StartCS();
    /// <summary>
    /// コマンドリストを実行し、フェンスで完了を待機してからコマンドリストをリセットする
    /// </summary>
    void ExecuteCS();

private:
    OriGine::PipelineStateObj* pso_ = nullptr;

    std::unique_ptr<OriGine::DxCommand> dxCommand_ = nullptr;

    // 出力点群バッファ（UAV）のルートパラメータインデックス
    size_t outputPointsBuffIndex_ = 0;
    // 配置パラメータ（CBV）のルートパラメータインデックス
    size_t paramBuffIndex_        = 0;
    // 密度テクスチャ（SRV）のルートパラメータインデックス
    size_t pointDensityBuffIndex_ = 0;
    // ボリュームテクスチャ（SRV）のルートパラメータインデックス
    size_t pointVolumeBuffIndex_  = 0;
    // マスクテクスチャ（SRV）のルートパラメータインデックス
    size_t pointMaskBuffIndex_    = 0;
};
