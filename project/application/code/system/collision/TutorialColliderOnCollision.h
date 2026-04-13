#pragma once
#include "system/ISystem.h"

/// <summary>
/// チュートリアル用の衝突反応システム
/// </summary>
class TutorialColliderOnCollision
    : public OriGine::ISystem {
public:
    TutorialColliderOnCollision();
    ~TutorialColliderOnCollision() override;

    void Initialize() override;
    void Finalize() override;

    void Update() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

protected:
    OriGine::EntityHandle playerEntityHandle_ = OriGine::EntityHandle();
};
