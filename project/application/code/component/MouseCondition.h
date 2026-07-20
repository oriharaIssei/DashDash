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

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    /// <summary>
    /// エディタ用編集UI
    /// </summary>
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;
    /// <summary>
    /// 終了処理
    /// </summary>
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
