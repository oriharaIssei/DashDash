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
    /// <summary>
    /// 衝突発生時にターゲットへ適用する動作の種別。
    /// </summary>
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
    // 衝突時に適用する動作モード（デフォルト: Activate）
    Mode mode_ = Mode::Activate;
    // 動作対象となる Entity のハンドル一覧
    std::vector<OriGine::EntityHandle> targetHandles_;
};

// Mode の値に対応するUI表示用の名称一覧
static const char* kCollisionTriggerModeNames[] = {
    "Activate",
    "Deactivate",
};
