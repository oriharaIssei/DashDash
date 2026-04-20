#pragma once

#include "system/ISystem.h"

#include "component/collision/collider/AABBCollider.h"
#include "component/gimmick/ICollisionTriggerComponent.h"

/// <summary>
/// 衝突 (Enter) をトリガーにして、ターゲット Entity のコンポーネントを
/// Activate / Deactivate する共通基底システム。
/// </summary>
class ICollisionTriggerSystem
	: public OriGine::ISystem{
public:
	ICollisionTriggerSystem(): ISystem(OriGine::SystemCategory::Collision){}
	~ICollisionTriggerSystem() override = default;

	void Initialize() override{}
	void Finalize() override{}

protected:
	virtual ICollisionTriggerComponent* GetTrigger(OriGine::EntityHandle _handle) = 0;
	virtual void ApplyActivate(OriGine::EntityHandle _targetHandle) = 0;
	virtual void ApplyDeactivate(OriGine::EntityHandle _targetHandle) = 0;

	void UpdateEntity(OriGine::EntityHandle _handle) override;
};
