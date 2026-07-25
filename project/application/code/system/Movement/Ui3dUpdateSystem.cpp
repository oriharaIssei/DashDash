#include "UI3DUpdateSystem.h"

/// ECS
// component
#include "component/transform/Transform.h"

using namespace OriGine;

Ui3dUpdateSystem::Ui3dUpdateSystem() : ISystem(OriGine::SystemCategory::Movement) {}
Ui3dUpdateSystem::~Ui3dUpdateSystem() {}

void Ui3dUpdateSystem::Initialize() {}
void Ui3dUpdateSystem::Finalize() {}

void Ui3dUpdateSystem::UpdateEntity(const OriGine::EntityHandle& _handle) {
    auto playerHandle = GetUniqueEntity("Player");
    if (!playerHandle.IsValid()) {
        return;
    }
    auto playerTransform = GetComponent<OriGine::Transform>(playerHandle);
    auto transform       = GetComponent<OriGine::Transform>(_handle);
    auto camTransform    = transform->parent; // Camera の Transform
    // NOTE: transform のnullチェックが、transform->parent を読んだ後に来ている。
    //       このUIエンティティは必ずTransformを持つ前提のため現状は問題ないが、
    //       順序としては先にチェックすべき箇所
    if (!transform || !playerTransform || !camTransform) {
        return;
    }

    // --- ワールド方向 ---
    OriGine::Vec3f worldDir = transform->GetWorldTranslate() - playerTransform->GetWorldTranslate();
    worldDir                = worldDir.normalize();

    // --- カメラのローカル空間へ変換 ---
    // このUIはカメラの子(transform->parentがカメラ)なので、rotateはカメラから見た相対回転として
    // 解釈される。ワールドの向きをそのまま入れるとカメラの回転が二重に掛かるため、
    // カメラ回転の逆行列を掛けてカメラローカルの向きに直してから使う
    Matrix4x4 invCam        = MakeMatrix4x4::RotateQuaternion(camTransform->rotate).inverse();
    OriGine::Vec3f localDir = OriGine::Vec3f(worldDir * invCam).normalize();

    // --- UI のローカル回転を設定 ---
    // NOTE: この代入は直後の傾き補正で同じ値から上書きされるため、実質的に結果へ影響しない
    transform->rotate = Quaternion::LookAt(localDir, axisY);

    // --- 傾き補正 ---
    // プレイヤー方向を完全に向くとUIが真横や裏を向いて読めなくなるため、
    // 無回転(カメラ正面)との間をkTiltだけ補間し、正面寄りに留めたまま方向だけ示唆する
    constexpr float kTilt = 0.33f; // 0=正面固定, 1=完全にPlayer方向
    transform->rotate     = Slerp(
        Quaternion::Identity(),
        Quaternion::LookAt(localDir, axisY),
        kTilt);

    transform->UpdateMatrix();
}
