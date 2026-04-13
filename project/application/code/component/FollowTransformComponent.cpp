#include "FollowTransformComponent.h"

#ifdef DEBUG
#include "myGui/MyGui.h"
#endif // DEBUG

using namespace OriGine;

FollowTransformComponent::FollowTransformComponent() {}
FollowTransformComponent::~FollowTransformComponent() {}

void FollowTransformComponent::Initialize(OriGine::Scene* /*scene*/, OriGine::EntityHandle /*owner*/) {}

void FollowTransformComponent::Edit(OriGine::Scene* /*scene*/, OriGine::EntityHandle /*owner*/, [[maybe_unused]] const std::string& parentLabel) {
#ifdef DEBUG
    DragGuiVectorCommand("Offset##" + parentLabel, offset_, 0.01f);
    DragGuiCommand("Sensitivity##" + parentLabel, sensitivity_, 0.1f, 0.0f, 100.0f);
#endif // DEBUG
}

void FollowTransformComponent::Finalize() {}

void FollowTransformComponent::SetTarget(const OriGine::EntityHandle& targetHandle) {
    targetHandle_ = targetHandle;
    isActive_     = true;
}

void to_json(nlohmann::json& j, const FollowTransformComponent& component) {
    j = nlohmann::json{
        {"offset", component.offset_},
        {"sensitivity", component.sensitivity_},
    };
}

void from_json(const nlohmann::json& j, FollowTransformComponent& component) {
    j.at("offset").get_to(component.offset_);
    if (j.contains("sensitivity")) {
        j.at("sensitivity").get_to(component.sensitivity_);
    }
}
