#include "AddForceComponent.h"

#ifdef _DEBUG
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

AddForceComponent::AddForceComponent() {}
AddForceComponent::~AddForceComponent() {}

void AddForceComponent::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}
void AddForceComponent::Finalize() {}

void AddForceComponent::Edit(Scene* /*_scene*/, EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    // Trigger Mode
    {
        const char* triggerModeItems[] = {"OnEnter", "WhileColliding"};
        int triggerModeInt             = static_cast<int>(triggerMode_);
        if (ImGui::Combo(("Trigger Mode##" + _parentLabel).c_str(), &triggerModeInt, triggerModeItems, 2)) {
            triggerMode_ = static_cast<TriggerMode>(triggerModeInt);
        }
    }
    ImGui::Spacing();
    DragGuiVectorCommand("Add Force##" + _parentLabel, addForce_, 0.01f);
#endif // _DEBUG
}

void to_json(nlohmann::json& j, const AddForceComponent& c) {
    j = nlohmann::json{
        {"triggerMode", static_cast<int>(c.triggerMode_)},
        {"addForce", c.addForce_},
    };
}

void from_json(const nlohmann::json& j, AddForceComponent& c) {
    c.triggerMode_ = static_cast<AddForceComponent::TriggerMode>(j.value("triggerMode", 0));
    j.at("addForce").get_to(c.addForce_);
}
