#pragma once

#include "system/collision/ICollisionTriggerSystem.h"

/// <summary>
/// BulletSpawnerTrigger を持つ Entity の Collider が衝突 (Enter) したとき、
/// ターゲット Entity の BulletSpawner を Activate / Deactivate するシステム。
/// </summary>
class BulletSpawnerTriggerSystem
    : public ICollisionTriggerSystem {
public:
    BulletSpawnerTriggerSystem()           = default;
    ~BulletSpawnerTriggerSystem() override = default;

protected:
    /// <summary>
    /// 対象EntityのBulletSpawnerTriggerComponentを取得する
    /// </summary>
    /// <param name="_handle">トリガーを持つEntityのハンドル</param>
    /// <returns>BulletSpawnerTriggerComponentへのポインタ</returns>
    ICollisionTriggerComponent* GetTrigger(const OriGine::EntityHandle& _handle) override;
    /// <summary>
    /// ターゲットEntityが持つ全BulletSpawnerの発射を開始する
    /// </summary>
    /// <param name="_targetHandle">対象Entityのハンドル</param>
    void ApplyActivate(const OriGine::EntityHandle& _targetHandle) override;
    /// <summary>
    /// ターゲットEntityが持つ全BulletSpawnerの発射を停止する
    /// </summary>
    /// <param name="_targetHandle">対象Entityのハンドル</param>
    void ApplyDeactivate(const OriGine::EntityHandle& _targetHandle) override;
};
