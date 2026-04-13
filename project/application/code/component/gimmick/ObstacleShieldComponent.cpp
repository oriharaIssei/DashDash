#include "ObstacleShieldComponent.h"

using namespace OriGine;

ObstacleShieldComponent::ObstacleShieldComponent() {}
ObstacleShieldComponent::~ObstacleShieldComponent() {}

void ObstacleShieldComponent::Initialize(OriGine::Scene* /*scene*/, OriGine::EntityHandle /*owner*/) {}

void ObstacleShieldComponent::Edit(OriGine::Scene* /*scene*/, OriGine::EntityHandle /*owner*/, [[maybe_unused]] const std::string& parentLabel) {}

void ObstacleShieldComponent::Finalize() {}

void to_json(nlohmann::json& j, const ObstacleShieldComponent& /*component*/) {
    j = nlohmann::json::object();
}

void from_json(const nlohmann::json& /*j*/, ObstacleShieldComponent& /*component*/) {}
