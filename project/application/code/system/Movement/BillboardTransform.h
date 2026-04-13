#pragma once
#include "system/ISystem.h"

/// <summary>
/// Transformをビルボード化するシステム
/// </summary>
class BillboardTransform
    : public OriGine::ISystem {
public:
    BillboardTransform() : ISystem(OriGine::SystemCategory::Movement, 0) {};
    ~BillboardTransform() override {}

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;
};
