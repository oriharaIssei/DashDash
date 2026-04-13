#include "BackFireInitialize.h"

/// component
#include "component/effect/MaterialEffectPipeLine.h"
#include "component/player/PlayerStatus.h"
#include "component/renderer/MeshRenderer.h"

using namespace OriGine;

BackFireInitialize::BackFireInitialize() : ISystem(OriGine::SystemCategory::Initialize) {}
BackFireInitialize::~BackFireInitialize() {}

void BackFireInitialize::Initialize() {}
void BackFireInitialize::Finalize() {}

void BackFireInitialize::UpdateEntity(OriGine::EntityHandle _handle) {
    /// Transform
    Transform* playerTransform   = GetComponent<OriGine::Transform>(GetUniqueEntity("Player"));
    Transform* backFirTrans      = GetComponent<OriGine::Transform>(_handle);
    OriGine::EntityHandle sparks = GetUniqueEntity("BackFireSparks");
    Transform* sparksTrans       = GetComponent<OriGine::Transform>(sparks);
    // 親子関係設定
    backFirTrans->parent = playerTransform;
    sparksTrans->parent  = backFirTrans;
}
