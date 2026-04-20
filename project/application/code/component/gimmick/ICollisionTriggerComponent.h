#pragma once

/// stl
#include <vector>

/// engine
#include "component/IComponent.h"
#include "entity/EntityHandle.h"

/// <summary>
/// 衝突をトリガーにして、指定した Entity の特定コンポーネントを
/// アクティブ化 または 非アクティブ化 する基底コンポーネント。
/// </summary>
class ICollisionTriggerComponent
    : public OriGine::IComponent {
public:
    enum class Mode {
        Activate,
        Deactivate,
    };

public:
    ICollisionTriggerComponent()           = default;
    ~ICollisionTriggerComponent() override = default;

    Mode GetMode() const { return mode_; }
    void SetMode(Mode _mode) { mode_ = _mode; }

    const std::vector<OriGine::EntityHandle>& GetTargetHandles() const { return targetHandles_; }

protected:
    Mode mode_ = Mode::Activate;
    std::vector<OriGine::EntityHandle> targetHandles_;
};

static const char* kCollisionTriggerModeNames[] = {
    "Activate",
    "Deactivate",
};
