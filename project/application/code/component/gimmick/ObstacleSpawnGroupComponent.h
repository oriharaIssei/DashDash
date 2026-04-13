#pragma once

/// stl
#include <vector>

/// engine
#include "component/IComponent.h"
#include "entity/EntityHandle.h"

/// <summary>
/// Obstacleコンポーネントを持つEntityのハンドルを複数管理するコンポーネント。
/// Eventによる障害物の一括出現に使用する。
/// </summary>
class ObstacleSpawnGroupComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const ObstacleSpawnGroupComponent& _c);
    friend void from_json(const nlohmann::json& _j, ObstacleSpawnGroupComponent& _c);

public:
    ObstacleSpawnGroupComponent()           = default;
    ~ObstacleSpawnGroupComponent() override = default;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

private:
    bool isTriggered_ = false;
    std::vector<OriGine::EntityHandle> obstacleHandles_;

public:
    bool IsTriggered() const { return isTriggered_; }
    void SetTriggered(bool _isTriggered) { isTriggered_ = _isTriggered; }

    const std::vector<OriGine::EntityHandle>& GetObstacleHandles() const { return obstacleHandles_; }
};

inline void to_json(nlohmann::json& _j, const ObstacleSpawnGroupComponent& _c) {
    _j["obstacleHandles"] = _c.obstacleHandles_;
}

inline void from_json(const nlohmann::json& _j, ObstacleSpawnGroupComponent& _c) {
    _j.at("obstacleHandles").get_to(_c.obstacleHandles_);
}
