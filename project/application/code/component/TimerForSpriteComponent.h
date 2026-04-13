#pragma once

#include "component/IComponent.h"

/// math
#include <cstdint>
#include <math/Vector2.h>

namespace OriGine {
class Scene;
}

/// <summary>
/// スプライト表示用のタイマーコンポーネント
/// </summary>
struct TimerForSpriteComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const TimerForSpriteComponent& c);
    friend void from_json(const nlohmann::json& j, TimerForSpriteComponent& c);

public:
    TimerForSpriteComponent()           = default;
    ~TimerForSpriteComponent() override = default;

    void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;
    void Finalize() override {}

    /// <summary>
    /// テクスチャ読み込み時の設定
    /// </summary>
    /// <param name="_texIdx"></param>
    void SettingOnLoadTexture(size_t _texIdx);

    /// <summary>
    /// Spriteの再配置
    /// </summary>
    /// <param name="_scene"></param>
    void RelocationSprites(OriGine::Scene* _scene);

private:
    OriGine::EntityHandle spriteEntityHandle_ = OriGine::EntityHandle(); // スプライトを持つエンティティID
public:
    int32_t digitInteger = 0; // 整数部の桁数
    int32_t digitDecimal = 0; // 小数部の桁数

    size_t textureIndex               = 0; // 数字のテクスチャインデックス
    std::string numbersTexturePath    = ""; // 数字のテクスチャ名
    OriGine::Vec2f numbersTextureSize = {0.f, 0.f}; // 数字のテクスチャサイズ
    OriGine::Vec2f numberTileSize     = {0.f, 0.f}; // 数字のタイルサイズ

    OriGine::Vec2f anchorOnWindow = {0.5f, 0.2f}; // ウィンドウ上のアンカー位置
    OriGine::Vec2f offset         = {0.f, 0.f}; // オフセット位置

    // 整数部・小数部のスプライトサイズ
    OriGine::Vec2f spriteSizeInteger = {0.f, 0.f}; // 整数部のスプライトサイズ
    OriGine::Vec2f spriteSizeDecimal = {0.f, 0.f}; // 小数部のスプライトサイズ

    // 整数部・小数部のスプライトマージン
    OriGine::Vec2f spriteMarginInteger = {0.f, 0.f}; // 整数部のスプライトマージン
    OriGine::Vec2f spriteMarginDecimal = {0.f, 0.f}; // 小数部のスプライトマージン

    // 整数部と小数部の間のスペース
    OriGine::Vec2f marginBetweenIntegerAndDecimal = {0.f, 0.f}; // 整数部と小数部の間のスペース

public:
    OriGine::EntityHandle GetSpritesEntityHandle() const { return spriteEntityHandle_; }
    void SetSpritesEntityHandle(OriGine::EntityHandle _handle) { spriteEntityHandle_ = _handle; }
};
