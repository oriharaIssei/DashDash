#pragma once

#include "component/IComponent.h"

/// stl
#include <array>

/// math
#include "math/Vector3.h"

/// <summary>
/// 速度を強制的に上書きするコンポーネント
/// </summary>
class VelocityOverrideComponent
    : public OriGine::IComponent {
    /// <summary>
    /// JSON 変換用
    /// </summary>
    friend void to_json(nlohmann::json& j, const VelocityOverrideComponent& c);
    /// <summary>
    /// JSON 復元用
    /// </summary>
    friend void from_json(const nlohmann::json& j, VelocityOverrideComponent& c);

public:
    VelocityOverrideComponent();
    ~VelocityOverrideComponent() override;
    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;
    /// <summary>
    /// エディタ用編集UI
    /// </summary>
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;

public:
    const OriGine::Vec3f& GetForcedMovementVector() const { return forcedMovementVector_; }
    void SetForcedMovementVector(const OriGine::Vec3f& _forcedMovementVector) { forcedMovementVector_ = _forcedMovementVector; }

    const std::array<bool, OriGine::Vec3f::dim>& GetIsOverrideAxis() const { return isOverrideAxis_; }
    void SetIsOverrideAxis(const std::array<bool, OriGine::Vec3f::dim>& _isOverrideAxis) { isOverrideAxis_ = _isOverrideAxis; }

    float GetStunTime() const { return stunTime_; }
    void SetStunTime(float _stunTime) { stunTime_ = _stunTime; }

private:
    // 強制移動ベクトル
    OriGine::Vec3f forcedMovementVector_ = OriGine::Vec3f();

    std::array<bool, OriGine::Vec3f::dim> isOverrideAxis_ = {false, false, false};

    // 硬直時間
    float stunTime_ = 0.f;
};
