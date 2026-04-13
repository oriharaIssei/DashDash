#include "PathControllerSystem.h"

/// stl
#include <algorithm>

/// engine
#include "Engine.h"

/// ECS
// component
#include "component/gimmick/PathController.h"
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

void PathControllerSystem::UpdateEntity(OriGine::EntityHandle _handle) {
    Transform* transform           = GetComponent<Transform>(_handle);
    PathController* pathController = GetComponent<PathController>(_handle);

    if (!transform || !pathController) {
        return;
    }
    if (!pathController->isActive || !pathController->isPlaying) {
        return;
    }

    const auto& pts      = pathController->points;
    const int32_t n      = static_cast<int32_t>(pts.size());
    const int32_t maxIdx = n - 2; // 有効な区間インデックスの最大値

    if (n < 2) {
        return;
    }

    // --- 進行度を更新 ---
    const float deltaTime = Engine::GetInstance()->GetDeltaTime();

    // 現在区間の長さを求めて、速度を距離ベースに変換
    const float segmentLength = Vec3f(pts[pathController->currentIndex + 1] - pts[pathController->currentIndex]).length();
    const float step          = (segmentLength > kEpsilon) ? (pathController->speed * deltaTime / segmentLength) : 0.0f;

    if (pathController->isReversed) {
        pathController->progress -= step;
    } else {
        pathController->progress += step;
    }

    // --- 区間オーバーフロー処理 ---
    if (pathController->progress >= 1.0f) {
        const float overflow = pathController->progress - 1.0f;
        pathController->currentIndex++;

        if (pathController->currentIndex > maxIdx) {
            // パス終端に到達
            switch (pathController->playOption) {
            case PathController::PlayOptions::Once:
                pathController->currentIndex = maxIdx;
                pathController->progress     = 1.0f;
                pathController->isPlaying    = false;
                break;
            case PathController::PlayOptions::Loop:
                pathController->currentIndex = 0;
                pathController->progress     = overflow;
                break;
            case PathController::PlayOptions::PingPong:
                pathController->currentIndex = maxIdx;
                pathController->progress     = 1.0f - overflow;
                pathController->isReversed   = true;
                break;
            }
        } else {
            pathController->progress = overflow;
        }
    }

    // --- 区間アンダーフロー処理（PingPong の逆再生時） ---
    if (pathController->progress < 0.0f) {
        const float underflow = -pathController->progress;
        pathController->currentIndex--;

        if (pathController->currentIndex < 0) {
            // パス始端に到達
            switch (pathController->playOption) {
            case PathController::PlayOptions::Once:
                pathController->currentIndex = 0;
                pathController->progress     = 0.0f;
                pathController->isPlaying    = false;
                break;
            case PathController::PlayOptions::Loop:
                pathController->currentIndex = maxIdx;
                pathController->progress     = 1.0f - underflow;
                break;
            case PathController::PlayOptions::PingPong:
                pathController->currentIndex = 0;
                pathController->progress     = underflow;
                pathController->isReversed   = false;
                break;
            }
        } else {
            pathController->progress = 1.0f - underflow;
        }
    }

    // 安全クランプ
    pathController->currentIndex = std::clamp(pathController->currentIndex, 0, maxIdx);
    pathController->progress     = std::clamp(pathController->progress, 0.0f, 1.0f);

    // --- 座標を計算して Transform に反映 ---
    transform->translate = CalcPosition(
        pts,
        pathController->currentIndex,
        pathController->progress,
        pathController->interpolation);

    // --- 回転を計算して Transform に反映 ---
    if (pathController->rotationMode != PathController::RotationMode::Fixed) {
        Vec3f forward = CalcForwardDirection(
            pts,
            pathController->currentIndex,
            pathController->progress,
            pathController->interpolation);

        // 逆再生中は進行方向を反転
        if (pathController->isReversed) {
            forward = forward * -1.0f;
        }

        const Quaternion targetRotation = Quaternion::LookAt(forward, axisY);

        if (pathController->rotationMode == PathController::RotationMode::FaceForward) {
            transform->rotate = targetRotation;
        } else { // FaceForwardSmooth
            transform->rotate = SlerpByDeltaTime(
                transform->rotate,
                targetRotation,
                deltaTime,
                pathController->rotationSmoothSpeed);
        }
    }
}
