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

	void Initialize() override;
	void Finalize() override;

	void UpdateEntity(OriGine::EntityHandle _handle);
	void InputUpdate(float _deltaTime,OriGine::MouseInput* _mouseInput,OriGine::GamepadInput* _padInput,CameraController* _cameraController);
};
