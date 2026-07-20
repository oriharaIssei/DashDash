#pragma once
#include "system/ISystem.h"

/// <summary>
/// プレイヤーの状態が指定された状態になるまで 時間を止めるシステム
/// </summary>
class PlayerStateOverrideSystem
    : public OriGine::ISystem {
public:
    PlayerStateOverrideSystem();
    ~PlayerStateOverrideSystem() override;
    void Initialize() override;
    void Finalize() override;

protected:
    /// <summary>
    /// UpdateEntityの結果(isTimeScaled_)に応じて、Player/Effect/Cameraのタイムスケールを
    /// 通常速度またはスロー速度に切り替える。
    /// </summary>
    void Update() override;
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;

private:
    // 今フレーム、状態不一致によりスロー処理を適用すべきか(Updateの1周期内でUpdateEntityから設定される)
    bool isTimeScaled_ = false;
};
