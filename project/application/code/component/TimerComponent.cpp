#include "TimerComponent.h"

#ifdef DEBUG
#include "myGui/MyGui.h"
#endif // DEBUG

using namespace OriGine;

TimerComponent::TimerComponent() {}
TimerComponent::~TimerComponent() {}

void TimerComponent::Initialize(Scene* /*_scene*/, EntityHandle /*_owner*/) {
    if (isStarted_) {
        currentTime_ = maxTime_;
    } else {
        currentTime_ = 0.f;
    }
}
void TimerComponent::Finalize() {}

void TimerComponent::Edit(Scene* /*_scene*/, EntityHandle /* _handle */, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef DEBUG
    CheckBoxCommand("IsStarted For Using System##" + _parentLabel, isStarted_);
    DragGuiCommand("Max Time (s)##" + _parentLabel, maxTime_, 0.1f);
#endif // DEBUG
}

void to_json(nlohmann::json& j, const TimerComponent& c) {
    j = nlohmann::json{
        {"isStarted", c.isStarted_},
        {"maxTime", c.maxTime_}};
}

void from_json(const nlohmann::json& j, TimerComponent& c) {
    j.at("maxTime").get_to(c.maxTime_);
    j.at("isStarted").get_to(c.isStarted_);
}
