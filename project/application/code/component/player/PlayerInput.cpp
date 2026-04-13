#include "PlayerInput.h"

/// externals
#ifdef _DEBUG
#include <myGui/MyGui.h>
#endif // _DEBUG

/// math
#include "math/MathEnv.h"
#include "math/Matrix4x4.h"

using namespace OriGine;

void PlayerInput::Initialize(Scene* /*_scene*/, EntityHandle /* _handle */) {}

void PlayerInput::Edit(Scene* /*_scene*/, EntityHandle /* _handle */, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    DragGuiCommand("MaxJumpInputTime##" + _parentLabel, maxJumpTime_, 0.01f, 0.01f);
#endif // _DEBUG
}

void PlayerInput::Finalize() {
    // 何もしない
}

Vec3f PlayerInput::CalculateWorldInputDirection(const Quaternion& _cameraRotation) {
    if (inputDirection_.lengthSq() <= kEpsilon) {
        worldInputDirection_ = Vec3f(0.f, 0.f, 0.f);
        return worldInputDirection_;
    }

    // カメラのYaw軸回転のみ抽出
    float cameraYaw = _cameraRotation.ToYaw();

    // 入力を3Dへ (Zが前)
    Vec3f inputDir3D = {inputDirection_[X], 0.0f, inputDirection_[Y]};
    inputDir3D       = inputDir3D.normalize();

    // カメラのY軸回転を適用
    worldInputDirection_ = inputDir3D * MakeMatrix4x4::RotateY(cameraYaw);
    return worldInputDirection_;
}

void to_json(nlohmann::json& _j, const PlayerInput& _input) {
    _j["MaxJumpInputTime"] = _input.maxJumpTime_;
}

void from_json(const nlohmann::json& _j, PlayerInput& _input) {
    _input.maxJumpTime_ = _j["MaxJumpInputTime"].get<float>();
}
