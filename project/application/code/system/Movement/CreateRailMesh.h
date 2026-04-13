#pragma once

#include "system/ISystem.h"

/// ECS
// component
#include "component/gimmick/RailPoints.h"
#include "component/renderer/primitive/CylinderRenderer.h"

/// <summary>
/// レールメッシュ作成システム
/// RailPointsコンポーネントのポイント列からシリンダー形状のメッシュを生成する
/// </summary>
class CreateRailMesh
    : public OriGine::ISystem {
public:
    CreateRailMesh();
    ~CreateRailMesh() override;

private:
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// エンティティごとの更新処理
    /// </summary>
    void UpdateEntity(OriGine::EntityHandle _handle) override;

    /// <summary>
    /// RailPointsからシリンダーメッシュを生成する
    /// CylinderRendererのPrimitiveからradialDivisionsを取得して使用
    /// </summary>
    /// <param name="_renderer">メッシュを設定するレンダラー</param>
    /// <param name="_railPoints">レールの制御点コンポーネント</param>
    void CreateCylinderMeshFromRail(
        OriGine::CylinderRenderer* _renderer,
        const RailPoints* _railPoints);
};
