#pragma once

#include "system/ISystem.h"

/// stl
#include <memory>
#include <string>

/// <summary>
/// シーン遷移リクエスト受信システム
/// </summary>
class SceneTransitionRequestReceiverSystem
    : public OriGine::ISystem,
      public std::enable_shared_from_this<SceneTransitionRequestReceiverSystem> {
public:
    SceneTransitionRequestReceiverSystem();
    ~SceneTransitionRequestReceiverSystem();

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

protected:
    bool isSceneChangeRequested_      = false; ///< シーン変更がリクエストされたかどうか
    size_t sceneChangeRequestEventId_ = 0; ///< シーン変更リクエストイベントID
    std::string targetSceneName_      = ""; ///< 遷移先のシーン名
};
