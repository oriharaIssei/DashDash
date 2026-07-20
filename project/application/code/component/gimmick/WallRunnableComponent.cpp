#include "WallRunnableComponent.h"

/// stl
#include <cmath>

/// math
#include "math/MathEnv.h"

#ifdef _DEBUG
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

WallRunnableComponent::WallRunnableComponent() {}
WallRunnableComponent::~WallRunnableComponent() {}

void WallRunnableComponent::Initialize(Scene* /*_scene*/, const EntityHandle& /*_owner*/) {}
void WallRunnableComponent::Finalize() {}

void WallRunnableComponent::Edit(Scene* /*_scene*/, const EntityHandle& /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    DragGuiCommand("Angle Tolerance##" + _parentLabel, angleTolerance_, 0.1f);

    ImGui::Spacing();
    ImGui::SeparatorText("Allowed Normals");

    for (size_t i = 0; i < allowedNormals_.size(); ++i) {
        std::string label = "Normal " + std::to_string(i) + "##" + _parentLabel;
        DragGuiVectorCommand(label, allowedNormals_[i], 0.01f);
        ImGui::SameLine();
        if (ImGui::Button(("X##RemoveNormal" + std::to_string(i) + _parentLabel).c_str())) {
            allowedNormals_.erase(allowedNormals_.begin() + i);
            --i;
        }
    }
    if (ImGui::Button(("+ Add Normal##" + _parentLabel).c_str())) {
        allowedNormals_.push_back(Vec3f(1.f, 0.f, 0.f));
    }
#endif // _DEBUG
}

bool WallRunnableComponent::IsNormalAllowed(const Vec3f& _collisionNormal) const {
    // 空の場合は全方向許可
    if (allowedNormals_.empty()) {
        return true;
    }

    // 許容角度をコサイン値に変換し、内積との比較で角度判定する（acosを避けるため）
    float cosThreshold = std::cos(angleTolerance_ * OriGine::kDeg2Rad);

    for (const auto& allowed : allowedNormals_) {
        float dot = _collisionNormal.dot(allowed.normalize());
        if (dot >= cosThreshold) {
            return true;
        }
    }

    return false;
}

void to_json(nlohmann::json& _j, const WallRunnableComponent& _c) {
    _j = nlohmann::json{
        {"angleTolerance", _c.angleTolerance_},
        {"allowedNormals", _c.allowedNormals_},
    };
}

void from_json(const nlohmann::json& _j, WallRunnableComponent& _c) {
    _j.at("angleTolerance").get_to(_c.angleTolerance_);
    _j.at("allowedNormals").get_to(_c.allowedNormals_);
}
