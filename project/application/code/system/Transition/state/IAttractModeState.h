#pragma once

/// <summary>
/// アトラクションモードの状態インターフェース
/// </summary>
class IAttractModeState {
public:
    virtual ~IAttractModeState() = default;

    /// <summary>
    /// この状態に遷移した際に一度だけ呼ばれる初期化処理。
    /// </summary>
    virtual void Enter() {}
    /// <summary>
    /// 毎フレーム呼ばれる状態の更新処理。
    /// </summary>
    /// <param name="_deltaTime">前フレームからの経過時間</param>
    virtual void Update(float _deltaTime) = 0;

    /// <summary>
    /// アトラクションモードが有効(入力待ち演出中)かどうか。
    /// </summary>
    virtual bool IsActive() const             = 0;
    /// <summary>
    /// この状態の間、対象を描画すべきかどうか。
    /// </summary>
    virtual bool ShouldRender() const         = 0;
    /// <summary>
    /// 遷移条件を満たしていれば次の状態を返す。満たしていなければ nullptr を返す。
    /// </summary>
    /// <returns>遷移先の状態。遷移しない場合は nullptr</returns>
    virtual IAttractModeState* Transition()   = 0;
};
