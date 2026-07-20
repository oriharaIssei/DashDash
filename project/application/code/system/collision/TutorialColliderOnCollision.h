#pragma once
#include "system/ISystem.h"

/// <summary>
/// チュートリアル用の衝突反応システム
/// </summary>
class TutorialColliderOnCollision
    : public OriGine::ISystem {
public:
    TutorialColliderOnCollision();
    ~TutorialColliderOnCollision() override;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize() override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// PlayerのEntityHandleを毎フレーム取得してから基底のUpdateを呼ぶ
    /// </summary>
    void Update() override;

protected:
    /// <summary>
    /// Playerとの衝突を検知し、衝突開始/終了に応じてSpriteのアニメーションを再生する
    /// </summary>
    /// <param name="_handle">チュートリアル用コライダーを持つエンティティハンドル</param>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

protected:
    OriGine::EntityHandle playerEntityHandle_ = OriGine::EntityHandle(); // 判定対象となるPlayerのエンティティハンドル
};
