#pragma once
#include "system/ISystem.h"

/// stl
#include <memory>
#include <vector>

/// event
#include "event/AddForceEvent.h"

/// <summary>
/// AddForceTriggerSystem で適応された加算移動を実行するシステム
/// </summary>
class ApplyAddForceSystem
    : public OriGine::ISystem,
      public std::enable_shared_from_this<ApplyAddForceSystem> {
public:
    ApplyAddForceSystem();
    ~ApplyAddForceSystem() override;
    void Initialize() override;
    void Finalize() override;

    void Update();

private:
    size_t addForceEventId_ = 0; ///< シーン変更リクエストイベントID
    std::vector<AddForceEvent> addForceEvents_; ///< 保留中の速度上書きイベント
};
