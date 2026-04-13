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

    virtual void Initialize(const std::vector<std::string>& _commandLines) = 0;
    virtual void Finalize()                                                = 0;

    /// <summary>
    /// Applicationのメインループ
    /// </summary>
    virtual void Run() = 0;

protected:
    bool isEndRequest_                   = false;
    OriGine::Engine* engine_             = nullptr;
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
