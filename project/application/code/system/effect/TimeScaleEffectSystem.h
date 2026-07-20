#pragma once

#include "system/ISystem.h"

/// stl
#include <string>
#include <unordered_map>

/// ECS
// entity
#include "entity/EntityHandle.h"

/// <summary>
/// タイムスケールタイマー
/// </summary>
struct TimeScaleTimer {
    float timeScale  = 0.f; // タイムスケール
    float scaleTimer = 0.f; // どれだけ時間を止めるか
};

/// <summary>
/// タイムスケールエフェクトを処理するシステム
/// </summary>
class TimeScaleEffectSystem
    : public OriGine::ISystem {
public:
    TimeScaleEffectSystem();
    ~TimeScaleEffectSystem() override;
    /// <summary>
    /// 初期化処理。各タグのタイムスケールを初期化し、グレースケールエフェクト用Entityを生成する
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

private:
    /// <summary>
    /// TimeScaleEffectComponentを持つEntityの効果時間をタグごとに積算し、対象Entityを削除する
    /// </summary>
    /// <param name="_handle">対象のエンティティハンドル</param>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
    /// <summary>
    /// 毎フレームの更新処理。タイムスケールタイマーの更新に加え、グレースケールエフェクトのフェードイン/アウトを制御する
    /// </summary>
    void Update() override;

    /// <summary>
    /// タイムスケールタイマーの更新
    /// </summary>
    void UpdateScaleTimer();

private:
    float fadeInTime_        = 0.2f; // グレースケールエフェクトのフェードインにかかる時間
    float fadeOutTime_       = 0.6f; // グレースケールエフェクトのフェードアウトにかかる時間
    float effectElapsedTime_ = 0.f; // 効果が有効な間の経過時間（フェードイン計算に使用）
    OriGine::EntityHandle grayScaleEffectEntityHandle_; // グレースケールエフェクト用Entityのハンドル
    std::unordered_map<std::string, TimeScaleTimer> stopTimesByTimeScale_; // タグ名ごとのタイムスケール残り時間
};
