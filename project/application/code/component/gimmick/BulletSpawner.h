#pragma once

/// engine
// ECS
#include "component/IComponent.h"
// spawn control
#include "component/effect/particle/emitter/Emitter.h"

/// externals
#include <nlohmann/json.hpp>

/// stl
#include <string>

namespace OriGine {

// 前方宣言
class EmitterShapeRenderingSystem;

/// <summary>
/// .ent テンプレートから弾 Entity をスポーンするコンポーネント。
/// スポーンされた Entity の Rigidbody Velocity を、
/// Spawner の Transform の回転で回転させて発射方向を決定する。
/// </summary>
class BulletSpawner
    : public IComponent {
    friend void to_json(nlohmann::json& _j, const BulletSpawner& _comp);
    friend void from_json(const nlohmann::json& _j, BulletSpawner& _comp);

    friend class BulletSpawnerWorkSystem;
    friend class EmitterShapeRenderingSystem;

public:
    BulletSpawner()           = default;
    ~BulletSpawner() override = default;

    void Initialize(Scene* _scene, EntityHandle _entity) override;
    void Finalize() override;
    void Edit(Scene* _scene, EntityHandle _entity, const std::string& _parentLabel) override;

    // ── 再生制御 ──────────────────────────────────────────────

    void PlayStart() { emitter_.PlayStart(); }
    void PlayContinue() { emitter_.PlayContinue(); }
    void PlayStop() { emitter_.PlayStop(); }

private:
    // スポーン制御
    Emitter emitter_;

    // スポーンする .ent テンプレートの型名
    std::string templateTypeName_;

public:
    bool IsActive() const { return emitter_.IsActive(); }
    const std::string& GetTemplateTypeName() const { return templateTypeName_; }
    void SetTemplateTypeName(const std::string& _name) { templateTypeName_ = _name; }
};

void to_json(nlohmann::json& _j, const BulletSpawner& _comp);
void from_json(const nlohmann::json& _j, BulletSpawner& _comp);

} // namespace OriGine
