#pragma once
#include "FrameWork.h"
#if defined(_DEBUG) || defined(DEBUG_REPLAY)

namespace OriGine {
class EditorController;
}

/// <summary>
/// 編集用のアプリケーションフレームワーク
/// </summary>
class MyEditor
    : public FrameWork {
public:
    MyEditor();
    virtual ~MyEditor();

    /// <summary>
    /// 初期化処理
    /// </summary>
    /// <param name="_commandLines">コマンドライン引数</param>
    virtual void Initialize(const std::vector<std::string>& _commandLines) override;
    /// <summary>
    /// 終了処理
    /// </summary>
    virtual void Finalize() override;
    /// <summary>
    /// メインループの実行
    /// </summary>
    virtual void Run() override;

protected:
    bool isEndRequest_                           = false;
    OriGine::EditorController* editorController_ = nullptr;

public:
    bool isEndRequest() const { return isEndRequest_; }
    void SetEndRequest(bool endRequest) { isEndRequest_ = endRequest; }
};

#endif // _DEBUG || DEBUG_REPLAY
