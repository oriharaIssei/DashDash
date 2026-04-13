#pragma once
/// parent
#include "component/IComponent.h"

/// stl
#include <vector>

/// engine
#include "input/GamepadInput.h"
#include "input/KeyboardInput.h"

// component
#include "component/renderer/Sprite.h"

/// math
#include <stdint.h>
#include <Vector4.h>

/// config
#include "component/ui/UIConfig.h"

/// <summary>
/// Sprite を Button として扱うためのクラス
/// </summary>
class Button
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const Button& r);
    friend void from_json(const nlohmann::json& j, Button& r);

public:
    Button();
    ~Button();

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;

    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

    void Finalize() override;

private:
    bool isHovered_  = false;
    bool isPressed_  = false;
    bool IsReleased_ = false;

    std::vector<OriGine::GamepadButton> shortcutGamepadButton_;
    std::vector<OriGine::Key> shortcutKey_;

    OriGine::Vec4f normalColor_  = AppConfig::UI::kButtonNormalColor;
    OriGine::Vec4f hoverColor_   = AppConfig::UI::kButtonHoverColor;
    OriGine::Vec4f pressColor_   = AppConfig::UI::kButtonPressColor;
    OriGine::Vec4f releaseColor_ = AppConfig::UI::kButtonReleaseColor;

public: // Getter Setter
    const std::vector<OriGine::GamepadButton>& GetShortcutGamepadButton() const {
        return shortcutGamepadButton_;
    }
    const std::vector<OriGine::Key>& GetShortcutKey() const {
        return shortcutKey_;
    }

    bool isHovered() const {
        return isHovered_;
    }
    void SetHovered(bool _hovered) {
        isHovered_ = _hovered;
    }

    bool isPressed() const {
        return isPressed_;
    }
    void SetPressed(bool _pressed) {
        isPressed_ = _pressed;
    }

    bool IsReleased() const {
        return IsReleased_;
    }
    void SetReleased(bool _released) {
        IsReleased_ = _released;
    }

    const OriGine::Vec4f& GetNormalColor() const {
        return normalColor_;
    }
    void SetNormalColor(const OriGine::Vec4f& _color) {
        normalColor_ = _color;
    }

    const OriGine::Vec4f& GetHoverColor() const {
        return hoverColor_;
    }
    void SetHoverColor(const OriGine::Vec4f& _color) {
        hoverColor_ = _color;
    }

    const OriGine::Vec4f& GetPressColor() const {
        return pressColor_;
    }
    void SetPressColor(const OriGine::Vec4f& _color) {
        pressColor_ = _color;
    }

    const OriGine::Vec4f& GetReleaseColor() const {
        return releaseColor_;
    }
    void SetReleaseColor(const OriGine::Vec4f& _color) {
        releaseColor_ = _color;
    }
};
