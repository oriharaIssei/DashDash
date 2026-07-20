#include "ICollisionTriggerSystem.h"

/// ECS
// entity
#include <entity/EntityHandle.h>
// component
#include <component/gimmick/ICollisionTriggerComponent.h>
#include <component/collision/collider/AABBCollider.h>
#include <component/collision/collider/base/Collider.h>

using namespace OriGine;

void ICollisionTriggerSystem::UpdateEntity(const OriGine::EntityHandle& _handle){
	auto* trigger  = GetTrigger(_handle);
	auto* collider = GetComponent<AABBCollider>(_handle);

	if(!trigger || !collider){
		return;
	}

	// いずれかの相手とEnter状態（衝突した瞬間）であればトリガー成立とみなす
	bool isTriggered = false;
	for(const auto& [handle,state] : collider->GetCollisionStateMap()){
		if(state == CollisionState::Enter){
			isTriggered = true;
			break;
		}
	}

	if(!isTriggered){
		return;
	}

	// トリガーが登録する全ターゲットに対し、モードに応じてActivate/Deactivateを適用する
	for(OriGine::EntityHandle targetHandle : trigger->GetTargetHandles()){
		switch(trigger->GetMode()){
			case ICollisionTriggerComponent::Mode::Activate:
				ApplyActivate(targetHandle);
				break;
			case ICollisionTriggerComponent::Mode::Deactivate:
				ApplyDeactivate(targetHandle);
				break;
		}
	}
}