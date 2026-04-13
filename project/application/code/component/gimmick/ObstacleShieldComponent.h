#pragma once

#include "component/IComponent.h"

class ObstacleShieldComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const ObstacleShieldComponent& component);
    friend void from_json(const nlohmann::json& j, ObstacleShieldComponent& component);

public:
    ObstacleShieldComponent();
    ~ObstacleShieldComponent();

    void Initialize(OriGine::Scene* scene, OriGine::EntityHandle owner) override;
    void Edit(OriGine::Scene* scene, OriGine::EntityHandle owner, const std::string& parentLabel) override;
    void Finalize() override;
};
