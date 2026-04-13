#include "PlayerSpeedFor3dUI.h"

/// ECS
// component
#include "component/material/Material.h"
#include "component/physics/Rigidbody.h"
#include "component/ui/SpeedFor3dUIComponent.h"

/// math
#include "math/MathEnv.h"

using namespace OriGine;

PlayerSpeedFor3dUI::PlayerSpeedFor3dUI() : ISystem(SystemCategory::Effect) {}
PlayerSpeedFor3dUI::~PlayerSpeedFor3dUI() {}

void PlayerSpeedFor3dUI::Initialize() {}
void PlayerSpeedFor3dUI::Finalize() {}

void PlayerSpeedFor3dUI::UpdateEntity(EntityHandle _handle) {
    auto playerEnt = GetUniqueEntity("Player");
    if (!playerEnt.IsValid()) {
        return;
    }

    // component取得
    auto rigidbodyComp = GetComponent<Rigidbody>(playerEnt);
    auto speedUIComp   = GetComponent<SpeedFor3dUIComponent>(_handle);
    if (!speedUIComp || !rigidbodyComp) {
        return;
    }
    auto ui3dEntityHandle = speedUIComp->GetPlaneEntityHandle();

    // 速度取得
    float speed = Vec2f(rigidbodyComp->GetVelocity(X),
        rigidbodyComp->GetVelocity(Z))
                      .length();
    // 各桁の数字を抽出
    std::vector<int> digits = CalculateDigitsFromFloat(
        speed,
        speedUIComp->digitInteger,
        speedUIComp->digitDecimal);

    // uv を設定
    for (int32_t i = 0; i < speedUIComp->digitInteger + speedUIComp->digitDecimal; ++i) {
        auto material = GetComponent<Material>(ui3dEntityHandle, i);
        if (!material) {
            continue; // Materialがない場合は何もしない
        }

        if (int32_t(digits.size()) <= i) {
            return;
        }

        material->uvTransform_.translate_ = Vec2f(0.1f * digits[i], 0.f);
    }
}
