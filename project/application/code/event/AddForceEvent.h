#pragma once

/// ECS
// component
#include "component/ComponentHandle.h"

/// math
#include "math/Vector3.h"

/// <summary>
/// エンティティに力を加えるイベント
/// </summary>
struct AddForceEvent {
    AddForceEvent() = default;
    AddForceEvent(OriGine::ComponentHandle _rigidbodyHandle,
        const OriGine::Vector3f& _force)
        : rigidbodyHandle(_rigidbodyHandle), force(_force) {}

    OriGine::ComponentHandle rigidbodyHandle;
    OriGine::Vector3f force;
};
