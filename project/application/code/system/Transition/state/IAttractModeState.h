#pragma once

/// <summary>
/// アトラクションモードの状態インターフェース
/// </summary>
class IAttractModeState {
public:
    virtual ~IAttractModeState() = default;

    virtual void Enter() {}
    virtual void Update(float _deltaTime) = 0;

    virtual bool IsActive() const             = 0;
    virtual bool ShouldRender() const         = 0;
    virtual IAttractModeState* Transition()   = 0;
};
