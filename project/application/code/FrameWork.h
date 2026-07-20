#pragma once

/// stl
#include <string>
#include <vector>

namespace OriGine {
class Engine;
class GlobalVariables;
}

/// <summary>
/// Applicationの土台となる フレームワーク クラス
/// </summary>
/// <remarks>
/// 各アプリケーションはこのクラスを継承して実装する
/// </remarks>
class FrameWork {
public:
    FrameWork();
    virtual ~FrameWork();

    /// <summary>
    /// アプリケーションの初期化処理（派生クラスで実装）
    /// </summary>
    /// <param name="_commandLines">コマンドライン引数</param>
    virtual void Initialize(const std::vector<std::string>& _commandLines) = 0;
    /// <summary>
    /// アプリケーションの終了処理（派生クラスで実装）
    /// </summary>
    virtual void Finalize()                                                = 0;

    /// <summary>
    /// Applicationのメインループ
    /// </summary>
    virtual void Run() = 0;

protected:
    // アプリケーション終了要求フラグ
    bool isEndRequest_                   = false;
    // エンジン本体への参照
    OriGine::Engine* engine_             = nullptr;
    // グローバル変数管理への参照
    OriGine::GlobalVariables* variables_ = nullptr;
};

/// <summary>
/// 使用される コンポーネントを登録する
/// </summary>
void RegisterUsingComponents();
/// <summary>
/// 使用される System を登録する
/// </summary>
void RegisterUsingSystems();
