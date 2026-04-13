#include "Obstacle.h"

#ifdef _DEBUG
#include "myGui/MyGui.h"
#endif // _DEBUG

void Obstacle::Initialize(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/) {}
void Obstacle::Finalize() {}

void Obstacle::Edit(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    DragGuiCommand("PenaltyTime##" + _parentLabel, penaltyTime_, 0.1f);
    DragGuiCommand("InvincibilityTimeOnCollision##" + _parentLabel, invincibilityTimeOnCollision_, 0.1f);
#endif // _DEBUG
}

void to_json(nlohmann::json& _j, const Obstacle& _c) {
    _j["penaltyTime"]                  = _c.penaltyTime_;
    _j["invincibilityTimeOnCollision"] = _c.invincibilityTimeOnCollision_;
}
void from_json(const nlohmann::json& _j, Obstacle& _c) {
    _j.at("penaltyTime").get_to(_c.penaltyTime_);
    _j.at("invincibilityTimeOnCollision").get_to(_c.invincibilityTimeOnCollision_);
}
