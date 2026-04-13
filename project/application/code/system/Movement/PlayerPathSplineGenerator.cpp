#include "PlayerPathSplineGenerator.h"

/// engine
#include "Engine.h"

// component
#include "component/physics/Rigidbody.h"
#include "component/player/state/PlayerState.h"
#include "component/spline/SplinePoints.h"
#include "component/transform/Transform.h"

/// math
#include <math/MathEnv.h>

using namespace OriGine;

PlayerPathSplineGenerator::PlayerPathSplineGenerator() : ISystem(SystemCategory::Movement) {}
PlayerPathSplineGenerator::~PlayerPathSplineGenerator() {}

void PlayerPathSplineGenerator::Initialize() {}
void PlayerPathSplineGenerator::Finalize() {}

bool PlayerPathSplineGenerator::TryGetContext(EntityHandle _handle, Context& _outContext) {
    _outContext.splinePoints = GetComponent<SplinePoints>(_handle);
    if (!_outContext.splinePoints) {
        return false;
    }

    auto playerEntityHandle     = GetUniqueEntity("Player");
    _outContext.playerTransform = GetComponent<Transform>(playerEntityHandle);
    if (!_outContext.playerTransform) {
        return false;
    }

    _outContext.playerRigidBody = GetComponent<Rigidbody>(playerEntityHandle);
    if (!_outContext.playerRigidBody) {
        return false;
    }

    return true;
}

void PlayerPathSplineGenerator::ProcessMovement(Context& _ctx) {
    SplinePoints* splinePoints = _ctx.splinePoints;

    // 設定値の安全チェック
    if (splinePoints->commonSettings.segmentLength <= kEpsilon) {
        return;
    }

    // 1. 新しい位置へ点を追加
    Vec3f playerPos = _ctx.playerTransform->GetWorldTranslate();
    AppendNewPoints(splinePoints, playerPos);

    // 2. パス全体の再構築（分割・統合処理）
    RefineSplinePoints(splinePoints);

    // 3. 容量制限
    while (splinePoints->points.size() > splinePoints->capacity) {
        splinePoints->points.pop_front();
    }

    // 4. フェードアウトタイマーのリセット
    splinePoints->commonSettings.fadeoutTimer = splinePoints->commonSettings.fadeoutTime;
}

void PlayerPathSplineGenerator::AppendNewPoints(SplinePoints* _splinePoints, const Vec3f& _targetPos) {
    // 最低4点確保
    if (_splinePoints->points.size() < 4) {
        int32_t diff = 4 - static_cast<int32_t>(_splinePoints->points.size());
        for (int32_t i = 0; i < diff; ++i) {
            _splinePoints->PushPoint(_targetPos);
        }
    }

    const float segLen     = _splinePoints->commonSettings.segmentLength;
    const Vec3f& lastPoint = _splinePoints->points.back();
    Vec3f distanceVec      = _targetPos - lastPoint;
    float distLen          = distanceVec.length();

    constexpr float kThresholdSplitOrMerger = 0.3f;

    // 距離が離れていたら補間して点を追加
    if (distLen - segLen >= kThresholdSplitOrMerger) {
        Vec3f direction = distanceVec.normalize();

        // あまりにも離れている場合は制限をかける
        constexpr int32_t kSkipThreshold = 18;
        int32_t segmentsToAdd            = static_cast<int32_t>(distLen / segLen);
        segmentsToAdd                    = (std::min)(segmentsToAdd, kSkipThreshold);

        for (int32_t i = 1; i <= segmentsToAdd; ++i) {
            Vec3f dividedPoint = lastPoint + direction * (segLen * static_cast<float>(i));
            _splinePoints->PushPoint(dividedPoint);
        }
    }
}

void PlayerPathSplineGenerator::RefineSplinePoints(SplinePoints* _splinePoints) {
    if (_splinePoints->points.empty()) {
        return;
    }

    std::deque<Vec3f> newPoints;
    const float segLen                      = _splinePoints->commonSettings.segmentLength;
    constexpr float kThresholdSplitOrMerger = 0.3f;

    // 既存のポイントを走査して再構築
    for (int32_t i = 0; i < static_cast<int32_t>(_splinePoints->points.size()) - 1; ++i) {
        Vec3f current = _splinePoints->points[i];
        Vec3f next    = _splinePoints->points[i + 1];
        Vec3f delta   = next - current;
        float len     = delta.length();

        // 現在の点は常に残す
        newPoints.push_back(current);

        if (len - segLen > segLen * kThresholdSplitOrMerger) {
            // 長すぎる → 分割して補間点を追加
            int divs  = (std::max)(1, static_cast<int>(len / segLen));
            Vec3f dir = delta.normalize();

            for (int j = 1; j < divs; ++j) {
                newPoints.push_back(current + dir * (segLen * static_cast<float>(j)));
            }
        } else if (len - segLen < segLen * kThresholdSplitOrMerger && (i + 1) < static_cast<int>(_splinePoints->points.size() - 1)) {
            // 短すぎる → 統合（次の点を飛ばして平均位置にするなど）
            // 元コードのロジック: 現在点と次点の平均で現在点を更新し、次点をスキップ
            Vec3f merged     = (current + next) * 0.5f;
            newPoints.back() = merged;
            ++i; // 次の点をスキップ
        }
    }

    // 最後の点を追加
    newPoints.push_back(_splinePoints->points.back());

    // リストを差し替え
    _splinePoints->points = std::move(newPoints);
}

void PlayerPathSplineGenerator::ProcessIdle(Context& _ctx) {
    SplinePoints* splinePoints = _ctx.splinePoints;

    if (!splinePoints->points.empty()) {
        float deltaTime = Engine::GetInstance()->GetDeltaTimer()->GetScaledDeltaTime("World");
        splinePoints->commonSettings.fadeoutTimer += deltaTime;

        if (splinePoints->commonSettings.fadeoutTimer >= splinePoints->commonSettings.fadeoutTime) {
            splinePoints->commonSettings.fadeoutTimer = 0.0f;
            splinePoints->points.pop_front();
        }
    }
}

void PlayerPathSplineGenerator::UpdateEntity(EntityHandle _handle) {
    Context ctx;
    if (!TryGetContext(_handle, ctx)) {
        return;
    }

    // 速度判定定数
    constexpr float kFadeOutThresholdSpeed = 8.f;
    float speedSq                          = ctx.playerRigidBody->GetVelocity().lengthSq();

    // 移動中か停止中かで分岐
    if (speedSq > kFadeOutThresholdSpeed * kFadeOutThresholdSpeed) {
        ProcessMovement(ctx);
    } else {
        ProcessIdle(ctx);
    }
}
