#pragma once

/// parent
#include "FrameWork.h"

/// stl
#include <memory>

/// Engine
#include <debugReplayer/ReplayRecorder.h>
#include <scene/SceneManager.h>

/// <summary>
/// ゲーム本体
/// </summary>
class MyGame
    : public FrameWork {
public:
    MyGame();
    ~MyGame();

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="_commandLines">コマンドライン引数</param>
    void Initialize(const std::vector<std::string>& _commandLines) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// メインループの実行
    /// </summary>
    void Run();

private:
    // シーン全体の管理
    std::unique_ptr<OriGine::SceneManager> sceneManager_ = nullptr;

#ifdef _DEVELOP
    // リプレイ録画中かどうか
    bool isRecording_                                  = false;
    // 入力リプレイの録画・再生を担う
    std::unique_ptr<OriGine::ReplayRecorder> recorder_ = nullptr;
#endif // _DEVELOP
};
