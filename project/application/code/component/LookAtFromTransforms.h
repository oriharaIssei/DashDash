#pragma once
#include "component/IComponent.h"

/// util
#include "util/EnumBitmask.h"

/// <summary>
/// 2つの Transformコンポーネントを参照し、1つ目のTransformの位置から2つ目のTransformの位置を向くように設定するコンポーネント
/// </summary>
struct LookAtFromTransforms
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const LookAtFromTransforms& _c);
    friend void from_json(const nlohmann::json& _j, LookAtFromTransforms& _c);

public:
    LookAtFromTransforms();
    ~LookAtFromTransforms() override;
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

public:
    enum class RotateAxis : int32_t {
        X = 0b1 << 0,
        Y = 0b1 << 1,
        Z = 0b1 << 2
    };

public:
    EnumBitmask<LookAtFromTransforms::RotateAxis> rotateAxis = 0; // 回転を許可する軸のビットマスク

    OriGine::ComponentHandle fromTransformComp = {}; // 視点のTransformコンポーネント
    OriGine::ComponentHandle toTransformComp   = {}; // 注視点のTransformコンポーネント
};
