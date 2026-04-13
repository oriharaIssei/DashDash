#pragma once
#include "component/IComponent.h"

/// math
#include "math/Vector2.h"

/// <summary>
/// マウスの状態を指定するコンポーネント
/// </summary>
class MouseCondition
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const MouseCondition& c);
    friend void from_json(const nlohmann::json& j, MouseCondition& c);

public:
    MouseCondition();
    ~MouseCondition() override;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

private:
    bool isActive_      = true;
    bool isShowCursor_  = true;
    bool isFixCursor_   = false;
    OriGine::Vec2f fixCursorPos_ = OriGine::Vec2f(0.0f, 0.0f);

public:
    bool IsActive() const { return isActive_; }
    bool IsShowCursor() const { return isShowCursor_; }
    bool IsFixCursor() const { return isFixCursor_; }
    const OriGine::Vec2f& GetFixCursorPos() const { return fixCursorPos_; }
};
