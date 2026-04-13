#include "StageData.h"

#ifdef DEBUG
#include "myGui/MyGui.h"
#endif // DEBUG

using namespace OriGine;

void StageData::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}

void StageData::Finalize() {}

void StageData::Edit(Scene* /*_scene*/, EntityHandle /*_OriGine::Entity*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef DEBUG
    InputGuiCommand("StageNumber##" + _parentLabel, stageNumber_, "%d");
    InputGuiCommand("Difficulty##" + _parentLabel, difficulty_, "%d");
#endif // DEBUG
}

void to_json(nlohmann::json& j, const StageData& component) {
    j = nlohmann::json{
        {"stageNumber", component.stageNumber_},
        {"difficulty", component.difficulty_}};
}

void from_json(const nlohmann::json& j, StageData& component) {
    j.at("stageNumber").get_to(component.stageNumber_);
    j.at("difficulty").get_to(component.difficulty_);
}
