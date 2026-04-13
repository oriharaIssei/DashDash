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

void WallRunnableComponent::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}
void WallRunnableComponent::Finalize() {}

void WallRunnableComponent::Edit(Scene* /*_scene*/, EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    DragGuiCommand("Angle Tolerance##" + _parentLabel, angleTolerance, 0.1f);

    ImGui::Spacing();
    ImGui::SeparatorText("Allowed Normals");

    for (size_t i = 0; i < allowedNormals.size(); ++i) {
        std::string label = "Normal " + std::to_string(i) + "##" + _parentLabel;
        DragGuiVectorCommand(label, allowedNormals[i], 0.01f);
        ImGui::SameLine();
        if (ImGui::Button(("X##RemoveNormal" + std::to_string(i) + _parentLabel).c_str())) {
            allowedNormals.erase(allowedNormals.begin() + i);
            --i;
        }
    }
    if (ImGui::Button(("+ Add Normal##" + _parentLabel).c_str())) {
        allowedNormals.push_back(Vec3f(1.f, 0.f, 0.f));
    }
#endif // _DEBUG
}

bool WallRunnableComponent::IsNormalAllowed(const Vec3f& _collisionNormal) const {
    // 空の場合は全方向許可
    if (allowedNormals.empty()) {
        return true;
    }

    float cosThreshold = std::cos(angleTolerance * OriGine::kDeg2Rad);

    for (const auto& allowed : allowedNormals) {
        float dot = _collisionNormal.dot(allowed.normalize());
        if (dot >= cosThreshold) {
            return true;
        }
    }

    return false;
}

void to_json(nlohmann::json& _j, const WallRunnableComponent& _c) {
    _j = nlohmann::json{
        {"angleTolerance", _c.angleTolerance},
        {"allowedNormals", _c.allowedNormals},
    };
}

void from_json(const nlohmann::json& _j, WallRunnableComponent& _c) {
    _j.at("angleTolerance").get_to(_c.angleTolerance);
    _j.at("allowedNormals").get_to(_c.allowedNormals);
}
