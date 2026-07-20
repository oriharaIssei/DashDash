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
class TimerForSpriteComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const TimerForSpriteComponent& c);
    friend void from_json(const nlohmann::json& j, TimerForSpriteComponent& c);

public:
    TimerForSpriteComponent()           = default;
    ~TimerForSpriteComponent() override = default;

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner) override;
    /// <summary>
    /// エディタ用編集UI
    /// </summary>
    void Edit(OriGine::Scene* _scene, const OriGine::EntityHandle& _owner, const std::string& _parentLabel) override;
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

public:
    OriGine::EntityHandle GetSpritesEntityHandle() const { return spriteEntityHandle_; }
    void SetSpritesEntityHandle(const OriGine::EntityHandle& _handle) { spriteEntityHandle_ = _handle; }

    int32_t GetDigitInteger() const { return digitInteger; }
    void SetDigitInteger(int32_t _digitInteger) { digitInteger = _digitInteger; }

    int32_t GetDigitDecimal() const { return digitDecimal; }
    void SetDigitDecimal(int32_t _digitDecimal) { digitDecimal = _digitDecimal; }

    size_t GetTextureIndex() const { return textureIndex; }
    void SetTextureIndex(size_t _textureIndex) { textureIndex = _textureIndex; }

    const std::string& GetNumbersTexturePath() const { return numbersTexturePath; }
    void SetNumbersTexturePath(const std::string& _numbersTexturePath) { numbersTexturePath = _numbersTexturePath; }

    const OriGine::Vec2f& GetNumbersTextureSize() const { return numbersTextureSize; }
    void SetNumbersTextureSize(const OriGine::Vec2f& _numbersTextureSize) { numbersTextureSize = _numbersTextureSize; }

    const OriGine::Vec2f& GetNumberTileSize() const { return numberTileSize; }
    void SetNumberTileSize(const OriGine::Vec2f& _numberTileSize) { numberTileSize = _numberTileSize; }

    const OriGine::Vec2f& GetAnchorOnWindow() const { return anchorOnWindow; }
    void SetAnchorOnWindow(const OriGine::Vec2f& _anchorOnWindow) { anchorOnWindow = _anchorOnWindow; }

    const OriGine::Vec2f& GetOffset() const { return offset; }
    void SetOffset(const OriGine::Vec2f& _offset) { offset = _offset; }

    const OriGine::Vec2f& GetSpriteSizeInteger() const { return spriteSizeInteger; }
    void SetSpriteSizeInteger(const OriGine::Vec2f& _spriteSizeInteger) { spriteSizeInteger = _spriteSizeInteger; }

    const OriGine::Vec2f& GetSpriteSizeDecimal() const { return spriteSizeDecimal; }
    void SetSpriteSizeDecimal(const OriGine::Vec2f& _spriteSizeDecimal) { spriteSizeDecimal = _spriteSizeDecimal; }

    const OriGine::Vec2f& GetSpriteMarginInteger() const { return spriteMarginInteger; }
    void SetSpriteMarginInteger(const OriGine::Vec2f& _spriteMarginInteger) { spriteMarginInteger = _spriteMarginInteger; }

    const OriGine::Vec2f& GetSpriteMarginDecimal() const { return spriteMarginDecimal; }
    void SetSpriteMarginDecimal(const OriGine::Vec2f& _spriteMarginDecimal) { spriteMarginDecimal = _spriteMarginDecimal; }

    const OriGine::Vec2f& GetMarginBetweenIntegerAndDecimal() const { return marginBetweenIntegerAndDecimal; }
    void SetMarginBetweenIntegerAndDecimal(const OriGine::Vec2f& _marginBetweenIntegerAndDecimal) { marginBetweenIntegerAndDecimal = _marginBetweenIntegerAndDecimal; }

private:
    OriGine::EntityHandle spriteEntityHandle_ = OriGine::EntityHandle(); // スプライトを持つエンティティID

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
};
