#pragma once

#include "component/IComponent.h"

/// stl
#include <vector>

/// math
#include "Vector3.h"

/// <summary>
/// Objectの移動を点によって制御するコンポーネント
/// </summary>
struct PathController
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const PathController& _c);
    friend void from_json(const nlohmann::json& _j, PathController& _c);

public:
    PathController()           = default;
    ~PathController() override = default;
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

public:
    /// <summary>
    /// 再生オプション
    /// </summary>
    enum class PlayOptions {
        Once, // 一度だけ再生
        Loop, // ループ再生
        PingPong, // 行きと帰りを繰り返す
    };

    /// <summary>
    /// 補間方式
    /// </summary>
    enum class InterpolationType {
        Linear, // 線形補間
        Bezier, // ベジェ曲線
        CatmullRom, // なめらかな曲線
    };

    /// <summary>
    /// 回転モード
    /// </summary>
    enum class RotationMode {
        Fixed,             // 回転を変更しない
        FaceForward,       // 進行方向に即時向く
        FaceForwardSmooth, // 進行方向にスムーズに向く
    };

public:
    // 再生設定
    bool isActive        = true; ///< 再生するかどうか
    bool isReversed      = false; ///< 逆再生中かどうか（PingPong時の内部状態）
    bool isPlaying       = false; ///< 再生中かどうか（isActiveとは別に再生状態を管理）
    int32_t startIndex      = 0; ///< 開始位置のインデックス
    float   startProgress   = 0.0f; ///< 開始位置の区間内進行度（0.0〜1.0）
    int32_t currentIndex = 0; ///< 現在の区間の始点インデックス

    float speed    = 1.0f; ///< 再生速度
    float progress = 0.0f; ///< 現在区間内の進行度（0.0〜1.0）

    PlayOptions playOption          = PlayOptions::Loop; ///< 再生オプション
    InterpolationType interpolation = InterpolationType::Linear; ///< 補間方式

    // 回転設定
    RotationMode rotationMode      = RotationMode::Fixed; ///< 回転モード
    float rotationSmoothSpeed      = 5.0f; ///< FaceForwardSmooth 時の補間速度

    // 制御点
    std::vector<OriGine::Vector3f> points; ///< 経由点
};
