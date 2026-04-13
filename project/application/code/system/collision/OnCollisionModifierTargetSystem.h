#pragma once

#include "system/ISystem.h"

/// stl
#include <unordered_map>

/// ECS
#include "ECS/Entity/EntityHandle.h"

/// <summary>
/// 衝突した際にSpeedModifierのターゲットとして設定するシステム
/// </summary>
class OnCollisionModifierTargetSystem
    : public OriGine::ISystem {
public:
    OnCollisionModifierTargetSystem();
    ~OnCollisionModifierTargetSystem() override;

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
    /// エンティティの更新処理
    /// </summary>
    void UpdateEntity(OriGine::EntityHandle _handle) override;

    /// <summary>
    /// WhileCollidingモードで生成したModifierエンティティを追跡するマップ
    /// key: {トリガーエンティティ, 衝突相手エンティティ}
    /// value: 生成したModifierエンティティ
    /// </summary>
    struct PairHash {
        size_t operator()(const std::pair<OriGine::EntityHandle, OriGine::EntityHandle>& _p) const {
            auto h1 = std::hash<OriGine::EntityHandle>{}(_p.first);
            auto h2 = std::hash<OriGine::EntityHandle>{}(_p.second);
            return h1 ^ (h2 << 16);
        }
    };
    std::unordered_map<
        std::pair<OriGine::EntityHandle, OriGine::EntityHandle>,
        OriGine::EntityHandle,
        PairHash>
        activeModifiers_;
};
