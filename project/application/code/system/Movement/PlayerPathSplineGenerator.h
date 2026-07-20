#pragma once
#include "system/ISystem.h"

/// ECS
// entity
#include "entity/EntityHandle.h"

// component
#include "component/physics/Rigidbody.h"
#include "component/spline/SplinePoints.h"
#include "component/transform/Transform.h"

/// <summary>
/// プレイヤーの移動経路スプラインを生成するシステム
/// </summary>
class PlayerPathSplineGenerator
    : public OriGine::ISystem {
public:
    PlayerPathSplineGenerator();
    ~PlayerPathSplineGenerator() override;

    void Initialize() override;
    void Finalize() override;

public:
    /// <summary>
    /// システムで使用するコンポーネントをまとめたコンテキスト構造体
    /// </summary>
    struct Context {
        SplinePoints* splinePoints;
        OriGine::Transform* playerTransform;
        OriGine::Rigidbody* playerRigidBody;
    };

public:
    // ヘルパー関数群
    /// <summary>
    /// エンティティからSplinePoints・プレイヤーのTransform/Rigidbodyを取得し、Contextにまとめる
    /// </summary>
    /// <param name="_handle">対象エンティティ</param>
    /// <param name="_outContext">取得結果の格納先</param>
    /// <returns>必要なコンポーネントが全て揃っていればtrue</returns>
    bool TryGetContext(const OriGine::EntityHandle& _handle, Context& _outContext);
    /// <summary>
    /// プレイヤー移動中の処理。現在位置に新しい点を追加し、パス全体を再構築する
    /// </summary>
    void ProcessMovement(Context& _ctx);
    /// <summary>
    /// プレイヤー停止中の処理。フェードアウトタイマーを進め、時間経過で先頭の点を削除する
    /// </summary>
    void ProcessIdle(Context& _ctx);

    // 詳細ロジック
    /// <summary>
    /// プレイヤーの現在位置へ向けて、セグメント長に応じた補間点を追加する
    /// </summary>
    void AppendNewPoints(SplinePoints* _splinePoints, const OriGine::Vec3f& _targetPos);
    /// <summary>
    /// 隣接する点同士の距離を基準に、長すぎる区間を分割・短すぎる区間を統合して点列を整える
    /// </summary>
    void RefineSplinePoints(SplinePoints* _splinePoints);

private:
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
