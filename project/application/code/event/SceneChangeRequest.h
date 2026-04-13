#pragma once

/// stl
#include <string>

/// <summary>
/// シーン変更リクエストイベント (SubSceneが使用する)
/// </summary>
struct SceneChangeRequest {
    std::string targetSceneName = ""; ///< 遷移先のシーン名
};
