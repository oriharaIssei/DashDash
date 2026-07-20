#pragma once

#include "system/ISystem.h"

/// stl
#include <string>
#include <vector>

/// <summary>
/// メニューはSubSceneとして実装されている想定. MenuがActiveの間 GameのUpdateを止める.
/// </summary>
class PauseMainSceneSystem
    : public OriGine::ISystem {
public:
    PauseMainSceneSystem();
    ~PauseMainSceneSystem() override;

    void Initialize() override;
    void Finalize() override;

private:
    /// <summary>
    /// 対象のSubSceneがアクティブかどうかをまとめて判定し、ポーズ状態の開始/解除に応じて
    /// 対象システム群・システムカテゴリの有効/無効を切り替える
    /// </summary>
    void Update();
    /// <summary>
    /// SubSceneコンポーネントの状態を見て、isPausing_ フラグを更新する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

private:
    // ポーズ中かどうか（対象SubSceneのいずれかがActiveであればtrue）
    bool isPausing_ = false;

    // ポーズ中に無効化するシステム名の一覧
    std::vector<std::string> deactivateSystemNames_ = {};
};
