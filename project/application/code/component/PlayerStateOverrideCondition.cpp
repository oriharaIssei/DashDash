#include "PlayerStateOverrideCondition.h"

#ifdef _DEBUG
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

PlayerStateOverrideCondition::PlayerStateOverrideCondition() {}
PlayerStateOverrideCondition::~PlayerStateOverrideCondition() {}

void PlayerStateOverrideCondition::Initialize(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/) {}
void PlayerStateOverrideCondition::Finalize() {}

void PlayerStateOverrideCondition::Edit(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/, [[maybe_unused]] const ::std::string& _parentLabel) {
#ifdef _DEBUG
    int32_t bitmask = static_cast<int32_t>(overrideConditions.ToUnderlying());

    for (int32_t i = 0; i < static_cast<int32_t>(PlayerMoveState::Count); ++i) {
        std::string label = moveStateName[static_cast<PlayerMoveState>(1 << i)];
        label += "##" + _parentLabel;
        bool flag = (bitmask & (1 << i)) != 0;
        if (ImGui::Checkbox(label.c_str(), &flag)) {
            if (flag) {
                bitmask |= (1 << i);
            } else {
                bitmask &= ~(1 << i);
            }
        }
    }

    if (overrideConditions != bitmask) {
        auto command = std::make_unique<SetterCommand<EnumBitmask<PlayerMoveState>>>(
            &overrideConditions, EnumBitmask<PlayerMoveState>(bitmask));
        EditorController::GetInstance()->PushCommand(std::move(command));
    }

#endif // _DEBUG
}

void to_json(nlohmann::json& j, const PlayerStateOverrideCondition& c) {
    j = nlohmann::json{
        {"overrideConditions", c.overrideConditions.ToUnderlying()},
    };
}

void from_json(const nlohmann::json& j, PlayerStateOverrideCondition& c) {
    int32_t stateInt;
    j.at("overrideConditions").get_to(stateInt);
    c.overrideConditions = EnumBitmask<PlayerMoveState>(stateInt);
}
