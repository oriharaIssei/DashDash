#pragma once
#include "system/ISystem.h"

/// stl
#include <memory>

/// <summary>
///  リスタートメッセージを受け取ってリスタート処理を行うシステム
/// </summary>
class RestartSystem
    : public OriGine::ISystem,
      public std::enable_shared_from_this<RestartSystem> {
public:
    RestartSystem();
    ~RestartSystem() override;

    void Initialize() override;
    void Finalize() override;

protected:
    void Update() override;

protected:
    size_t restartEventId_ = 0;
    bool isRestarting_     = false;
};
