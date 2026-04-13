#pragma once

#include "component/IComponent.h"

/// stl
#include <string>

/// DirectX12
#include "directX12/buffer/AppendBuffer.h"
#include "directX12/buffer/IConstantBuffer.h"

/// math
#include "math/Vector2.h"

/// <summary>
/// 点群配置をプロシージャルで設定するためのパラメータ構造体
/// </summary>
struct PointPlacementParamsData {
    uint32_t seed = 0;

    float collisionRadius = 0.f;

    float densityThreshold = 0.f;
    float volumeThreshold  = 0.f;
    float maskThreshold    = 0.f;

    uint32_t placementResolution = 0;
    uint32_t sampleCount         = 0;

    OriGine::Vec2f worldMinXZ = OriGine::Vec2f(0.f, 0.f);
    OriGine::Vec2f worldMaxXZ = OriGine::Vec2f(0.f, 0.f);

    /// <summary>
    /// 定数バッファ用の構造体
    /// </summary>
    struct ConstantBuffer {
        uint32_t seed = 0;

        float collisionRadius = 0.f;

        float densityThreshold = 0.f;
        float volumeThreshold  = 0.f;
        float maskThreshold    = 0.f;

        uint32_t placementResolution = 0;
        uint32_t sampleCount         = 0;

        float pad;

        OriGine::Vec2f worldMinXZ = OriGine::Vec2f(0.f, 0.f);
        OriGine::Vec2f worldMaxXZ = OriGine::Vec2f(0.f, 0.f);

        ConstantBuffer& operator=(const PointPlacementParamsData& data) {
            seed                = data.seed;
            collisionRadius     = data.collisionRadius;
            densityThreshold    = data.densityThreshold;
            volumeThreshold     = data.volumeThreshold;
            maskThreshold       = data.maskThreshold;
            placementResolution = data.placementResolution;
            sampleCount         = data.sampleCount;
            worldMinXZ          = data.worldMinXZ;
            worldMaxXZ          = data.worldMaxXZ;
            return *this;
        }
    };
};

/// <summary>
/// 点群配置をプロシージャルで設定するためのパラメータクラス
/// </summary>
class PointPlacementParams
    : public OriGine::IComponent {
    friend void to_json(nlohmann::json& _j, const PointPlacementParams& _p);
    friend void from_json(const nlohmann::json& _j, PointPlacementParams& _p);

public:
    PointPlacementParams()          = default;
    virtual ~PointPlacementParams() = default;
    virtual void Initialize(OriGine::Scene* _scene, OriGine::EntityHandle _owner) override;
    virtual void Finalize() override;
    virtual void Edit(OriGine::Scene* _scene, OriGine::EntityHandle _owner, const std::string& _parentLabel) override;

private:
    /// <summary>
    /// 点群の各点を表す構造体
    /// </summary>
    struct Point {
        OriGine::Vec3f pos = OriGine::Vec3f(0.f, 0.f, 0.f);
        float volume       = 0.f;

        /// <summary>
        /// 定数バッファ用の構造体
        /// </summary>
        struct ConstantBuffer {
            OriGine::Vec3f pos = OriGine::Vec3f(0.f, 0.f, 0.f);
            float volume       = 0.f;
            ConstantBuffer& operator=(const Point& data) {
                pos    = data.pos;
                volume = data.volume;
                return *this;
            }
        };
    };

private:
    OriGine::IConstantBuffer<PointPlacementParamsData> paramData_{};
    OriGine::AppendBuffer<Point> outputPoints_{};

    OriGine::Vec3f defaultScale_ = OriGine::Vec3f(1.f, 1.f, 1.f);

    // first: directory, second: filename
    std::pair<std::string, std::string> originalEntityFilePath_ = {};

    std::string pointDensityTexturePath_ = "";
    size_t pointDensityTextureIndex_     = 0;
    std::string pointVolumeTexturePath_  = "";
    size_t pointVolumeTextureIndex_      = 0;
    std::string pointMaskTexturePath_    = "";
    size_t pointMaskTextureIndex_        = 0;

public:
    OriGine::IConstantBuffer<PointPlacementParamsData>& GetParamData() { return paramData_; }
    OriGine::AppendBuffer<Point>& GetOutputPoints() { return outputPoints_; }

    const OriGine::Vec3f& GetDefaultScale() const { return defaultScale_; }

    size_t GetPointDensityTextureIndex() const { return pointDensityTextureIndex_; }
    size_t GetPointVolumeTextureIndex() const { return pointVolumeTextureIndex_; }
    size_t GetPointMaskTextureIndex() const { return pointMaskTextureIndex_; }

    const std::string& GetPointDensityTexturePath() const { return pointDensityTexturePath_; }
    const std::string& GetPointVolumeTexturePath() const { return pointVolumeTexturePath_; }
    const std::string& GetPointMaskTexturePath() const { return pointMaskTexturePath_; }

    const std::string& GetOriginalEntityDirectory() const { return originalEntityFilePath_.first; }
    const std::string& GetOriginalEntityFileName() const { return originalEntityFilePath_.second; }
};
