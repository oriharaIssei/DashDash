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
    std::unique_ptr<OriGine::SceneManager> sceneManager_ = nullptr;

#ifdef _DEVELOP
    bool isRecording_                                  = false;
    std::unique_ptr<OriGine::ReplayRecorder> recorder_ = nullptr;
#endif // _DEVELOP
};
