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
    void Update();
    void UpdateEntity(OriGine::EntityHandle _handle) override;

private:
    bool isPausing_ = false;

    std::vector<std::string> deactivateSystemNames_ = {};
};
