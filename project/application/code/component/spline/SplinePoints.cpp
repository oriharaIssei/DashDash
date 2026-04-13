#include "SplinePoints.h"

#include "ECS/entity/Entity.h"
#include "scene/Scene.h"

#ifdef _DEBUG
#include "myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

SplinePoints::SplinePoints() {}
SplinePoints::~SplinePoints() {}

void SplinePoints::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {}
void SplinePoints::Finalize() {}

void SplinePoints::Edit(Scene* /*_scene*/, EntityHandle /*_OriGine::Entity*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    CheckBoxCommand("Is cross mesh##" + _parentLabel, commonSettings.isCrossMesh);

    ImGui::Spacing();

    CheckBoxCommand("Is uv loop enable ##" + _parentLabel, commonSettings.isUvLoopEnable);
    if (commonSettings.isUvLoopEnable) {
        DragGuiCommand("Uv loop length ##" + _parentLabel, commonSettings.uvLoopLength, 0.01f);
        if (commonSettings.uvLoopLength < 0.01f) {
            commonSettings.uvLoopLength = 0.01f;
        }
    }

    ImGui::Spacing();

    InputGuiCommand("Capacity##" + _parentLabel, capacity);
    if (capacity < 4) {
        capacity = 4;
    }

    ImGui::Spacing();

    InputGuiCommand("SegmentDivide##" + _parentLabel, segmentDivide);
    if (segmentDivide < 1) {
        segmentDivide = 1;
    }
    DragGuiCommand("SegmentLength##" + _parentLabel, commonSettings.segmentLength, 0.01f, 0.01f);
    if (commonSettings.segmentLength < 0.0f) {
        commonSettings.segmentLength = 0.0f;
    }

    ImGui::Spacing();

    EasingComboGui("WidthEaseType##" + _parentLabel, commonSettings.widthEaseType);

    DragGuiCommand("StartWidth##" + _parentLabel, commonSettings.startWidth, 0.01f);
    if (commonSettings.startWidth < 0.0f) {
        commonSettings.startWidth = 0.0f;
    }
    DragGuiCommand("EndWidth##" + _parentLabel, commonSettings.endWidth, 0.01f);
    if (commonSettings.endWidth < 0.0f) {
        commonSettings.endWidth = 0.0f;
    }

    ImGui::Spacing();

    EasingComboGui("UV EaseType##" + _parentLabel, commonSettings.uvEaseType);
    DragGuiVectorCommand("StartUv##" + _parentLabel, commonSettings.startUv);
    DragGuiVectorCommand("EndUv##" + _parentLabel, commonSettings.endUv);

    ImGui::Spacing();

    DragGuiCommand("FadeOutTime##" + _parentLabel, commonSettings.fadeoutTime, 0.01f);
#endif // _DEBUG
}

void SplinePoints::PushPoint(const OriGine::Vec3f& _pos) {
    if (points.size() >= static_cast<size_t>(capacity)) {
        points.pop_front();
    }
    points.push_back(_pos);
}

void to_json(nlohmann::json& j, const SplinePoints& c) {
    j["capacity"]       = c.capacity;
    j["segmentDivide"]  = c.segmentDivide;
    j["startUv"]        = c.commonSettings.startUv;
    j["endUv"]          = c.commonSettings.endUv;
    j["segmentLength"]  = c.commonSettings.segmentLength;
    j["startWidth"]     = c.commonSettings.startWidth;
    j["endWidth"]       = c.commonSettings.endWidth;
    j["fadeoutTime"]    = c.commonSettings.fadeoutTime;
    j["isCrossMesh"]    = c.commonSettings.isCrossMesh;
    j["isUvLoopEnable"] = c.commonSettings.isUvLoopEnable;
    j["uvLoopLength"]   = c.commonSettings.uvLoopLength;
    j["widthEaseType"]  = static_cast<int>(c.commonSettings.widthEaseType);
    j["uvEaseType"]     = static_cast<int>(c.commonSettings.uvEaseType);

    j["upVector"] = c.commonSettings.upVector;
}

void from_json(const nlohmann::json& j, SplinePoints& c) {
    j.at("capacity").get_to(c.capacity);
    j.at("segmentDivide").get_to(c.segmentDivide);
    j.at("startUv").get_to(c.commonSettings.startUv);
    j.at("endUv").get_to(c.commonSettings.endUv);
    j.at("segmentLength").get_to(c.commonSettings.segmentLength);
    j.at("startWidth").get_to(c.commonSettings.startWidth);
    j.at("endWidth").get_to(c.commonSettings.endWidth);
    j.at("fadeoutTime").get_to(c.commonSettings.fadeoutTime);

    j.at("isCrossMesh").get_to(c.commonSettings.isCrossMesh);
    {
        int temp = 0;
        j.at("widthEaseType").get_to(temp);
        c.commonSettings.widthEaseType = static_cast<EaseType>(temp);
    }
    {
        int temp = 0;
        j.at("uvEaseType").get_to(temp);
        c.commonSettings.uvEaseType = static_cast<EaseType>(temp);
    }

    j.at("isUvLoopEnable").get_to(c.commonSettings.isUvLoopEnable);
    j.at("uvLoopLength").get_to(c.commonSettings.uvLoopLength);

    if (j.contains("upVector")) {
        j.at("upVector").get_to(c.commonSettings.upVector);
    }
}
