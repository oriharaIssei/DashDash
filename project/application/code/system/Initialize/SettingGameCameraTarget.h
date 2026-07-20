#pragma once

/// parent
#include "system/ISystem.h"

/// <summary>
/// ゲームカメラのターゲット設定を行うシステム
/// </summary>
class SettingGameCameraTarget
    : public OriGine::ISystem {
public:
    SettingGameCameraTarget() : ISystem(OriGine::SystemCategory::Initialize) {}
    ~SettingGameCameraTarget() = default;

    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// GameCameraの追従ターゲットをPlayerに設定し、初期位置・PlayerStateへのカメラハンドルを反映する
    /// </summary>
    void Update() override;

    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize() override;
};
