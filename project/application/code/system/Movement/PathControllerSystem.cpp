#include "PathControllerSystem.h"

/// stl
#include <algorithm>

/// engine
#include "Engine.h"

/// ECS
// component
#include "component/gimmick/PathController.h"
#include "component/spline/SplineConfig.h"
#include "component/transform/Transform.h"

/// math
#include "math/Interpolation.h"
#include "math/Quaternion.h"
#include "math/Spline.h"

using namespace OriGine;

//-------------------------------------------------------
// 内部ヘルパー
//-------------------------------------------------------
namespace {

/// <summary>
/// 区間 [index, index+1] 内の補間座標を計算する
/// </summary>
Vec3f CalcPosition(
    const std::vector<Vec3f>& _pts,
    int32_t _index,
    float _t,
    PathController::InterpolationType _interp) {

    const int32_t n = static_cast<int32_t>(_pts.size());

    switch (_interp) {
    case PathController::InterpolationType::Linear: {
        const Vec3f& p0 = _pts[_index];
        const Vec3f& p1 = _pts[_index + 1];
        return p0 + (p1 - p0) * _t;
    }

    case PathController::InterpolationType::CatmullRom: {
        const Vec3f& p0 = _pts[std::max(0, _index - 1)];
        const Vec3f& p1 = _pts[_index];
        const Vec3f& p2 = _pts[_index + 1];
        const Vec3f& p3 = _pts[std::min(n - 1, _index + 2)];
        return CatmullRomSpline(p0, p1, p2, p3, _t);
    }

    case PathController::InterpolationType::Bezier: {
        // 隣接4点を制御点とする3次ベジェ曲線
        const Vec3f& p0 = _pts[std::max(0, _index - 1)];
        const Vec3f& p1 = _pts[_index];
        const Vec3f& p2 = _pts[_index + 1];
        const Vec3f& p3 = _pts[std::min(n - 1, _index + 2)];
        const float u   = 1.0f - _t;
        return p0 * (u * u * u)
               + p1 * (3.0f * u * u * _t)
               + p2 * (3.0f * u * _t * _t)
               + p3 * (_t * _t * _t);
    }
    }
    return _pts[_index];
}

/// <summary>
/// 現在位置における進行方向ベクトルを計算する（パスの接線方向）
/// </summary>
Vec3f CalcForwardDirection(
    const std::vector<Vec3f>& _pts,
    int32_t _index,
    float _t,
    PathController::InterpolationType _interp) {

    constexpr float kEps = 0.01f;
    const int32_t n      = static_cast<int32_t>(_pts.size());
    const int32_t maxIdx = n - 2;

    float t2     = _t + kEps;
    int32_t idx2 = _index;
    if (t2 > 1.0f) {
        t2 -= 1.0f;
        idx2 = std::min(_index + 1, maxIdx);
    }

    const Vec3f p0  = CalcPosition(_pts, _index, _t, _interp);
    const Vec3f p1  = CalcPosition(_pts, idx2, t2, _interp);
    const Vec3f dir = p1 - p0;

    if (dir.length() < kEpsilon) {
        // フォールバック: セグメント方向
        return (_pts[std::min(_index + 1, n - 1)] - _pts[_index]).normalize();
    }
    return dir.normalize();
}

} // namespace

//-------------------------------------------------------
// システム本体
//-------------------------------------------------------

PathControllerSystem::PathControllerSystem() : ISystem(SystemCategory::Movement) {}

void PathControllerSystem::Initialize() {}
void PathControllerSystem::Finalize() {}

void PathControllerSystem::UpdateEntity(const OriGine::EntityHandle& _handle) {
    Transform* transform           = GetComponent<Transform>(_handle);
    PathController* pathController = GetComponent<PathController>(_handle);

    if (!transform || !pathController) {
        return;
    }
    if (!pathController->IsActive() || !pathController->IsPlaying()) {
        return;
    }

    const auto& pts      = pathController->GetPoints();
    const int32_t n      = static_cast<int32_t>(pts.size());
    const int32_t maxIdx = n - 2; // 有効な区間インデックスの最大値

    if (n < AppConfig::Spline::kMinLinePoints) {
        return;
    }

    // --- 進行度を更新 ---
    const float deltaTime = Engine::GetInstance()->GetDeltaTime();

    // 内部状態をローカルにコピーして更新し、最後に書き戻す
    int32_t currentIndex = pathController->GetCurrentIndex();
    float progress       = pathController->GetProgress();
    bool isReversed      = pathController->IsReversed();
    bool isPlaying       = pathController->IsPlaying();

    // 現在区間の長さを求めて、速度を距離ベースに変換
    const float segmentLength = Vec3f(pts[currentIndex + 1] - pts[currentIndex]).length();
    const float step          = (segmentLength > kEpsilon) ? (pathController->GetSpeed() * deltaTime / segmentLength) : 0.0f;

    if (isReversed) {
        progress -= step;
    } else {
        progress += step;
    }

    // --- 区間オーバーフロー処理 ---
    if (progress >= 1.0f) {
        const float overflow = progress - 1.0f;
        currentIndex++;

        if (currentIndex > maxIdx) {
            // パス終端に到達
            switch (pathController->GetPlayOption()) {
            case PathController::PlayOptions::Once:
                currentIndex = maxIdx;
                progress     = 1.0f;
                isPlaying    = false;
                break;
            case PathController::PlayOptions::Loop:
                currentIndex = 0;
                progress     = overflow;
                break;
            case PathController::PlayOptions::PingPong:
                currentIndex = maxIdx;
                progress     = 1.0f - overflow;
                isReversed   = true;
                break;
            }
        } else {
            progress = overflow;
        }
    }

    // --- 区間アンダーフロー処理（PingPong の逆再生時） ---
    if (progress < 0.0f) {
        const float underflow = -progress;
        currentIndex--;

        if (currentIndex < 0) {
            // パス始端に到達
            switch (pathController->GetPlayOption()) {
            case PathController::PlayOptions::Once:
                currentIndex = 0;
                progress     = 0.0f;
                isPlaying    = false;
                break;
            case PathController::PlayOptions::Loop:
                currentIndex = maxIdx;
                progress     = 1.0f - underflow;
                break;
            case PathController::PlayOptions::PingPong:
                currentIndex = 0;
                progress     = underflow;
                isReversed   = false;
                break;
            }
        } else {
            progress = 1.0f - underflow;
        }
    }

    // 安全クランプ
    currentIndex = std::clamp(currentIndex, 0, maxIdx);
    progress     = std::clamp(progress, 0.0f, 1.0f);

    // 更新した内部状態を書き戻す
    pathController->SetCurrentIndex(currentIndex);
    pathController->SetProgress(progress);
    pathController->SetReversed(isReversed);
    pathController->SetPlaying(isPlaying);

    // --- 座標を計算して Transform に反映 ---
    transform->translate = CalcPosition(
        pts,
        currentIndex,
        progress,
        pathController->GetInterpolation());

    // --- 回転を計算して Transform に反映 ---
    if (pathController->GetRotationMode() != PathController::RotationMode::Fixed) {
        Vec3f forward = CalcForwardDirection(
            pts,
            currentIndex,
            progress,
            pathController->GetInterpolation());

        // 逆再生中は進行方向を反転
        if (isReversed) {
            forward = forward * -1.0f;
        }

        const Quaternion targetRotation = Quaternion::LookAt(forward, axisY);

        if (pathController->GetRotationMode() == PathController::RotationMode::FaceForward) {
            transform->rotate = targetRotation;
        } else { // FaceForwardSmooth
            transform->rotate = SlerpByDeltaTime(
                transform->rotate,
                targetRotation,
                deltaTime,
                pathController->GetRotationSmoothSpeed());
        }
    }
}
