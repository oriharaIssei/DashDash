#include "TireTrailGenerateSystem.h"

#include "system/effect/EffectConfig.h"

/// engine
#include "Engine.h"

#include "scene/Scene.h"

/// ECS
// component
#include "component/physics/Rigidbody.h"
#include "component/player/PlayerEffectControlParam.h"
#include "component/player/state/PlayerState.h"
#include "component/transform/Transform.h"

/// math
#include <math/MathEnv.h>
#include <MyEasing.h>

using namespace OriGine;

TireTrailGenerateSystem::TireTrailGenerateSystem() : ISystem(SystemCategory::Movement) {}
TireTrailGenerateSystem::~TireTrailGenerateSystem() {}

void TireTrailGenerateSystem::Initialize() {}
void TireTrailGenerateSystem::Finalize() {}

void TireTrailGenerateSystem::UpdateEntity(const OriGine::EntityHandle& _handle) {
    TireSplinePoints* spline = GetComponent<TireSplinePoints>(_handle);
    if (!spline) {
        return;
    }

    GenerateContext ctx{};
    if (!BuildGenerateContext(*spline, ctx)) {
        UpdateFadeOut(*spline, _handle);
        return;
    }

    EnsureMinimumControlPoints(*spline, ctx);
    AppendNewPoints(*spline, ctx);
    ResamplePoints(*spline);

    // capacity超過分を削除
    while (spline->GetPoints().size() > spline->GetCapacity()) {
        spline->GetPoints().pop_front();
    }

    spline->GetCommonSettings().fadeoutTimer = spline->GetCommonSettings().fadeoutTime;
}

bool TireTrailGenerateSystem::BuildGenerateContext(
    TireSplinePoints& spline,
    GenerateContext& out) {
    auto* player = GetEntity(spline.GetCommonSettings().playerEntityHandle);
    if (!player) {
        spline.GetCommonSettings().playerEntityHandle = EntityHandle();
        return false;
    }

    auto* transform   = GetComponent<OriGine::Transform>(spline.GetCommonSettings().playerEntityHandle);
    auto* state       = GetComponent<PlayerState>(spline.GetCommonSettings().playerEntityHandle);
    auto* effectParam = GetComponent<PlayerEffectControlParam>(spline.GetCommonSettings().playerEntityHandle);
    auto* rigidBody   = GetComponent<OriGine::Rigidbody>(spline.GetCommonSettings().playerEntityHandle);

    // コンポーネントが揃っていない場合は処理しない
    if (!transform || !state || !rigidBody || !effectParam) {
        spline.GetCommonSettings().playerEntityHandle = EntityHandle();
        if (effectParam) {
            // エフェクトパラメータが存在する場合は、スプラインの紐付けを解除する
            effectParam->SetTireTrailSplineEntityId(EntityHandle());
        }
        return false;
    }

    if (!state->IsOnGround() || rigidBody->GetVelocity().lengthSq() < kEpsilon) {
        // 地上にいない、または停止中は処理しない
        // スプラインの紐付けも解除する
        spline.GetCommonSettings().playerEntityHandle = EntityHandle();
        effectParam->SetTireTrailSplineEntityId(EntityHandle());
        return false;
    }

    out.position      = transform->GetWorldTranslate();
    out.segmentLength = spline.GetCommonSettings().segmentLength;

    // ---- alpha 計算 ----
    float gearT =
        static_cast<float>(state->GetGearLevel()) / static_cast<float>(kMaxPlayerGearLevel);

    gearT = EasingFunctions[static_cast<int>(spline.GetSpeedIntensityEaseType())](gearT);

    float speedFactor =
        1.f + std::lerp(spline.GetMinSpeedFactor(), spline.GetMaxSpeedFactor(), gearT);

    float bank = std::abs(transform->rotate[Z]);
    if (bank >= spline.GetThresholdBankAngle()) {
        constexpr float kMaxBankAngle = EffectConfig::TireTrail::kMaxBankAngle;
        float bankT                   = (bank - spline.GetThresholdBankAngle()) / kMaxBankAngle;
        out.alpha +=
            std::lerp(spline.GetMinBankFactor(), spline.GetMaxBankFactor(), bankT) * speedFactor;
    }

    if (state->IsGearUp()) {
        out.alpha += spline.GetGearupFactor() * speedFactor;
    }

    out.alpha = (std::min)(out.alpha, 1.f);

    return true;
}

void TireTrailGenerateSystem::EnsureMinimumControlPoints(
    TireSplinePoints& spline,
    const GenerateContext& ctx) {
    constexpr int32_t kMinPoints = EffectConfig::TireSpline::kMinPoints;

    if (spline.GetPoints().size() >= kMinPoints) {
        return;
    }

    const int32_t need =
        kMinPoints - static_cast<int32_t>(spline.GetPoints().size());

    for (int i = 0; i < need; ++i) {
        spline.PushPoint(ctx.position, spline.GetGroundedFactor());
    }
}

void TireTrailGenerateSystem::AppendNewPoints(
    TireSplinePoints& spline,
    const GenerateContext& ctx) {
    const auto& last = spline.GetPoints().back();
    Vec3f delta      = ctx.position - last.position;
    float dist       = delta.length();

    constexpr float kThreshold = EffectConfig::TireTrail::kPointThreshold;
    if (dist - ctx.segmentLength < kThreshold) {
        return;
    }

    Vec3f dir = Vec3f(0.f, 0.f, 0.f);
    if (dist > kEpsilon) {
        dir = delta / dist;
    }

    constexpr int32_t kMaxSplit = EffectConfig::TireTrail::kMaxSplit;
    int32_t count =
        static_cast<int32_t>(dist / ctx.segmentLength);

    count = (std::min)(count, kMaxSplit);

    for (int i = 1; i <= count; ++i) {
        spline.PushPoint(
            last.position + dir * (ctx.segmentLength * i),
            ctx.alpha);
    }
}

void TireTrailGenerateSystem::ResamplePoints(TireSplinePoints& spline) {
    std::deque<TireSplinePoints::ControlPoint> result;
    const float segLen         = spline.GetCommonSettings().segmentLength;
    constexpr float kThreshold = 0.3f;

    TireSplinePoints::ControlPoint point;

    for (int32_t i = 0; i < static_cast<int32_t>(spline.GetPoints().size()) - 1; ++i) {
        auto& current        = spline.GetPoints()[i];
        auto& next           = spline.GetPoints()[i + 1];
        OriGine::Vec3f delta = next.position - current.position;
        float len            = delta.length();

        // 現在の点は常に残す
        result.push_back(current);

        if (len - segLen > segLen * kThreshold) {
            // 長すぎる → 分割
            int divs = (std::max)(1, static_cast<int>(len / segLen));

            OriGine::Vec3f dir = OriGine::Vec3f(0.0f, 0.0f, 0.0f);
            if (len > kEpsilon) {
                dir = delta / len;
            }

            for (int j = 1; j < divs; ++j) {
                point.position = current.position + dir * (segLen * static_cast<float>(j));
                point.alpha    = std::lerp(current.alpha, next.alpha, static_cast<float>(j) / static_cast<float>(divs));
                result.push_back(point);
            }
        } else if (len - segLen < segLen * kThreshold && (i + 1) < static_cast<int>(spline.GetPoints().size() - 1)) {
            // 短すぎる → 統合
            point.position = (current.position + next.position) * 0.5f;
            point.alpha    = (current.alpha + next.alpha) * 0.5f;
            result.back()  = point;
            ++i; // 次をスキップ（統合済み）
        }
    }

    result.push_back(spline.GetPoints().back());
    spline.GetPoints() = std::move(result);
}

void TireTrailGenerateSystem::UpdateFadeOut(
    TireSplinePoints& _spline,
    const EntityHandle& _handle) {
    constexpr int32_t kMinPoints = 4;
    float deltaTime              = Engine::GetInstance()->GetDeltaTime();

    if (_spline.GetPoints().size() < kMinPoints) {
        GetScene()->AddDeleteEntity(_handle);
        return;
    }

    _spline.GetCommonSettings().fadeoutTimer += deltaTime;
    if (_spline.GetCommonSettings().fadeoutTimer >= _spline.GetCommonSettings().fadeoutTime) {
        _spline.GetCommonSettings().fadeoutTimer = 0.f;
        _spline.GetPoints().pop_front();
    }
}
