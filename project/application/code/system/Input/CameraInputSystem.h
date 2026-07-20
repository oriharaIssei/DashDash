#pragma once

/// parent
#include "system/ISystem.h"

namespace OriGine {
	/// engine
	// input
	class GamepadInput;
	class MouseInput;
}

/// ECS
// component
class CameraController;

/// <summary>
/// Cameraの入力処理を行うシステム
/// </summary>
class CameraInputSystem
	: public OriGine::ISystem{
public:
	CameraInputSystem(): ISystem(OriGine::SystemCategory::Input){}
	~CameraInputSystem() = default;

	/// <summary>
	/// 初期化処理（本システムでは特に処理なし）
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 終了処理（本システムでは特に処理なし）
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 対象エンティティが持つCameraControllerに対して入力更新処理を呼び出す
	/// </summary>
	/// <param name="_handle">対象エンティティのハンドル</param>
	void UpdateEntity(const OriGine::EntityHandle& _handle);
	/// <summary>
	/// マウス・ゲームパッド入力からカメラの目標回転角度を更新する
	/// </summary>
	/// <param name="_deltaTime">経過時間</param>
	/// <param name="_mouseInput">マウス入力</param>
	/// <param name="_padInput">ゲームパッド入力</param>
	/// <param name="_cameraController">更新対象のカメラコントローラ</param>
	void InputUpdate(float _deltaTime,OriGine::MouseInput* _mouseInput,OriGine::GamepadInput* _padInput,CameraController* _cameraController);
};
