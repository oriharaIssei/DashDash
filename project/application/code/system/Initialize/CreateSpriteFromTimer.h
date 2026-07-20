#pragma once
#include "system/ISystem.h"

class TimerForSpriteComponent;

/// <summary>
/// Timerの数字をSpriteとして表示するためにSpriteを生成するシステム
/// </summary>
class CreateSpriteFromTimer
    : public OriGine::ISystem {
public:
    CreateSpriteFromTimer();
    ~CreateSpriteFromTimer() override;
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
    /// TimerForSpriteComponentを持つエンティティごとに、数字表示用SpriteのEntityを生成する
    /// </summary>
    void UpdateEntity(const OriGine::EntityHandle& _handle) override;
    /// <summary>
    /// SpriteRenderSystemへの登録とSpriteコンポーネントの配置を行う
    /// </summary>
    /// <param name="_handle">Spriteを追加する対象のエンティティハンドル</param>
    /// <param name="_forSpriteComp">表示するタイマー情報を持つコンポーネント</param>
    void CreateSprites(const OriGine::EntityHandle& _handle, TimerForSpriteComponent* _forSpriteComp);
};
