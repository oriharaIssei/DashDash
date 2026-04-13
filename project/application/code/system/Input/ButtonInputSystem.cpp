#include "ButtonInputSystem.h"

/// Engine
#define ENGINE_ECS
#include "EngineInclude.h"

#include "scene/Scene.h"
// input
#include "input/GamepadInput.h"
#include "input/KeyboardInput.h"
#include "input/MouseInput.h"
// component
#include "component/ui/Button.h"

using namespace OriGine;

void ButtonInputSystem::Initialize() {}
void ButtonInputSystem::Finalize() {}

void ButtonInputSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    OriGine::KeyboardInput* keyInput = GetScene()->GetKeyboardInput();
    MouseInput* mouseInput           = GetScene()->GetMouseInput();
    OriGine::GamepadInput* padInput  = GetScene()->GetGamepadInput();

    std::vector<Button>& buttons = GetComponents<Button>(_handle);

    if (buttons.empty()) {
        return;
    }

    for (auto& button : buttons) {

        // リセット
        button.SetHovered(false);
        button.SetPressed(false);
        button.SetReleased(false);

        /// ====================== check Keyboard Input ====================== ///
        //! Shortcut で キャンセルの方法が無い
        for (auto key : button.GetShortcutKey()) {
            if (keyInput->IsPress(key)) {
                button.SetPressed(true);
            }
            if (keyInput->IsRelease(key)) {
                button.SetReleased(true);
            }
        }

        /// ====================== check Pad Input ====================== ///
        for (auto padButton : button.GetShortcutGamepadButton()) {
            if (padInput->IsPress(padButton)) {
                button.SetPressed(true);
            }
            if (padInput->IsRelease(padButton)) {
                button.SetReleased(true);
            }
        }

        /// ====================== check Mouse Input ====================== ///
        SpriteRenderer* buttonSprite = GetComponent<SpriteRenderer>(_handle);
        if (buttonSprite != nullptr) {
            const OriGine::Vec2f& mousePos   = mouseInput->GetPosition();
            const OriGine::Vec2f& buttonPos  = buttonSprite->GetTranslate();
            const OriGine::Vec2f& buttonSize = buttonSprite->GetSpriteBuff()->scale_;
            const OriGine::Vec2f& anchor     = buttonSprite->GetAnchorPoint();

            OriGine::Vec2f buttonLeftTop     = buttonPos - buttonSize * anchor;
            OriGine::Vec2f buttonRightBottom = buttonPos + buttonSize * (OriGine::Vec2f(1.0f, 1.0f) - anchor);

            // マウスがボタンの範囲内にあるかどうかを判定
            bool isMouseOverButton = (mousePos[X] >= buttonLeftTop[X] && mousePos[X] <= buttonRightBottom[X] && mousePos[Y] >= buttonLeftTop[Y] && mousePos[Y] <= buttonRightBottom[Y]);

            button.SetHovered(isMouseOverButton);

            // ボタンがホバーされている場合、クリック状態を更新
            if (!button.isHovered()) {
                return;
            }
            // クリック状態を更新
            if (button.isPressed()) {
                button.SetReleased(mouseInput->IsRelease(MouseButton::LEFT));
            } else {
                button.SetPressed(mouseInput->IsRelease(MouseButton::LEFT));
            }
        }
    }
}
