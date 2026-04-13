
#pragma once

#include "IPlayerMoveState.h"

/// stl
#include <deque>

/// ECS
// entity
#include "entity/EntityHandle.h"

// Component
struct Transform;
#include "component/physics/Rigidbody.h"

/// Math
#include "Matrix4x4.h"

/// <summary>
/// Playerの壁走り状態
/// </summary>
class PlayerWallRunState
    : public IPlayerMoveState {
public:
    PlayerWallRunState(OriGine::Scene* _scene, OriGine::EntityHandle _playerEntityHandle) : IPlayerMoveState(_scene, _playerEntityHandle, PlayerMoveState::WALL_RUN) {}
    ~PlayerWallRunState() override {};

    void Initialize() override;
    void Update(float _deltaTime) override;
    void Finalize() override;

    PlayerMoveState TransitionState() const override;

protected:
    OriGine::EntityHandle pathEntityHandle_ = OriGine::EntityHandle(); // 壁走り軌道エンティティハンドル

    bool isRightWall_ = false; // 壁が右側にあるかどうか

    OriGine::Vec3f prevVelo_   = OriGine::Vec3f(0.0f, 0.0f, 0.0f); // 前の速度 壁走り前の速度を保存
    float separationGraceTime_ = 0.04f; // オブジェクトが離れていると判定するまでの猶予時間
    float separationLeftTime_  = 0.0f; // 壁との衝突判定の残り時間

    float thresholdFailedWallJumpSpeed_ = 0.3f; // 壁ジャンプ失敗と判定する速度閾値

    float playerSpeed_      = 0.f; // playerの速度
    float speedRate_        = 1.f; // 壁走り速度倍率
    float speedRumpUpTime_  = 0.f; // 壁走り速度上昇時間
    float speedRumpUpTimer_ = 0.0f; // 壁走り速度上昇タイマー

    float wallRunDetachSpeed_ = 5.f; // 壁走り離脱速度
    float gravityApplyDelay_  = 0.2f; // 重力適用遅延時間

    OriGine::Vec3f wallNormal_ = OriGine::Vec3f(0.0f, 0.0f, 0.0f); // 壁の法線

    OriGine::Vec3f cameraTargetOffsetOnWallRun_ = OriGine::Vec3f(0.0f, 0.0f, 0.0f);
    float minCameraTargetOffsetXOnWallRun_      = 0.f;
    OriGine::Vec3f cameraOffsetOnWallRun_       = OriGine::Vec3f(0.0f, 0.0f, 0.0f);

    const float kCameraAngleLerpTime_ = 1.8f;
    float cameraAngleLerpTimer_       = 0.0f;
};
