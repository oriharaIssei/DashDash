#pragma once

#include "component/IComponent.h"

/// <summary>
/// 障害物コンポーネント
/// </summary>
class Obstacle
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const Obstacle& _c);
    friend void from_json(const nlohmann::json& _j, Obstacle& _c);

public:
    Obstacle()           = default;
    ~Obstacle() override = default;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Finalize() override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

private:
    float penaltyTime_                  = 1.5f;
    float invincibilityTimeOnCollision_ = 1.0f; // 障害物と衝突した際の無敵時間 /sec

public:
    float GetPenaltyTime() const { return penaltyTime_; }
    float GetInvincibilityTimeOnCollision() const { return invincibilityTimeOnCollision_; }
};
