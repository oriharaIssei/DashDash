#pragma once

/// stl
#include <vector>

/// engine
#include "component/IComponent.h"
#include "entity/EntityHandle.h"

/// <summary>
/// 衝突をトリガーにして、指定した Entity の BulletSpawner を
/// アクティブ化 または 非アクティブ化 するコンポーネント。
/// </summary>
class BulletSpawnerTrigger
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const BulletSpawnerTrigger& _c);
    friend void from_json(const nlohmann::json& _j, BulletSpawnerTrigger& _c);

public:
    /// <summary>
    /// トリガー発火時の動作モード
    /// </summary>
    enum class Mode {
        Activate,   // BulletSpawner を PlayStart する
        Deactivate, // BulletSpawner を PlayStop する
    };

public:
    BulletSpawnerTrigger()           = default;
    ~BulletSpawnerTrigger() override = default;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

private:
    Mode mode_ = Mode::Activate;
    std::vector<OriGine::EntityHandle> targetHandles_;

public:
    Mode GetMode() const { return mode_; }
    void SetMode(Mode _mode) { mode_ = _mode; }

    const std::vector<OriGine::EntityHandle>& GetTargetHandles() const { return targetHandles_; }
};

static const char* kBulletSpawnerTriggerModeNames[] = {
    "Activate",
    "Deactivate",
};

inline void to_json(nlohmann::json& _j, const BulletSpawnerTrigger& _c) {
    _j["mode"]          = _c.mode_;
    _j["targetHandles"] = _c.targetHandles_;
}

inline void from_json(const nlohmann::json& _j, BulletSpawnerTrigger& _c) {
    _j.at("mode").get_to(_c.mode_);
    _j.at("targetHandles").get_to(_c.targetHandles_);
}
