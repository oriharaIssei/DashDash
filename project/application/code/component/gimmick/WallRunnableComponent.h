#pragma once

#include "component/IComponent.h"

/// stl
#include <vector>

/// math
#include "math/Vector3.h"

/// <summary>
/// 壁走り可能オブジェクトに付与するコンポーネント
/// </summary>
struct WallRunnableComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const WallRunnableComponent& _c);
    friend void from_json(const nlohmann::json& _j, WallRunnableComponent& _c);

public:
    WallRunnableComponent();
    ~WallRunnableComponent() override;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

    /// <summary>
    /// 衝突法線が壁走り許可方向かどうかを判定する
    /// </summary>
    /// <param name="_collisionNormal">衝突面の法線 (正規化済み)</param>
    /// <returns>壁走り可能か</returns>
    bool IsNormalAllowed(const OriGine::Vec3f& _collisionNormal) const;

public:
    /// 壁走りを許可する面の法線方向 (空の場合は全方向許可)
    std::vector<OriGine::Vec3f> allowedNormals;

    /// 許容角度 (degrees)
    float angleTolerance = 180.0f;
};
