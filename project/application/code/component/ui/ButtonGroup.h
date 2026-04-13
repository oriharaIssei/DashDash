#pragma once
#include "component/IComponent.h"

/// stl
#include <unordered_map>

/// engine
#include "input/InputManager.h"

/// <summary>
/// 1シーン内で複数のボタンをグループ化し、選択と決定の操作を管理するコンポーネント
/// </summary>
class ButtonGroup
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const ButtonGroup& r);
    friend void from_json(const nlohmann::json& j, ButtonGroup& r);

public:
    ButtonGroup()           = default;
    ~ButtonGroup() override = default;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

private:
    // ボタンのエンティティID
    std::vector<OriGine::EntityHandle> buttonEntityHandles_;
    int32_t startButtonNumber_   = 0;
    int32_t currentButtonNumber_ = 0;

    // ボタンを選ぶためのキー ボタン
    std::vector<OriGine::Key> selectAddKeys_{};
    std::vector<OriGine::GamepadButton> selectAddGamepadButtons_{};
    std::vector<OriGine::Key> selectSubKeys_{};
    std::vector<OriGine::GamepadButton> selectSubGamepadButtons_{};

    // ボタンを決定するためのキー ボタン
    std::vector<OriGine::Key> decideKeys_{};
    std::vector<OriGine::GamepadButton> decideGamepadButtons_{};

public:
    int32_t GetCurrentButtonNumber() const { return currentButtonNumber_; }
    void SetCurrentButtonNumber(int32_t _buttonNumber) { currentButtonNumber_ = _buttonNumber; }
    int32_t GetStartButtonNumber() const { return startButtonNumber_; }
    void SetStartButtonNumber(int32_t _buttonNumber) { startButtonNumber_ = _buttonNumber; }

    std::vector<OriGine::EntityHandle>& GetButtonEntityHandles() { return buttonEntityHandles_; }

    int32_t GetButtonNumber(OriGine::EntityHandle _entityId) const {
        int32_t index = 0;
        for (const auto& handle : buttonEntityHandles_) {
            if (handle == _entityId) {
                return index;
            }
            ++index;
        }
        return -1; // 見つからなかった場合のデフォルト値
    }
    OriGine::EntityHandle GetEntityHandle(int32_t _buttonNumber) const {
        if (_buttonNumber >= 0 && _buttonNumber < buttonEntityHandles_.size()) {
            return buttonEntityHandles_[_buttonNumber];
        }
        return OriGine::EntityHandle(); // 見つからなかった場合のデフォルト値
    }

    const std::vector<OriGine::Key>& GetSelectAddKeys() const { return selectAddKeys_; }
    const std::vector<OriGine::GamepadButton>& GetSelectAddGamepadButtons() const { return selectAddGamepadButtons_; }
    const std::vector<OriGine::Key>& GetSelectSubKeys() const { return selectSubKeys_; }
    const std::vector<OriGine::GamepadButton>& GetSelectSubGamepadButtons() { return selectSubGamepadButtons_; }
    const std::vector<OriGine::Key>& GetDecideKeys() const { return decideKeys_; }
    const std::vector<OriGine::GamepadButton>& GetDecideGamepadButtons() const { return decideGamepadButtons_; }
};
