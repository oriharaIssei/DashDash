#pragma once

#include "system/ISystem.h"

/// <summary>
/// PathControllerコンポーネントに設定された点列に沿ってエンティティを移動・回転させるシステム。
/// Linear/CatmullRom/Bezier補間や Once/Loop/PingPong の再生方式に対応する。
/// </summary>
class PathControllerSystem
    : public OriGine::ISystem {
public:
    PathControllerSystem();
    ~PathControllerSystem() override = default;
    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
