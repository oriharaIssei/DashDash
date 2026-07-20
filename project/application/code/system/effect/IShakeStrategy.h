#pragma once

#include "component/Camera/CameraShakeSourceComponent.h"
#include "math/Vector3.h"

/// <summary>
/// シェイクの種類ごとの計算処理を抽象化するインターフェース
/// </summary>
class IShakeStrategy{
public:
	virtual ~IShakeStrategy() = default;

	/// <summary>
	/// シェイク量（カメラ座標へのオフセット）を計算する
	/// </summary>
	/// <param name="_source">シェイク元となるパラメータを持つコンポーネント</param>
	/// <param name="_deltaTime">経過時間</param>
	/// <returns>カメラ座標に加算するオフセット量</returns>
	virtual OriGine::Vec3f Calculate(CameraShakeSourceComponent& _source,float _deltaTime) = 0;
};

/// <summary>
/// Sin波に基づくシェイク計算
/// </summary>
class SinCurveShakeStrategy : public IShakeStrategy{
public:
	OriGine::Vec3f Calculate(CameraShakeSourceComponent& _source,float _deltaTime) override;
};

/// <summary>
/// ノイズに基づくシェイク計算
/// </summary>
class NoiseShakeStrategy : public IShakeStrategy{
public:
	OriGine::Vec3f Calculate(CameraShakeSourceComponent& _source,float _deltaTime) override;
};

/// <summary>
/// バネに基づくシェイク計算
/// </summary>
class SpringShakeStrategy : public IShakeStrategy{
public:
	OriGine::Vec3f Calculate(CameraShakeSourceComponent& _source,float _deltaTime) override;
};
