#include "PlayerState.h"

#include "PlayerIdleState.h"

/// externals
#include "myGui/MyGui.h"

#ifdef _DEBUG
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

PlayerState::PlayerState()
    : IComponent() {}

PlayerState::~PlayerState() {}

void PlayerState::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {
    // moveStateEnum の 初期化
    // current,prev を IDLE に設定
    moveStateEnum_.Set(PlayerMoveState::IDLE);
    moveState_ = nullptr;

    // stateFlag の 初期化
    stateFlag_.Set(EnumBitmask<PlayerStateFlag>(0));
    stateFlag_.Set(EnumBitmask<PlayerStateFlag>(0));

    wallCollisionNormal_ = {0.f, 0.f, 0.f};
    gearLevel_           = kDefaultPlayerGearLevel;
    gearUpCoolTime_      = 0.0f;
}

void PlayerState::Edit([[maybe_unused]] Scene* _scene, [[maybe_unused]] EntityHandle _owner, [[maybe_unused]] const std::string& _parentLabel) {
    ImGui::Text("MoveState     : %s", moveStateName[moveStateEnum_.Current().ToEnum()]);
    ImGui::Text("Gear Level    : %d", gearLevel_);
    ImGui::Spacing();

    ImGui::Text("Is On Ground  : %s", stateFlag_.Current() | PlayerStateFlag::ON_GROUND ? "true" : "false");
    ImGui::Text("Is Goal       : %s", stateFlag_.Current() | PlayerStateFlag::IS_GOAL ? "true" : "false");

    ImGui::Spacing();

    ImGui::Text("Is Gear Up        : %s", stateFlag_.Current() | PlayerStateFlag::GEAR_UP ? "true" : "false");
    ImGui::Text("Gear Up Cool Time : %f", gearUpCoolTime_);

    ImGui::Spacing();
    ImGui::Text("Wall Collision    : %s", stateFlag_.Current() | PlayerStateFlag::ON_WALL ? "true" : "false");
}

void PlayerState::Finalize() {}

void PlayerState::OnCollisionWall(const Vec3f& _collisionNormal, EntityHandle _wallEntityHandle) {
    stateFlag_.CurrentRef().SetFlag(PlayerStateFlag::ON_WALL);
    wallCollisionNormal_ = _collisionNormal;
    wallEntityHandle_    = _wallEntityHandle;
}

void PlayerState::OffCollisionWall() {
    stateFlag_.CurrentRef().ClearFlag(PlayerStateFlag::ON_WALL);

    wallCollisionNormal_ = {0.f, 0.f, 0.f};
}

void PlayerState::OnCollisionRail(OriGine::EntityHandle _entityHandle) {
    stateFlag_.CurrentRef().SetFlag(PlayerStateFlag::ON_RAIL);
    railEntityHandle_ = _entityHandle;
}

void PlayerState::OffCollisionRail() {
    stateFlag_.CurrentRef().ClearFlag(PlayerStateFlag::ON_RAIL);
}

void PlayerState::OnCollisionShield(OriGine::EntityHandle shieldEntityHandle) {
    stateFlag_.CurrentRef().SetFlag(PlayerStateFlag::HAS_SHIELD);
    shieldEntityHandle_ = shieldEntityHandle;
}

void PlayerState::ClearHasShieldFlag() {
    stateFlag_.CurrentRef().ClearFlag(PlayerStateFlag::HAS_SHIELD);
}

void PlayerState::OnCollisionGround() {
    stateFlag_.CurrentRef().SetFlag(PlayerStateFlag::ON_GROUND);
}
void PlayerState::OffCollisionGround() {
    stateFlag_.CurrentRef().ClearFlag(PlayerStateFlag::ON_GROUND);
    stateFlag_.CurrentRef().ClearFlag(PlayerStateFlag::JUST_LANDED);
}

void PlayerState::OnCollisionObstacle(float _penaltyTime, float _invincibility) {
    if (invincibility_ > 0.f) {
        return;
    }

    stateFlag_.CurrentRef().SetFlag(PlayerStateFlag::IS_PENALTY);
    penaltyTime_ = _penaltyTime;

    invincibility_ = _invincibility;
}

float PlayerState::SufferPenalty() {
    stateFlag_.CurrentRef().ClearFlag(PlayerStateFlag::IS_PENALTY);

    float penalty = penaltyTime_;
    penaltyTime_  = 0.0f;
    return penalty;
}

void PlayerState::SetPlayerMoveState(std::shared_ptr<IPlayerMoveState> _playerMoveState) {
    moveState_ = _playerMoveState;
}

void to_json(nlohmann::json& /*_j*/, const PlayerState& /*_p*/) {}
void from_json(const nlohmann::json& /*_j*/, PlayerState& /*_p*/) {}
