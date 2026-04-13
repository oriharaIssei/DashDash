#pragma once
#include "system/ISystem.h"

/// stl
#include <memory>
#include <vector>

/// event
#include "event/VelocityOverrideEvent.h"

/// <summary>
/// VelocityOverrideTriggerSystem で適応された強制移動を実行するシステム
/// </summary>
class VelocityOverrideSystem
    : public OriGine::ISystem,
      public std::enable_shared_from_this<VelocityOverrideSystem> {
public:
    VelocityOverrideSystem();
    ~VelocityOverrideSystem() override;
    void Initialize() override;
    void Finalize() override;

    void Update();

private:
    size_t velocityOverrideEventId_ = 0; ///< シーン変更リクエストイベントID
    std::vector<VelocityOverrideEvent> velocityOverrides_; ///< 保留中の速度上書きイベント
};
