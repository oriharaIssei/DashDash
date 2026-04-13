#include "Ui3dObjectInitialize.h"

/// ECS
// component
#include "component/transform/Transform.h"

using namespace OriGine;

Ui3dObjectInitialize::Ui3dObjectInitialize() : ISystem(SystemCategory::Initialize) {}

void Ui3dObjectInitialize::Initialize() {}
void Ui3dObjectInitialize::Finalize() {}

void Ui3dObjectInitialize::UpdateEntity(EntityHandle _handle) {
    // カメラエンティティ取得
    EntityHandle cameraHandle = GetUniqueEntity("GameCamera");
    if (!cameraHandle.IsValid()) {
        return;
    }

    auto* cameraTransform = GetComponent<OriGine::Transform>(cameraHandle);
    auto* transform       = GetComponent<OriGine::Transform>(_handle);
    if (!cameraTransform || !transform) {
        return;
    }

    // parentを結ぶ
    transform->parent = cameraTransform;
    transform->UpdateMatrix();
}
