#pragma once

#include "component/IComponent.h"

/// stl
#include <vector>

/// math
#include "Vector3.h"

/// <summary>
/// Objectの移動を点によって制御するコンポーネント
/// </summary>
class PathController
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const PathController& _c);
    friend void from_json(const nlohmann::json& _j, PathController& _c);

public:
    PathController()           = default;
    ~PathController() override = default;
    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;

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
    bool IsActive() const { return isActive_; }
    void SetActive(bool _isActive) { isActive_ = _isActive; }

    bool IsReversed() const { return isReversed_; }
    void SetReversed(bool _isReversed) { isReversed_ = _isReversed; }

    bool IsPlaying() const { return isPlaying_; }
    void SetPlaying(bool _isPlaying) { isPlaying_ = _isPlaying; }

    int32_t GetStartIndex() const { return startIndex_; }
    void SetStartIndex(int32_t _startIndex) { startIndex_ = _startIndex; }

    float GetStartProgress() const { return startProgress_; }
    void SetStartProgress(float _startProgress) { startProgress_ = _startProgress; }

    int32_t GetCurrentIndex() const { return currentIndex_; }
    void SetCurrentIndex(int32_t _currentIndex) { currentIndex_ = _currentIndex; }

    float GetSpeed() const { return speed_; }
    void SetSpeed(float _speed) { speed_ = _speed; }

    float GetProgress() const { return progress_; }
    void SetProgress(float _progress) { progress_ = _progress; }

    PlayOptions GetPlayOption() const { return playOption_; }
    void SetPlayOption(PlayOptions _playOption) { playOption_ = _playOption; }

    InterpolationType GetInterpolation() const { return interpolation_; }
    void SetInterpolation(InterpolationType _interpolation) { interpolation_ = _interpolation; }

    RotationMode GetRotationMode() const { return rotationMode_; }
    void SetRotationMode(RotationMode _rotationMode) { rotationMode_ = _rotationMode; }

    float GetRotationSmoothSpeed() const { return rotationSmoothSpeed_; }
    void SetRotationSmoothSpeed(float _rotationSmoothSpeed) { rotationSmoothSpeed_ = _rotationSmoothSpeed; }

    const std::vector<OriGine::Vector3f>& GetPoints() const { return points_; }
    std::vector<OriGine::Vector3f>& GetPoints() { return points_; }
    void SetPoints(const std::vector<OriGine::Vector3f>& _points) { points_ = _points; }

private:
    // 再生設定
    bool isActive_       = true; ///< 再生するかどうか
    bool isReversed_     = false; ///< 逆再生中かどうか（PingPong時の内部状態）
    bool isPlaying_      = false; ///< 再生中かどうか（isActiveとは別に再生状態を管理）
    int32_t startIndex_    = 0; ///< 開始位置のインデックス
    float   startProgress_ = 0.0f; ///< 開始位置の区間内進行度（0.0〜1.0）
    int32_t currentIndex_ = 0; ///< 現在の区間の始点インデックス

    float speed_    = 1.0f; ///< 再生速度
    float progress_ = 0.0f; ///< 現在区間内の進行度（0.0〜1.0）

    PlayOptions playOption_          = PlayOptions::Loop; ///< 再生オプション
    InterpolationType interpolation_ = InterpolationType::Linear; ///< 補間方式

    // 回転設定
    RotationMode rotationMode_      = RotationMode::Fixed; ///< 回転モード
    float rotationSmoothSpeed_      = 5.0f; ///< FaceForwardSmooth 時の補間速度

    // 制御点
    std::vector<OriGine::Vector3f> points_; ///< 経由点
};
