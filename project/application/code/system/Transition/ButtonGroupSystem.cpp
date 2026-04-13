#include "ButtonGroupSystem.h"

/// engine
#include "scene/Scene.h"
// input
#include "input/GamepadInput.h"
#include "input/KeyboardInput.h"

/// ECS
// component
#include "component/renderer/Sprite.h"
#include "component/ui/Button.h"
#include "component/ui/ButtonGroup.h"

using namespace OriGine;

ButtonGroupSystem::ButtonGroupSystem() : ISystem(SystemCategory::StateTransition) {}

void ButtonGroupSystem::Initialize() {}
void ButtonGroupSystem::Finalize() {}

void ButtonGroupSystem::UpdateEntity(EntityHandle _handle) {
    KeyboardInput* keyInput    = GetScene()->GetKeyboardInput();
    GamepadInput* gamePadInput = GetScene()->GetGamepadInput();

    auto* buttonGroup = GetComponent<ButtonGroup>(_handle);
    if (buttonGroup == nullptr) {
        return;
    }

    int32_t currentButtonNumber      = buttonGroup->GetCurrentButtonNumber();
    EntityHandle currentButtonEntity = buttonGroup->GetEntityHandle(currentButtonNumber);
    const auto& buttonEntityHandles  = buttonGroup->GetButtonEntityHandles();
    Button* currentButton            = GetComponent<Button>(currentButtonEntity);
    if (!currentButton) {
        return;
    }
    currentButton->SetHovered(true);

    // 外部システムの入力に従う (ボタンのショートカットやマウスでの選択など)
    int32_t index = 0;
    for (const auto& entityHandle : buttonEntityHandles) {
        auto* button = GetComponent<Button>(entityHandle);
        if (button == nullptr || button == currentButton) {
            continue;
        }

        bool isInput = button->isHovered() || button->isPressed() || button->IsReleased();

        // 選択されているボタンの更新
        if (isInput) {
            // 初期化
            if (currentButton != nullptr) {
                currentButton->SetHovered(false);
                currentButton->SetPressed(false);
                currentButton->SetReleased(false);
            }
            currentButtonNumber = index;
            buttonGroup->SetCurrentButtonNumber(currentButtonNumber);
            // 直接移動させたら return
            return;
        }
        ++index;
    }

    bool isPressed  = currentButton->isPressed();
    bool isReleased = currentButton->IsReleased();

    // 決定ボタンの判定
    // pad が有効な場合は pad 優先
    if (gamePadInput->IsActive()) {
        for (const auto& button : buttonGroup->GetDecideGamepadButtons()) {
            if (isPressed) {
                if (gamePadInput->IsPress(button)) {
                    isPressed = true;
                    break;
                }
            } else {
                if (gamePadInput->IsRelease(button)) {
                    isReleased = true;
                    break;
                }
            }
        }
    }
    if (!isReleased) {
        for (const auto& key : buttonGroup->GetDecideKeys()) {
            if (isPressed) {
                if (keyInput->IsPress(key)) {
                    isPressed = true;
                    break;
                }
            } else {
                if (keyInput->IsTrigger(key)) {
                    isReleased = true;
                    break;
                }
            }
        }
    }

    currentButton->SetPressed(isPressed);
    currentButton->SetReleased(isReleased);

    if (isReleased) {
        return;
    }

    // 選択ボタンの変更
    int32_t delta = 0;

    if (gamePadInput->IsActive()) {
        for (const auto& button : buttonGroup->GetSelectAddGamepadButtons()) {
            if (gamePadInput->IsTrigger(button)) {
                ++delta;
                break;
            }
        }
        for (const auto& button : buttonGroup->GetSelectSubGamepadButtons()) {
            if (gamePadInput->IsTrigger(button)) {
                --delta;
                break;
            }
        }
    }

    for (const auto& key : buttonGroup->GetSelectAddKeys()) {
        if (keyInput->IsTrigger(key)) {
            ++delta;
        }
    }
    for (const auto& key : buttonGroup->GetSelectSubKeys()) {
        if (keyInput->IsTrigger(key)) {
            --delta;
        }
    }

    if (delta != 0) {
        delta = std::clamp(delta, -1, 1);

        // 初期化
        currentButton->SetPressed(false);
        currentButton->SetHovered(false);

        // 次のボタンを取得
        currentButtonNumber += delta;
        currentButtonNumber = std::clamp(currentButtonNumber, 0, (int32_t)buttonEntityHandles.size() - 1);

        currentButton = GetComponent<Button>(buttonGroup->GetEntityHandle(currentButtonNumber));

        // 次のボタンが存在しなければ戻す
        if (!currentButton) {
            currentButtonNumber -= delta;
            currentButton = GetComponent<Button>(buttonGroup->GetEntityHandle(currentButtonNumber));
        }
        currentButton->SetHovered(true);

        buttonGroup->SetCurrentButtonNumber(currentButtonNumber);

        /// ボタン位置に合わせて SpriteRenderer の位置を更新
        SpriteRenderer* buttonRenderer = GetComponent<SpriteRenderer>(currentButtonEntity);
        if (!buttonRenderer) {
            return;
        }

        auto& sprites = GetComponents<SpriteRenderer>(_handle);
        for (auto& sprite : sprites) {
            sprite.SetTranslate(buttonRenderer->GetTranslate()); // 強制更新
        }
    }
}
