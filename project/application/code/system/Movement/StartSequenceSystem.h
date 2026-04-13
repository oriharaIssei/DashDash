#pragma once
#include "system/ISystem.h"

/// stl
#include <string>
#include <vector>

/// ECS
// component
#include "component/TimerComponent.h"

/// <summary>
/// 開始シーケンスの状態
/// </summary>
enum class StartSequenceStateType {
    Introduction,
    StartTimer,
    GameStart
};

class IStartSequenceState;

/// <summary>
/// ゲームを開始するまでの処理
/// 順序は　ステージ紹介シーケンス -> スタートタイマー -> ゲーム開始
/// </summary>
class StartSequenceSystem
    : public OriGine::ISystem {
public:
    StartSequenceSystem();
    ~StartSequenceSystem();

    void Initialize() override;
    void Finalize() override;

private:
    void UpdateEntity(OriGine::EntityHandle _handle) override;

    /// <summary>
    /// ステージ紹介シーケンス の開始処理
    /// </summary>
    void StageIntroductionSequence();
    /// <summary>
    /// スタートタイマー の開始処理
    /// </summary>
    void TimerStartSequence();
    /// <summary>
    /// ゲーム開始(スタートタイマーが終わった時)の処理
    /// </summary>
    void GameStartSequence();

    /// <summary>
    /// 開始シーケンスの状態を変更する
    /// </summary>
    /// <param name="type"></param>
    /// <param name="handle"></param>
    void ChangeState(
        StartSequenceStateType _type,
        OriGine::EntityHandle _handle);

    /// <summary>
    /// ゲームカメラを無効化する
    /// </summary>
    void StopGameTimer();

    /// <summary>
    /// ゲームカメラを無効化する
    /// </summary>
    void DisableGameCamera();
    /// <summary>
    /// ゲームカメラを有効化する
    /// </summary>
    void EnableGameCamera();

    /// <summary>
    /// ステージ紹介カメラが終了したかどうか
    /// </summary>
    /// <returns></returns>
    bool IsIntroductionCameraEnd();

    /// <summary>
    /// 開始スプライトの表示・非表示設定
    /// </summary>
    /// <param name="_handle"></param>
    /// <param name="_visible"></param>
    void SetStartSpriteVisible(OriGine::EntityHandle _handle, bool _visible);

    /// <summary>
    /// 不要なエンティティのクリーンアップ処理
    /// </summary>
    /// <param name="handle"></param>
    void CleanupEntities(OriGine::EntityHandle handle);

private:
    /// <summary>
    /// 開始シーケンスの状態インターフェース
    /// </summary>
    class IStartSequenceState {
    public:
        virtual ~IStartSequenceState() = default;

        virtual void Enter(StartSequenceSystem& /*_system*/, OriGine::EntityHandle /*_handle*/) {}
        virtual void Update(StartSequenceSystem& _system, OriGine::EntityHandle _handle) = 0;
    };

    /// <summary>
    /// ステージ紹介シーケンス状態
    /// </summary>
    class IntroductionState final
        : public IStartSequenceState {
    public:
        void Enter(StartSequenceSystem& _system, OriGine::EntityHandle _handle) override;
        void Update(StartSequenceSystem& _system, OriGine::EntityHandle _handle) override;
    };

    /// <summary>
    /// スタートタイマー状態
    /// </summary>
    class StartTimerState final
        : public IStartSequenceState {
    public:
        void Update(StartSequenceSystem& system, OriGine::EntityHandle handle) override;
    };

    /// <summary>
    /// ゲーム開始状態
    /// </summary>
    class GameStartState final
        : public IStartSequenceState {
    public:
        void Enter(StartSequenceSystem& _system, OriGine::EntityHandle _handle) override;
        void Update(StartSequenceSystem&, OriGine::EntityHandle) override {}
    };

private:
    bool isStarted_                                   = false;
    bool isInitializeState_                           = false;
    std::vector<std::string> stopSystems_             = {};
    std::vector<std::string> stopSystemsInStartTimer_ = {};

    StartSequenceStateType stateType_                  = StartSequenceStateType::Introduction;
    std::unique_ptr<IStartSequenceState> currentState_ = nullptr;

public:
    bool IsStarted() const {
        return isStarted_;
    }
};
