#include "MouseCondition.h"

/// stl
#include <string>

#ifdef _DEBUG
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

MouseCondition::MouseCondition() {}
MouseCondition::~MouseCondition() {}

void MouseCondition::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}

void MouseCondition::Edit([[maybe_unused]] OriGine::Scene* _scene, [[maybe_unused]] EntityHandle _owner, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    std::string label = "IsActive##" + _parentLabel;
    CheckBoxCommand(label, isActive_);

    label = "IsShowCursor##" + _parentLabel;
    CheckBoxCommand(label, isShowCursor_);

    label = "IsFixCursor##" + _parentLabel;
    CheckBoxCommand(label, isFixCursor_);
    if (isFixCursor_) {
        label = "FixCursorPos##" + _parentLabel;
        DragGuiVectorCommand(label, fixCursorPos_, 0.1f);
    }

#endif // _DEBUG
}

void MouseCondition::Finalize() {}

void to_json(nlohmann::json& j, const MouseCondition& c) {
    j = nlohmann::json{
        {"isActive", c.isActive_},
        {"isShowCursor", c.isShowCursor_},
        {"isFixCursor", c.isFixCursor_},
        {"fixCursorPos", c.fixCursorPos_},
    };
}

void from_json(const nlohmann::json& j, MouseCondition& c) {
    j.at("isActive").get_to(c.isActive_);
    j.at("isShowCursor").get_to(c.isShowCursor_);
    j.at("isFixCursor").get_to(c.isFixCursor_);
    j.at("fixCursorPos").get_to(c.fixCursorPos_);
}
