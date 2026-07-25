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

/// <summary>
/// タイヤ痕(スプライン)の制御点を1フレーム分更新する。
/// </summary>
/// <remarks>
/// 次の順序で処理する。順序に依存関係があるため入れ替えないこと。
/// ①生成条件(プレイヤーが接地して動いているか)を評価し、満たさなければフェードアウトへ回す
/// ②スプラインの計算に最低限必要な点数を、現在位置で埋めて確保する
/// ③前回位置から進んだ距離に応じて新しい点を追加する
/// ④点の間隔が均一になるよう分割・統合で整える
/// ⑤capacityを超えた古い点を捨てる
/// ④を②③の後に行うのは、追加直後は間隔がばらついており、そのままメッシュ化すると
/// UVが伸縮して痕の模様が歪むため。
/// </remarks>
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

    // 1.fを足しているのは、この値を後段で「濃さの倍率」として掛けるため。
    // 0始まりだと低ギア時に痕が完全に消えてしまうので、等倍(1.0)を下限にして
    // ギアが上がるほど上乗せされる形にしている
    float speedFactor =
        1.f + std::lerp(spline.GetMinSpeedFactor(), spline.GetMaxSpeedFactor(), gearT);

    // クォータニオンのZ成分を機体のバンク角(横倒しの度合い)の近似として使う。
    // 左右どちらに傾いても痕は出したいので絶対値を取る
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

    // 1フレームで大きく移動した場合は、間を等間隔で埋めるように複数点を打つ。
    // 1点しか打たないと高速移動時に痕が飛び飛びになるため。
    // ただしリスポーンやワープで座標が大きく飛ぶと分割数が爆発するので、kMaxSplitで頭打ちにする
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

/// <summary>
/// 制御点の間隔がsegmentLengthに近づくよう、長すぎる区間は分割し、短すぎる区間は統合する。
/// </summary>
/// <remarks>
/// 間隔がばらついたままメッシュを張ると、区間ごとにUVが引き伸ばされて痕の模様が不均一になり、
/// 短い区間が密集した箇所ではポリゴンが重なってちらつく。等間隔に整えることでこれを防ぐ。
/// 判定に使うkThresholdはsegLenに対する相対値(30%)で、絶対値で比較すると
/// segmentLengthをGUIで変えたときに分割・統合が過敏になったり効かなくなったりするため。
/// </remarks>
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
            // 2点の中点で置き換える。直前にpush_backしたcurrentを上書きする形なので、
            // result側の点数は増えないまま間隔だけが広がる。
            // 条件の後半で末尾から2番目までに限定しているのは、ループを抜けた後に最終点を
            // 無条件でpush_backするため。最終点を巻き込むと同じ点が2回入り痕の終端が潰れる
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

    // スプライン補間には最低4点必要なので、これを下回ったらもう描画できない。
    // 痕は消えきったとみなしてエンティティごと破棄し、放置されたスプラインが溜まるのを防ぐ
    if (_spline.GetPoints().size() < kMinPoints) {
        GetScene()->AddDeleteEntity(_handle);
        return;
    }

    // fadeoutTime間隔で古い点を1つずつ捨てることで、痕が根元から順に消えていく見た目を作る。
    // 一括で消すと痕が瞬間的に消滅して不自然になる
    _spline.GetCommonSettings().fadeoutTimer += deltaTime;
    if (_spline.GetCommonSettings().fadeoutTimer >= _spline.GetCommonSettings().fadeoutTime) {
        _spline.GetCommonSettings().fadeoutTimer = 0.f;
        _spline.GetPoints().pop_front();
    }
}
