#pragma once
#include "component/IComponent.h"

/// math
#include <math/Vector2.h>
#include <math/Vector3.h>

/// <summary>
/// 速度を 3D UI（Plane）で表示するための設定コンポーネント
/// </summary>
class SpeedFor3dUIComponent
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& j, const SpeedFor3dUIComponent& c);
    friend void from_json(const nlohmann::json& j, SpeedFor3dUIComponent& c);

public:
    SpeedFor3dUIComponent()           = default;
    ~SpeedFor3dUIComponent() override = default;

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
    /// 数字テクスチャを読み込んだ際の設定
    /// </summary>
    void SettingOnLoadTexture(size_t _texIdx);

public:
    OriGine::EntityHandle GetPlaneEntityHandle() const { return planeEntityHandle; }
    void SetPlaneEntityHandle(OriGine::EntityHandle id) { planeEntityHandle = id; }

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

    const OriGine::Vec3f& GetWorldPosition() const { return worldPosition; }
    void SetWorldPosition(const OriGine::Vec3f& _worldPosition) { worldPosition = _worldPosition; }

    const OriGine::Vec3f& GetOffset() const { return offset; }
    void SetOffset(const OriGine::Vec3f& _offset) { offset = _offset; }

    const OriGine::Vec2f& GetPlaneScaleInteger() const { return planeScaleInteger; }
    void SetPlaneScaleInteger(const OriGine::Vec2f& _planeScaleInteger) { planeScaleInteger = _planeScaleInteger; }

    const OriGine::Vec2f& GetPlaneScaleDecimal() const { return planeScaleDecimal; }
    void SetPlaneScaleDecimal(const OriGine::Vec2f& _planeScaleDecimal) { planeScaleDecimal = _planeScaleDecimal; }

    const OriGine::Vec2f& GetPlaneMarginInteger() const { return planeMarginInteger; }
    void SetPlaneMarginInteger(const OriGine::Vec2f& _planeMarginInteger) { planeMarginInteger = _planeMarginInteger; }

    const OriGine::Vec2f& GetPlaneMarginDecimal() const { return planeMarginDecimal; }
    void SetPlaneMarginDecimal(const OriGine::Vec2f& _planeMarginDecimal) { planeMarginDecimal = _planeMarginDecimal; }

    const OriGine::Vec2f& GetMarginBetweenIntegerAndDecimal() const { return marginBetweenIntegerAndDecimal; }
    void SetMarginBetweenIntegerAndDecimal(const OriGine::Vec2f& _marginBetweenIntegerAndDecimal) { marginBetweenIntegerAndDecimal = _marginBetweenIntegerAndDecimal; }

private:
    OriGine::EntityHandle planeEntityHandle = OriGine::EntityHandle(); // 速度表示PlaneのエンティティID

    //
    // --- 数値表示パラメータ ---
    //
    int32_t digitInteger = 0; // 整数部の桁数
    int32_t digitDecimal = 0; // 小数部の桁数

    //
    // --- テクスチャ設定 ---
    //
    size_t textureIndex               = 0; // 数字テクスチャのインデックス
    std::string numbersTexturePath    = ""; // 数字テクスチャファイルパス
    OriGine::Vec2f numbersTextureSize = {0.f, 0.f}; // テクスチャ全体サイズ
    OriGine::Vec2f numberTileSize     = {0.f, 0.f}; // 各数字のタイルサイズ（0～9）

    //
    // --- 表示位置・オフセット ---
    //
    OriGine::Vec3f worldPosition = {0.f, 0.f, 0.f}; // 3D空間上の表示座標
    OriGine::Vec3f offset        = {0.f, 0.f, 0.f}; // Planeの相対移動量

    //
    // --- Plane スプライトのサイズ設定 ---
    //
    OriGine::Vec2f planeScaleInteger = {0.f, 0.f}; // 整数部のPlaneScale
    OriGine::Vec2f planeScaleDecimal = {0.f, 0.f}; // 小数部のPlaneScale

    //
    // --- 桁ごとのマージン（左右の間隔） ---
    //
    OriGine::Vec2f planeMarginInteger = {0.f, 0.f}; // 整数部のマージン
    OriGine::Vec2f planeMarginDecimal = {0.f, 0.f}; // 小数部のマージン

    // 整数と小数の間のスペース
    OriGine::Vec2f marginBetweenIntegerAndDecimal = {0.f, 0.f};
};
