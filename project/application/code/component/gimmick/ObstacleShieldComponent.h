#pragma once

#include "component/IComponent.h"

/// <summary>
/// プレイヤーが取得することでシールド（障害物1回分の無効化）を得られる
/// アイテム Entity に付与するマーカーコンポーネント。データは持たない。
/// </summary>
class ObstacleShieldComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const ObstacleShieldComponent& component);
    friend void from_json(const nlohmann::json& j, ObstacleShieldComponent& component);

public:
    ObstacleShieldComponent();
    ~ObstacleShieldComponent();

    void Initialize(OriGine::Scene* scene, const OriGine::EntityHandle& owner) override;
    void Edit(OriGine::Scene* scene, const OriGine::EntityHandle& owner, const std::string& parentLabel) override;
    void Finalize() override;
};
