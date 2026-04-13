#pragma once

#include "system/ISystem.h"

class PathControllerSystem
    : public OriGine::ISystem {
public:
    PathControllerSystem();
    ~PathControllerSystem() override = default;
    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
