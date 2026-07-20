#pragma once

#include "system/ISystem.h"

/// <summary>
/// ボタン入力を処理するシステム
/// </summary>
class ButtonInputSystem
    : public OriGine::ISystem {
public:
    ButtonInputSystem() : ISystem(OriGine::SystemCategory::Input) {}
    ~ButtonInputSystem() {}

    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    virtual void Initialize();
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    virtual void Finalize();

protected:
    /// <summary>
    /// キーボード・パッド・マウスの入力を判定し、Buttonコンポーネントの押下/離す/ホバー状態を更新する
    /// </summary>
    /// <param name="_handle">対象エンティティのハンドル</param>
    virtual void UpdateEntity(const OriGine::EntityHandle& _handle);
};
