#pragma once
#include "system/ISystem.h"

class SpeedFor3dUIComponent;

/// <summary>
/// Timerの数字をSpriteとして表示するためにSpriteを生成するシステム
/// </summary>
class CreatePlaneFromSpeed
    : public OriGine::ISystem {
public:
    CreatePlaneFromSpeed();
    ~CreatePlaneFromSpeed() override;
    /// <summary>
    /// 初期化処理（本システムでは特に処理なし）
    /// </summary>
    void Initialize();
    // virtual void Update();
    // virtual void Edit();
    /// <summary>
    /// 終了処理（本システムでは特に処理なし）
    /// </summary>
    void Finalize();

    /// <summary>
    /// SpeedFor3dUIComponentを持つエンティティに対し、数字表示用のPlane群を生成するEntityを作成する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
    /// <summary>
    /// SpeedFor3dUIComponentの設定値から、整数部・小数部の桁数分のPlaneを生成し配置する
    /// </summary>
    /// <param name="_handle">Planeを追加する対象のエンティティハンドル</param>
    /// <param name="_speedFor3dUI">桁数・スケール・マージン等の表示設定を持つコンポーネント</param>
    void CreatePlanesFromComponent(const OriGine::EntityHandle& _handle, SpeedFor3dUIComponent* _speedFor3dUI);
};
