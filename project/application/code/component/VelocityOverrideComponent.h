#pragma once

#include "component/IComponent.h"

/// stl
#include <array>

/// math
#include "math/Vector3.h"

/// <summary>
/// 速度を強制的に上書きするコンポーネント
/// </summary>
struct VelocityOverrideComponent
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
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;
    /// <summary>
    /// エディタ用編集UI
    /// </summary>
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

public:
    // 強制移動ベクトル
    OriGine::Vec3f forcedMovementVector_ = OriGine::Vec3f();

    std::array<bool, OriGine::Vec3f::dim> isOverrideAxis_ = {false, false, false};

    // 硬直時間
    float stunTime_ = 0.f;
};
