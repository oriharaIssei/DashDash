#include "MyEditor.h"

#if defined(_DEBUG) || defined(DEBUG_REPLAY)

/// engine
#define RESOURCE_DIRECTORY
#include <EngineInclude.h>
// scene
#include "scene/SceneJsonRegistry.h"

/// editor
#ifdef DEBUG_REPLAY
#include "editor/debugReplayer/DebugReplayWindow.h"
#endif // DEBUG_REPLAY
#include "editor/EditorController.h"
#ifdef _DEBUG
#include "editor/sceneEditor/SceneEditor.h"
#include "editor/Setting/SettingWindow.h"
#include "editor/window/EditorWindowMenu.h"
#endif // _DEBUG

#include "Engine.h"

using namespace OriGine;

MyEditor::MyEditor() {}
MyEditor::~MyEditor() {}

void MyEditor::Initialize(const std::vector<std::string>& _commandLines) {
    ///=================================================================================================
    // Editor のための 初期化
    ///=================================================================================================
    variables_        = OriGine::GlobalVariables::GetInstance();
    engine_           = OriGine::Engine::GetInstance();
    editorController_ = OriGine::EditorController::GetInstance();

    variables_->LoadAllFile();
    engine_->Initialize();

    RegisterUsingComponents();
    RegisterUsingSystems();

    SceneJsonRegistry::GetInstance()->LoadAllScene(kApplicationResourceDirectory + "/" + kSceneJsonFolder);

    /// コマンドライン引数の処理
    // -s シーン名 : 指定したシーンを開く
    // -rpd : デバッグリプレイヤーモードで起動
    bool isReplayerMode = false;
    auto commandLineItr = _commandLines.begin();
    while (commandLineItr != _commandLines.end()) {
        const std::string& commandLine = *commandLineItr;
        // シーン名の指定
        if (commandLine == "-s") {
            ++commandLineItr;
            if (commandLineItr == _commandLines.end()) {
                break;
            }
            // コマンドライン引数がある場合、シーン名を設定する
            const std::string& sceneName = *commandLineItr;
            LOG_DEBUG("GetCommandLine : {}", sceneName);
        } else if (commandLine == "-rpd") {
            isReplayerMode = true;
        }

        // 次の引数へ
        ++commandLineItr;
    }

#ifdef DEBUG_REPLAY
    if (isReplayerMode) {
        auto debugReplayWindow = std::make_unique<DebugReplayWindow>();
        editorController_->AddEditor<DebugReplayWindow>(std::move(debugReplayWindow));
    }
#endif // DEBUG_REPLAY
#ifdef _DEBUG
    if (!isReplayerMode) {
        auto sceneEditorWindow = std::make_unique<SceneEditorWindow>();
        editorController_->AddEditor<SceneEditorWindow>(std::move(sceneEditorWindow));

        auto settingWindow = std::make_unique<SettingWindow>();
        editorController_->AddEditor<SettingWindow>(std::move(settingWindow));

        auto editorWindowMenu = std::make_unique<EditorWindowMenu>();
        editorWindowMenu->AddMenuItem(std::make_shared<WindowItem<SceneEditorWindow>>());

        editorWindowMenu->AddMenuItem(std::make_shared<WindowItem<SettingWindow>>());
        editorController_->AddMainMenu<EditorWindowMenu>(std::move(editorWindowMenu));
    }
#endif // _DEBUG

    editorController_->Initialize();
}

void MyEditor::Finalize() {
    editorController_->Finalize();

    engine_->Finalize();
}

void MyEditor::Run() {
    while (true) {
        if (engine_->ProcessMessage() || isEndRequest_) {
            break;
        }
        engine_->BeginFrame();

        editorController_->Update();

        // すべてImGuiに描画する
        engine_->ScreenPreDraw();
        engine_->ScreenPostDraw();

        engine_->EndFrame();
    }
}

#endif // _DEBUG || DEBUG_REPLAY
