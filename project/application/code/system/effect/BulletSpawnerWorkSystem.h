#pragma once

#include "system/ISystem.h"

namespace OriGine {

/// <summary>
/// BulletSpawner コンポーネントを管理するシステム。
/// スポーン時に、生成された Entity の Rigidbody Velocity を
/// Spawner の Transform の回転で回転させて発射方向を決定する。
/// </summary>
class BulletSpawnerWorkSystem
    : public ISystem {
public:
    BulletSpawnerWorkSystem() : ISystem(SystemCategory::Effect) {}
    ~BulletSpawnerWorkSystem() override = default;

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(EntityHandle _handle) override;
};

} // namespace OriGine
