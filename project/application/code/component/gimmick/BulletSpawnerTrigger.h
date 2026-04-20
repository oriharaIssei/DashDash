#pragma once

#include "component/gimmick/ICollisionTriggerComponent.h"

/// <summary>
/// 衝突をトリガーにして、指定した Entity の BulletSpawner を
/// アクティブ化 または 非アクティブ化 するコンポーネント。
/// </summary>
class BulletSpawnerTrigger
    : public ICollisionTriggerComponent {
    friend void to_json(nlohmann::json& _j, const BulletSpawnerTrigger& _c);
    friend void from_json(const nlohmann::json& _j, BulletSpawnerTrigger& _c);

public:
    BulletSpawnerTrigger()           = default;
    ~BulletSpawnerTrigger() override = default;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
};

inline void to_json(nlohmann::json& _j, const BulletSpawnerTrigger& _c) {
    _j["mode"]          = _c.mode_;
    _j["targetHandles"] = _c.targetHandles_;
}

inline void from_json(const nlohmann::json& _j, BulletSpawnerTrigger& _c) {
    _j.at("mode").get_to(_c.mode_);
    _j.at("targetHandles").get_to(_c.targetHandles_);
}
