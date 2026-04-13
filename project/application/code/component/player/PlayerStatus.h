#pragma once
#include "component/IComponent.h"

/// engine
#include "scene/Scene.h"

/// ECS
// component
#include "component/physics/Rigidbody.h"
#include "component/transform/Transform.h"
/// math
#include <math/MyEasing.h>
#include <math/Quaternion.h>
#include <math/Vector3.h>

#include "PlayerConfig.h"

/// 前方宣言
class PlayerInput;

constexpr float kPlayerHeight = AppConfig::Player::kDefaultHeight;

/// <summary>
/// Playerのステータスを管理するコンポーネント
/// </summary>
class PlayerStatus
    : public OriGine::IComponent {
    /// <summary>
    /// JSON 変換用
    /// </summary>
    friend void to_json(nlohmann::json& _j, const PlayerStatus& _playerStatus);
    /// <summary>
    /// JSON 復元用
    /// </summary>
    friend void from_json(const nlohmann::json& _j, PlayerStatus& _playerStatus);

public:
    PlayerStatus();
    ~PlayerStatus();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    /// <summary>
    /// エディタ用編集UI
    /// </summary>
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    /// <summary>
    /// デバッグ用表示
    /// </summary>
    void Debug(OriGine::Scene* _scene, OriGine::EntityHandle _handle, const std::string& _parentLabel) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// ギアレベルに応じた速度を計算する
    /// </summary>
    /// <param name="_gearLevel">ギアレベル</param>
    /// <returns>速度</returns>
    float CalculateSpeedByGearLevel(int32_t _gearLevel) const;
    /// <summary>
    /// ギアレベルに応じたクールタイムを計算する
    /// </summary>
    /// <param name="_gearLevel">ギアレベル</param>
    /// <returns>クールタイム</returns>
    float CalculateCoolTimeByGearLevel(int32_t _gearLevel) const;
    /// <summary>
    /// ギアレベルに応じた方向別速度を計算する
    /// </summary>
    /// <param name="_gearLevel"></param>
    /// <returns></returns>
    OriGine::Vec2f CalculateCurrentMaxDirectionalSpeed(int32_t _gearLevel) const;

    /// <summary>
    /// ギアアップしたときのステータスの更新処理
    /// </summary>
    /// <param name="_gearLevel"></param>
    void SetupOnGearUp(int32_t _gearLevel);

    /// <summary>
    /// 現在の移動方向を滑らかにする
    /// </summary>
    /// <param name="_rigidbody"></param>
    /// <param name="_transform"></param>
    /// <returns></returns>
    OriGine::Vec3f ComputeSmoothedDirection(const OriGine::Vec3f& _targetDir, const OriGine::Rigidbody* _rigidbody, const OriGine::Transform* _transform, float _deltaTime) const;

private:
    /// ==========================================
    // 能力値
    float decelerationFactor_        = 0.f; // 減速の係数 (0以上1以下の値で、1に近いほど減速しにくい)
    float baseGearupCoolTime_        = AppConfig::Player::kDefaultBaseGearupCoolTime; // ギアアップの基本クールタイム (秒単位)
    float gearUpCoolTime_            = 0.0f; // ギアレベルが上がるまでの時間
    float coolTimeAddRateBase_       = 1.0f;
    float coolTimeAddRateCommonRate_ = 1.f;

    // 速度
    float baseSpeed_             = 0.0f; // 基本速度 (ギアレベル0の時の速度)
    float speedUpRateBase_       = 1.0f; // ギアアップ時の速度上昇率の基本値
    float speedUpRateCommonRate_ = 1.f; // ギアアップ時の速度上昇率の共通値
    // currentMaxSpeed は gearLevel に応じて変化する
    float currentMaxSpeed_ = 0.0f; // 現在の最大速度

    OriGine::Vec2f baseDirectionalSpeed_             = {0.0f, 0.0f}; // 基本方向別速度 (X:前後, Y:左右)
    OriGine::Vec2f directionalSpeedUpRateBase_       = {1.0f, 1.0f}; // ギアアップ時の方向別速度上昇率の基本値
    OriGine::Vec2f directionalSpeedUpRateCommonRate_ = {1.f, 1.f}; // ギアアップ時の方向別速度上昇率の共通値
    OriGine::Vec2f currentMaxDirectionalSpeed_       = {0.0f, 0.0f}; // 現在の最大方向別速度 (X:前後, Y:左右)
    OriGine::Vec2f currentDirectionalSpeed_          = {0.0f, 0.0f}; // 現在の方向別速度 (X:前後, Y:左右)

    OriGine::Vec2f minSmoothTime_ = {0.1f, 0.1f}; // 現在の方向別速度を目標の方向別速度に近づけるための時間 (X:前後, Y:左右)
    OriGine::Vec2f maxSmoothTime_ = {0.1f, 0.1f}; // 現在の方向別速度を目標の方向別速度に近づけるための時間 (X:前後, Y:左右)

    OriGine::Vec2f minLimitDirectionalAccel_ = {10.0f, 10.0f}; // 方向別の最低加速度 (X:前後, Y:左右)
    OriGine::Vec2f maxLimitDirectionalAccel_ = {10.0f, 10.0f}; // 方向別の最大加速度 (X:前後, Y:左右)

    // チェック系
    float groundCheckThreshold_ = 0.0f;
    float wallCheckThreshold_   = 0.0f;

    // 壁系
    float wallRunRate_                = 0.0f; // 壁走りの速度倍率
    float wallRunRampUpTime_          = AppConfig::Player::kDefaultWallRunRampUpTime; // 壁走りの速度倍率が最大になるまでの時間
    float minWallJumpOffsetX_         = 0.0f; // 壁走りのオフセットのXの最小値 (X軸は、入力によって minWallJumpOffsetX_ から wallJumpOffset_[X]のなかから決まる)
    OriGine::Vec3f wallJumpOffset_    = {0.0f, 1.0f, 0.0f};
    float wallJumpRate_               = 0.0f; // 壁ジャンプ(壁から地面に行くとき)の速度倍率
    float gravityApplyDelayOnWallRun_ = AppConfig::Player::kDefaultGravityApplyDelayOnWallRun; // 壁走り開始時に重力を適用し始めるまでの遅延時間
    float wallRunDetachSpeed_         = AppConfig::Player::kDefaultWallRunDetachSpeed; // 壁走りから離脱する時の速度

    // rail
    float railSpeedRate_           = 1.0f; // レール上の速度倍率
    float railRampUpTime_          = 0.f; // レール上の速度倍率が最大になるまでの時間
    OriGine::Vec3f railJumpOffset_ = {0.f, 1.f, 0.f};

    OriGine::EaseType jumpHoldVelocityEaseType_ = OriGine::EaseType::Linear;
    OriGine::EaseType jumpChargeRateEaseType_   = OriGine::EaseType::Linear;
    float minJumpHoldVelocity_                  = 0.0f; // 最低ジャンプのパワー
    float maxJumpHoldVelocity_                  = 0.0f; // 最大ジャンプのパワー
    float minJumpChargeRate_                    = 0.0f; // 最低落下のパワー
    float maxJumpChargeRate_                    = 0.0f; // 最大落下のパワー

    float directionInterpolateRate_ = 0.1f;

    float invincibilityTime_ = AppConfig::Player::kDefaultInvincibilityTime; // 障害物に当たったときの無敵時間 /sec

    float defaultMass_   = AppConfig::Player::kDefaultMass; // 通常時の質量
    float massOnWallRun_ = AppConfig::Player::kDefaultMassOnWallRun; // 壁走り中の質量

    float wallRunInterval_        = AppConfig::Player::kDefaultWallRunInterval; // 壁走り&ウィリーが可能になるまでのインターバル時間
    float currentWallRunInterval_ = 0.0f; // 現在の壁走りインターバル時間
    float railInterval_           = 0.f; // 再びレールに乗れるまでのインターバル(定数)
    float currentRailInterval_    = 0.f;

public:
    float GetDecelerationFactor() const { return decelerationFactor_; }
    float GetGroundCheckThreshold() const { return groundCheckThreshold_; }
    float GetWallCheckThreshold() const { return wallCheckThreshold_; }

    float GetDirectionInterpolateRate() const { return directionInterpolateRate_; }

    float GetBaseSpeed() const { return baseSpeed_; }

    OriGine::EaseType GetJumpHoldVelocityEaseType() const { return jumpHoldVelocityEaseType_; }
    OriGine::EaseType GetJumpChargeRateEaseType() const { return jumpChargeRateEaseType_; }

    float GetMinJumpHoldVelocity() const { return minJumpHoldVelocity_; }
    float GetMaxJumpHoldVelocity() const { return minJumpHoldVelocity_; }
    float GetMinJumpChargeRate() const { return minJumpChargeRate_; }
    float GetMaxJumpChargeRate() const { return maxJumpChargeRate_; }

    float GetCurrentMaxSpeed() const { return currentMaxSpeed_; }
    void SetCurrentMaxSpeed(float _currentMaxSpeed) { currentMaxSpeed_ = _currentMaxSpeed; }
    const OriGine::Vec2f& GetCurrentMaxDirectionalSpeed() const { return currentMaxDirectionalSpeed_; }
    void SetCurrentMaxDirectionalSpeed(const OriGine::Vec2f& _currentMaxDirectionalSpeed) { currentMaxDirectionalSpeed_ = _currentMaxDirectionalSpeed; }
    const OriGine::Vec2f& GetCurrentDirectionalSpeed() const { return currentDirectionalSpeed_; }
    void SetCurrentDirectionalSpeed(const OriGine::Vec2f& _currentDirectionalSpeed) { currentDirectionalSpeed_ = _currentDirectionalSpeed; }

    const OriGine::Vec2f& GetMaxSmoothTime() const { return maxSmoothTime_; }
    const OriGine::Vec2f& GetMinSmoothTime() const { return minSmoothTime_; }
    const OriGine::Vec2f& GetMaxLimitDirectionalAccel() const { return maxLimitDirectionalAccel_; }
    const OriGine::Vec2f& GetMinLimitDirectionalAccel() const { return minLimitDirectionalAccel_; }

    float GetWallRunRate() const { return wallRunRate_; }
    void SetWallRunRate(float _wallRunRate) { wallRunRate_ = _wallRunRate; }

    float GetWallRunRampUpTime() const { return wallRunRampUpTime_; }

    float GetMinWallJumpOffsetX() const { return minWallJumpOffsetX_; }
    const OriGine::Vec3f& GetWallJumpOffset() const { return wallJumpOffset_; }
    void SetWallJumpOffset(const OriGine::Vec3f& _wallJumpOffset) { wallJumpOffset_ = _wallJumpOffset; }

    float GetBaseGearupCoolTime() const { return baseGearupCoolTime_; }
    float GetGearUpCoolTime() const { return gearUpCoolTime_; }
    void SetGearUpCoolTime(float _gearUpCoolTime) { gearUpCoolTime_ = _gearUpCoolTime; }
    void minusGearUpCoolTime(float _deltaTime) { gearUpCoolTime_ -= _deltaTime; }

    float GetCoolTimeUpRateBase() const { return coolTimeAddRateBase_; }
    float GetCoolTimeUpRateCommonRate() const { return coolTimeAddRateCommonRate_; }
    float GetSpeedUpRateBase() const { return speedUpRateBase_; }
    float GetSpeedUpRateCommonRate() const { return speedUpRateCommonRate_; }

    float GetInvincibilityTime() const { return invincibilityTime_; }

    float GetDefaultMass() const { return defaultMass_; }
    float GetMassOnWallRun() const { return massOnWallRun_; }

    float GetGravityApplyDelayOnWallRun() const { return gravityApplyDelayOnWallRun_; }

    float GetWallRunDetachSpeed() const { return wallRunDetachSpeed_; }

    /// <summary>
    /// 壁走りが可能かどうか
    /// </summary>
    /// <returns></returns>
    bool CanWallRun() const {
        return currentWallRunInterval_ <= 0.0f;
    }

    /// <summary>
    /// レールに乗れるかどうか
    /// </summary>
    /// <returns></returns>
    bool CanRideRail() const {
        return currentRailInterval_ <= 0.f;
    }

    /// <summary>
    /// 壁走りインターバル時間の更新
    /// </summary>
    /// <param name="_deltaTime"></param>
    void UpdateWallRunInterval(float _deltaTime) {
        currentWallRunInterval_ -= _deltaTime;
        currentWallRunInterval_ = (std::max)(currentWallRunInterval_, 0.f);
    }
    /// <summary>
    /// 壁走りインターバル時間のリセット
    /// </summary>
    void ResetWallRunInterval() {
        currentWallRunInterval_ = 0.f;
    }

    void SetupWallRunInterval() {
        currentWallRunInterval_ = wallRunInterval_;
    }

    void UpdateRailInterval(float _deltaTime) {
        currentRailInterval_ -= _deltaTime;
        currentRailInterval_ = (std::max)(currentRailInterval_, 0.f);
    }
    void SetupRailInterval() {
        currentRailInterval_ = railInterval_;
    }
    void ResetRailInterval() {
        currentRailInterval_ = 0.f;
    }

    float GetRailSpeedRate() const { return railSpeedRate_; }
    float GetRailRampUpTime() const { return railRampUpTime_; }
    const OriGine::Vec3f& GetRailJumpOffset() const { return railJumpOffset_; }
};
