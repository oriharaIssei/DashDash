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
    bool TryGetContext(OriGine::EntityHandle _handle, Context& _outContext);
    void ProcessMovement(Context& _ctx);
    void ProcessIdle(Context& _ctx);

    // 詳細ロジック
    void AppendNewPoints(SplinePoints* _splinePoints, const OriGine::Vec3f& _targetPos);
    void RefineSplinePoints(SplinePoints* _splinePoints);

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
