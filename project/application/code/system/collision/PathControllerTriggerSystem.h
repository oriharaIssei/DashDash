#pragma once

#include "system/collision/ICollisionTriggerSystem.h"

/// <summary>
/// PathControllerTrigger を持つ Entity の Collider が衝突 (Enter) したとき、
/// ターゲット Entity の PathController を Activate / Deactivate するシステム。
/// </summary>
class PathControllerTriggerSystem
    : public ICollisionTriggerSystem {
public:
    PathControllerTriggerSystem()           = default;
    ~PathControllerTriggerSystem() override = default;

protected:
    /// <summary>
    /// 対象EntityのPathControllerTriggerComponentを取得する
    /// </summary>
    /// <param name="_handle">トリガーを持つEntityのハンドル</param>
    /// <returns>PathControllerTriggerComponentへのポインタ</returns>
    ICollisionTriggerComponent* GetTrigger(const OriGine::EntityHandle& _handle) override;
    /// <summary>
    /// ターゲットEntityが持つ全PathControllerの再生を開始する
    /// </summary>
    /// <param name="_targetHandle">対象Entityのハンドル</param>
    void ApplyActivate(const OriGine::EntityHandle& _targetHandle) override;
    /// <summary>
    /// ターゲットEntityが持つ全PathControllerの再生を停止する
    /// </summary>
    /// <param name="_targetHandle">対象Entityのハンドル</param>
    void ApplyDeactivate(const OriGine::EntityHandle& _targetHandle) override;
};
