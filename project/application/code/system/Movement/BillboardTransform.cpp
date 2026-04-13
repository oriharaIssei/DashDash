#include "BillboardTransform.h"

/// engine
// camera
#include "camera/CameraManager.h"

/// ECS
// component
#include "component/BillboardComponent.h"
#include "component/transform/Transform.h"

using namespace OriGine;

void BillboardTransform::Initialize() {}
void BillboardTransform::Finalize() {}

void BillboardTransform::UpdateEntity(EntityHandle _handle) {
    // BillboardComponent がなければスキップ
    auto& billboards = GetComponents<BillboardComponent>(_handle);
    if (billboards.empty()) {
        return;
    }
    const auto& billboard = billboards[0];

    // 全軸が有効かどうか（フルビルボード）
    using BillboardAxis = BillboardComponent::BillboardAxis;
    const bool enableX  = billboard.axis.HasFlag(BillboardAxis::X);
    const bool enableY  = billboard.axis.HasFlag(BillboardAxis::Y);
    const bool enableZ  = billboard.axis.HasFlag(BillboardAxis::Z);
    const bool allAxes  = enableX && enableY && enableZ;

    // 有効な軸が一つも無ければスキップ
    if (!enableX && !enableY && !enableZ) {
        return;
    }

    CameraTransform cameraTransform = CameraManager::GetInstance()->GetTransform(GetScene());

    auto& transforms = GetComponents<Transform>(_handle);
    for (auto& transform : transforms) {
        // カメラ→オブジェクト方向ベクトル
        Vec3f lookAt = Vec3f(transform.translate - cameraTransform.translate);

        if (!allAxes) {
            // 有効な軸に対応する lookAt 成分をゼロにすることで
            // その軸の周りの回転だけを取り出す（残った成分に垂直な軸が回転軸になる）
            // 例: Y のみ有効 → lookAt.y = 0（XZ 平面に射影） → Y 軸回転（円柱ビルボード）
            if (enableX) {
                lookAt[X] = 0.0f;
            }
            if (enableY) {
                lookAt[Y] = 0.0f;
            }
            if (enableZ) {
                lookAt[Z] = 0.0f;
            }
        }

        // 制限後の lookAt が零ベクトルになる場合はスキップ
        if (lookAt.lengthSq() < 1e-6f) {
            continue;
        }
        lookAt = lookAt.normalize();

        // オブジェクトのフォワード軸（ローカル +Z）
        Vec3f nAxis = Vec3f(0.0f, 0.0f, 1.0f);

        // 回転軸を計算（外積）
        Vec3f normal = Vec3f::Cross(nAxis, lookAt);

        // 回転角度を計算（内積）
        float dot   = Vec3f::Dot(lookAt, nAxis);
        dot         = std::clamp(dot, -1.0f, 1.0f); // acosの範囲外防止
        float angle = std::acos(dot);

        // 回転軸を正規化（lookAt と nAxis が平行な場合は回転不要）
        if (normal.lengthSq() > 0.0f) {
            normal = normal.normalize();
        } else {
            normal = Vec3f(0.0f, 1.0f, 0.0f);
            angle  = 0.0f;
        }

        // クォータニオン作成・適用
        transform.rotate = Quaternion::RotateAxisAngle(normal, angle);
        transform.UpdateMatrix();
    }
}
