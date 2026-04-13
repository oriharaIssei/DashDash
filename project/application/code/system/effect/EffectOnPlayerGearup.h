#pragma once
#include "system/ISystem.h"

/// util
#include "util/DiffValue.h"

/// ECS
// component
#include <component/transform/Transform.h>

/// math
#include <math/Vector3.h>

/// <summary>
/// PlayerがGearupしたとき, 衝撃波もどきを発生させる
/// </summary>
class EffectOnPlayerGearup
    : public OriGine::ISystem {
public:
    EffectOnPlayerGearup();
    ~EffectOnPlayerGearup();

    void Initialize() override;
    void Finalize() override;

protected:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

    void UpdateShockWaveRing(OriGine::EntityHandle _handle, OriGine::Transform* _playerTransform);

protected:
    const OriGine::Vec3f emitterOffset_ = {0.0f, 0.0f, 8.f};

    /// <summary>
    /// エフェクトの再生状態を管理する構造体
    /// </summary>
    struct AnimationState {
        AnimationState(bool _playState, float _currentTime, float _maxTime)
            : playState(_playState), currentTime(_currentTime), maxTime(_maxTime) {}
        DiffValue<bool> playState = false; // 再生中かどうか
        float currentTime         = 0.f;
        float maxTime             = 1.f;
    };
    /// ==========================================
    // ShockWave Ring
    /// ==========================================
    AnimationState shockWaveState_ = AnimationState(false, 0.f, 0.6f);

    const OriGine::Vec3f shockWaveOffset_ = {0.f, 0.f, 33.f}; // 衝撃波のオフセット位置
    const float minInnerRadius_           = 4.2f; // 衝撃波の中半径
    const float maxInnerRadius_           = 12.3f; // 衝撃波の中半径
    const float minOuterRadius_           = 5.1f; // 衝撃波の外半径
    const float maxOuterRadius_           = 16.0f; // 衝撃波の外半径
};
