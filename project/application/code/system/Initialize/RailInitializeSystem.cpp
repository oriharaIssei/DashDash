#include "RailInitializeSystem.h"

/// engine
#include "scene/Scene.h"

/// ECS
// component
#include "component/collision/collider/base/CollisionCategoryManager.h"
#include "component/collision/collider/CapsuleCollider.h"
#include "component/collision/CollisionPushBackInfo.h"
#include "component/gimmick/RailPoints.h"
// system
#include "system/collision/CollisionCheckSystem.h"

/// util
#include <nameof.h>

/// math
#include "math/Spline.h"

using namespace OriGine;

RailInitializeSystem::RailInitializeSystem()
    : ISystem(SystemCategory::Initialize) {}
RailInitializeSystem::~RailInitializeSystem() {}

void RailInitializeSystem::Initialize() {}
void RailInitializeSystem::Finalize() {}

void RailInitializeSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    // RailPoints コンポーネントを取得
    auto* railPoints = GetComponent<RailPoints>(_handle);
    if (railPoints == nullptr) {
        return;
    }
    if (railPoints->points.size() < 2) {
        return;
    }

    // CapsuleCollider コンポーネントを追加
    auto& colliders          = GetComponents<CapsuleCollider>(_handle);
    std::deque<Vec3f> points = railPoints->points;

    int32_t segmentCount = 0;
    if (railPoints->points.size() >= 4) {
        constexpr int32_t kMinDivision = 6;
        constexpr int32_t kDivider     = 4;
        points                         = CatmullRomSpline(points, (std::max)(railPoints->segmentDivide / kDivider, kMinDivision));
    }
    segmentCount = static_cast<int32_t>(points.size()) - 1;

    int32_t diff = segmentCount - static_cast<int32_t>(colliders.size());
    if (diff > 0) {
        // 足りない分を追加
        for (size_t i = 0; i < static_cast<size_t>(diff); ++i) {
            AddComponent<CapsuleCollider>(_handle);
        }

        colliders = GetComponents<CapsuleCollider>(_handle);
    }

    // 衝突カテゴリを取得
    CollisionCategory railCategory = CollisionCategoryManager::GetInstance()->GetCategory("StageObject");

    // 各セグメントごとに CapsuleCollider を追加
    for (int i = 0; i < segmentCount; ++i) {
        const auto& segmentStart = points[i];
        const auto& segmentEnd   = points[i + 1];

        // CapsuleCollider コンポーネントを追加
        auto* capsuleCollider = GetComponent<CapsuleCollider>(_handle, i);
        if (capsuleCollider) {
            capsuleCollider->SetLocalStart(segmentStart);
            capsuleCollider->SetLocalEnd(segmentEnd);
            capsuleCollider->SetLocalRadius(railPoints->radius + railPoints->collisionRadiusOffset);
            capsuleCollider->CalculateWorldShape();

            // 衝突カテゴリを設定
            capsuleCollider->SetCollisionCategory(railCategory);
        }
    }

    // pushbackInfoを持っていなければ追加
    auto* info = GetComponent<CollisionPushBackInfo>(_handle);
    if (!info) {
        AddComponent(_handle, nameof<CollisionPushBackInfo>());
    }

    // Systemに登録
    GetScene()->GetSystem(nameof<CollisionCheckSystem>())->AddEntity(_handle);
}
