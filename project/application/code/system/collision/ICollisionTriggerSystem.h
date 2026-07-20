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
	/// <summary>
	/// 対象Entityが持つトリガーコンポーネントを取得する（派生クラスごとのトリガー種別を返す）
	/// </summary>
	/// <param name="_handle">トリガーを持つEntityのハンドル</param>
	/// <returns>トリガーコンポーネントへのポインタ（存在しない場合はnullptr）</returns>
	virtual ICollisionTriggerComponent* GetTrigger(const OriGine::EntityHandle& _handle) = 0;
	/// <summary>
	/// トリガー対象EntityをActivateする（派生クラスごとの具体的な有効化処理）
	/// </summary>
	/// <param name="_targetHandle">Activate対象のEntityハンドル</param>
	virtual void ApplyActivate(const OriGine::EntityHandle& _targetHandle) = 0;
	/// <summary>
	/// トリガー対象EntityをDeactivateする（派生クラスごとの具体的な無効化処理）
	/// </summary>
	/// <param name="_targetHandle">Deactivate対象のEntityハンドル</param>
	virtual void ApplyDeactivate(const OriGine::EntityHandle& _targetHandle) = 0;

	/// <summary>
	/// Colliderの衝突状態(Enter)を判定し、トリガーのモードに応じてターゲットEntityをActivate/Deactivateする
	/// </summary>
	/// <param name="_handle">トリガー・コライダーを持つEntityのハンドル</param>
	void UpdateEntity(const OriGine::EntityHandle& _handle) override;
};
