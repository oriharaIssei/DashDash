#pragma once

#include "system/ISystem.h"

/// <summary>
/// ProxyColliderの衝突を検知しアニメーションをトリガーする
/// </summary>
class PlayerAheadCollisionReactionSystem
    : public OriGine::ISystem {
public:
    PlayerAheadCollisionReactionSystem();
    ~PlayerAheadCollisionReactionSystem() {}

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

private:
    /// <summary>
    /// 落下/ジャンプ中のPlayerが前方の壁に衝突したとき、壁との押し戻し量から傾き角を計算しPlayerの姿勢に反映する
    /// </summary>
    /// <param name="_handle">壁側（ProxyCollider）のエンティティハンドル</param>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
