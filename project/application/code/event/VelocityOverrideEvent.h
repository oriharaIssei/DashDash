#pragma once

/// stl
#include <array>

/// ECS
// component
#include "component/ComponentHandle.h"

/// math
#include "math/Vector3.h"

/// <summary>
/// Velocity を上書きするイベント
/// </summary>
struct VelocityOverrideEvent {
    VelocityOverrideEvent() = default;
    VelocityOverrideEvent(OriGine::ComponentHandle _rigidbodyHandle, const OriGine::Vector3f& _newVelocity, std::array<bool, OriGine::Vector3f::dim> _isOverrideAxis)
        : rigidbodyHandle(_rigidbodyHandle), newVelocity(_newVelocity), isOverrideAxis(_isOverrideAxis) {}

    std::array<bool, OriGine::Vector3f::dim> isOverrideAxis; // 上書きする軸の指定
    OriGine::ComponentHandle rigidbodyHandle; // 変更対象の Rigidbody コンポーネントハンドル
    OriGine::Vector3f newVelocity; // 新しい速度ベクトル
};
