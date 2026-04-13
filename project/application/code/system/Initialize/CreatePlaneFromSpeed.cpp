#include "CreatePlaneFromSpeed.h"

/// engine
#include "Engine.h"
#include "scene/Scene.h"
#include "winApp/WinApp.h"

/// ECS
// component
#include "component/material/Material.h"
#include "component/renderer/primitive/PlaneRenderer.h"
#include "component/ui/SpeedFor3dUIComponent.h"
// system
#include "system/effect/PlayerSpeedFor3dUI.h"
#include "system/initialize/Ui3dObjectInitialize.h"
#include "system/movement/Ui3dUpdateSystem.h"
#include "system/render/OverlayRenderSystem.h"
#include "system/SystemRunner.h"

using namespace OriGine;

CreatePlaneFromSpeed::CreatePlaneFromSpeed()
    : ISystem(OriGine::SystemCategory::Initialize) {}
CreatePlaneFromSpeed::~CreatePlaneFromSpeed() {}

void CreatePlaneFromSpeed::Initialize() {}
void CreatePlaneFromSpeed::Finalize() {}

void CreatePlaneFromSpeed::UpdateEntity(OriGine::EntityHandle _handle) {
    auto speedFor3dUIComponent = GetComponent<SpeedFor3dUIComponent>(_handle);
    if (!speedFor3dUIComponent) {
        return; // タイマーコンポーネントがない場合は何もしない
    }
    Entity* entity = GetEntity(_handle);

    // Sprite用のEntityを作成
    EntityHandle ui3dEntityId   = CreateEntity(entity->GetDataType() + "_Speed3dUI", false);
    OriGine::Entity* ui3dEntity = GetEntity(ui3dEntityId);
    ui3dEntity->SetShouldSave(false); // セーブしない
    CreatePlanesFromComponent(ui3dEntityId, speedFor3dUIComponent);
}

void CreatePlaneFromSpeed::CreatePlanesFromComponent(OriGine::EntityHandle _handle, SpeedFor3dUIComponent* _comp) {
    // ==========================================
    // 3D基準座標の生成
    // ==========================================
    int digitAll = _comp->digitInteger + _comp->digitDecimal;

    OriGine::Vec2f uvScale = _comp->numberTileSize / _comp->numbersTextureSize;

    OriGine::Vec3f pos = _comp->offset;

    // 整数部の全幅
    float widthInteger = _comp->digitInteger * _comp->planeScaleInteger[X]
                         + (_comp->digitInteger > 0 ? (_comp->digitInteger - 1) * _comp->planeMarginInteger[X] : 0);

    // origin が「整数部右端」と「小数部左端」の間になるように補正
    pos[X] -= widthInteger;
    pos[X] += _comp->marginBetweenIntegerAndDecimal[X] * 0.5f;

    // ==========================================
    // Plane 用の Entity を System に登録
    // ==========================================
    GetScene()->GetSystemRunnerRef()->GetSystemRef<OverlayRenderSystem>()->AddEntity(_handle);
    GetScene()->GetSystemRunnerRef()->GetSystemRef<Ui3dObjectInitialize>()->AddEntity(_handle);
    GetScene()->GetSystemRunnerRef()->GetSystemRef<PlayerSpeedFor3dUI>()->AddEntity(_handle);
    GetScene()->GetSystemRunnerRef()->GetSystemRef<Ui3dUpdateSystem>()->AddEntity(_handle);

    _comp->SetPlaneEntityHandle(_handle);

    AddComponent<OriGine::Transform>(_handle);
    auto transform       = GetComponent<OriGine::Transform>(_handle);
    transform->translate = _comp->worldPosition;
    transform->UpdateMatrix();

    // ==========================================
    // 整数部 Plane 生成
    // ==========================================
    for (int i = 0; i < _comp->digitInteger; ++i) {
        AddComponent<OriGine::PlaneRenderer>(_handle);
        AddComponent<Material>(_handle);

        auto* plane    = GetComponent<OriGine::PlaneRenderer>(_handle, i);
        auto* material = GetComponent<Material>(_handle, i);

        plane->SetIsRender(true);
        plane->SetMaterialIndex(i);

        plane->LoadTexture(_comp->numbersTexturePath);

        material->uvTransform_.scale_ = uvScale;

        // Plane の Transform に書き込み
        auto& planeTransform     = plane->GetTransform();
        planeTransform.parent    = transform;
        planeTransform.scale     = OriGine::Vec3f(_comp->planeScaleInteger, 1.f);
        planeTransform.translate = pos;

        planeTransform.UpdateMatrix();

        pos[X] += _comp->planeScaleInteger[X];
        pos += OriGine::Vec3f(_comp->planeMarginInteger, 0.f);
    }

    // 整数部 → 小数部の間隔を追加
    pos += OriGine::Vec3f(_comp->marginBetweenIntegerAndDecimal[X],
        _comp->marginBetweenIntegerAndDecimal[Y],
        0.f);

    // ==========================================
    // 小数部 Plane 生成
    // ==========================================
    for (int i = _comp->digitInteger; i < digitAll; ++i) {
        AddComponent<OriGine::PlaneRenderer>(_handle);
        AddComponent<Material>(_handle);

        auto* plane    = GetComponent<OriGine::PlaneRenderer>(_handle, i);
        auto* material = GetComponent<Material>(_handle, i);

        plane->SetIsRender(true);
        plane->SetMaterialIndex(i);

        plane->LoadTexture(_comp->numbersTexturePath);

        material->uvTransform_.scale_ = uvScale;

        // Plane の Transform に書き込み
        auto& planeTransform     = plane->GetTransform();
        planeTransform.parent    = transform;
        planeTransform.scale     = OriGine::Vec3f(_comp->planeScaleDecimal, 1.f);
        planeTransform.translate = pos;

        planeTransform.UpdateMatrix();

        pos[X] += _comp->planeScaleDecimal[X];
        pos += OriGine::Vec3f(_comp->planeMarginDecimal[X], _comp->planeMarginDecimal[Y], 0.f);
    }
}
