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

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

protected:
    /// <summary>
    /// BulletSpawnerコンポーネントを持つEntityごとに弾のスポーン処理を行う
    /// </summary>
    /// <param name="_handle">対象のエンティティハンドル</param>
    void UpdateEntity(const EntityHandle& _handle) override;
};

} // namespace OriGine
