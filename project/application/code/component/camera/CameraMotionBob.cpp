#include "CameraMotionBob.h"

#ifdef _DEBUG
/// engine
#include "scene/Scene.h"

/// ECS
// component
#include "component/camera/CameraShakeSourceComponent.h"
#include "component/player/State/PlayerState.h"

/// GUI
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

CameraMotionBob::CameraMotionBob() {}
CameraMotionBob::~CameraMotionBob() {}

void CameraMotionBob::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}
void CameraMotionBob::Finalize() {}

void CameraMotionBob::Edit([[maybe_unused]] OriGine::Scene* _scene, [[maybe_unused]] EntityHandle _owner, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    auto& cameraShakeSourceComponents = _scene->GetComponents<CameraShakeSourceComponent>(_owner);
    int32_t entityMaterialSize        = static_cast<int32_t>(cameraShakeSourceComponents.size());
    // カメラTransformコンポーネントインデックス
    InputGuiCommand<int32_t>("Camera Shake Source Component Index##" + _parentLabel, cameraShakeSourceComponentId, "%d", [this, entityMaterialSize](int32_t* _newT) {
        this->cameraShakeSourceComponentId = std::clamp(*_newT, -1, entityMaterialSize - 1);
    });

    // 揺れを開始するギアレベル
    InputGuiCommand<int32_t>("Threshold Gear Level##" + _parentLabel, thresholdGearLevel, "%d", [this](int32_t* _newVal) {
        thresholdGearLevel = std::clamp(*_newVal, 0, kMaxPlayerGearLevel);
    });

    // 最小振幅
    DragGuiVectorCommand("Min Amplitude##" + _parentLabel, minAmplitude, 0.01f);
    // 最大振幅
    DragGuiVectorCommand("Max Amplitude##" + _parentLabel, maxAmplitude, 0.01f);

    ImGui::Spacing();

    // 最小周波数
    DragGuiVectorCommand("Min Frequency##" + _parentLabel, minFrequency, 0.1f);
    // 最大周波数
    DragGuiVectorCommand("Max Frequency##" + _parentLabel, maxFrequency, 0.1f);

#endif // _DEBUG
}

void to_json(nlohmann::json& _j, const CameraMotionBob& _c) {
    _j = nlohmann::json{
        {"cameraShakeSourceComponentId", _c.cameraShakeSourceComponentId},
        {"thresholdGearLevel", _c.thresholdGearLevel},
        {"minAmplitude", _c.minAmplitude},
        {"maxAmplitude", _c.maxAmplitude},
        {"minFrequency", _c.minFrequency},
        {"maxFrequency", _c.maxFrequency}};
}

void from_json(const nlohmann::json& _j, CameraMotionBob& _c) {
    _j.at("cameraShakeSourceComponentId").get_to(_c.cameraShakeSourceComponentId);
    _j.at("thresholdGearLevel").get_to(_c.thresholdGearLevel);
    _j.at("minAmplitude").get_to(_c.minAmplitude);
    _j.at("maxAmplitude").get_to(_c.maxAmplitude);
    _j.at("minFrequency").get_to(_c.minFrequency);
    _j.at("maxFrequency").get_to(_c.maxFrequency);
}
