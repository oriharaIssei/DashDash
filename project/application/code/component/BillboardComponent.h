#pragma once
#include "component/IComponent.h"

/// util
#include "util/EnumBitmask.h"

/// <summary>
/// BillboardTransformシステムの対象になるコンポーネント。
/// ビルボードを適用する軸をビットマスクで保持する。
/// </summary>
struct BillboardComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const BillboardComponent& _c);
    friend void from_json(const nlohmann::json& _j, BillboardComponent& _c);

public:
    BillboardComponent();
    ~BillboardComponent() override;
    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override;

public:
    /// <summary>
    /// ビルボードを適用する軸フラグ
    /// </summary>
    enum class BillboardAxis : int32_t {
        X = 0b1 << 0,
        Y = 0b1 << 1,
        Z = 0b1 << 2,
    };

public:
    /// ビルボードを適用する軸のビットマスク（デフォルト: XYZ すべて）
    EnumBitmask<BillboardAxis> axis =
        static_cast<int32_t>(BillboardAxis::X) | static_cast<int32_t>(BillboardAxis::Y) | static_cast<int32_t>(BillboardAxis::Z);
};
