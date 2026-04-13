#pragma once

#include "component/IComponent.h"

/// math
#include "math/Vector3.h"

/// <summary>
/// 指定したエンティティのTransformに追従するコンポーネント
/// </summary>
class FollowTransformComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const FollowTransformComponent& component);
    friend void from_json(const nlohmann::json& j, FollowTransformComponent& component);

public:
    FollowTransformComponent();
    ~FollowTransformComponent() override;

    void Initialize(OriGine::Scene* scene, OriGine::EntityHandle owner) override;
    void Edit(OriGine::Scene* scene, OriGine::EntityHandle owner, const std::string& parentLabel) override;
    void Finalize() override;

    /// <summary>
    /// 追従対象を設定する
    /// </summary>
    /// <param name="targetHandle">追従対象のエンティティハンドル</param>
    void SetTarget(const OriGine::EntityHandle& targetHandle);

private:
    OriGine::EntityHandle targetHandle_ = OriGine::EntityHandle();

    OriGine::Vec3f offset_ = {0.0f, 0.0f, 0.0f}; // 追従対象からのオフセット

    // 追従が有効かどうか
    bool isActive_ = false;

    // 追従感度 (値が大きいほどピッタリ追従、小さいほど遅延が大きい)
    float sensitivity_ = 10.0f;

public:
    bool IsActive() const { return isActive_; }
    void SetIsActive(bool isActive) { isActive_ = isActive; }

    OriGine::EntityHandle GetTargetHandle() const { return targetHandle_; }
    const OriGine::Vec3f& GetOffset() const { return offset_; }
    void SetOffset(const OriGine::Vec3f& offset) { offset_ = offset; }

    float GetSensitivity() const { return sensitivity_; }
    void SetSensitivity(float sensitivity) { sensitivity_ = sensitivity; }
};
