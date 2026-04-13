#include "RailPoints.h"

/// engine
#include "asset/AssetSystem.h"
#include "scene/Scene.h"
// asset
#include "asset/TextureAsset.h"

/// math
#include "math/Spline.h"

#ifdef _DEBUG
/// editor
#include "editor/EditorController.h"

/// util
#include "util/myGui/MyGui.h"
#endif // _DEBUG

using namespace OriGine;

void to_json(nlohmann::json& _j, const RailPoints& _c) {
    _j = nlohmann::json{
        {"points", _c.points},
        {"radius", _c.radius},
        {"segmentDivide", _c.segmentDivide},
        {"texturePath", _c.texturePath},
        {"collisionRadiusOffset", _c.collisionRadiusOffset},
    };
}

void from_json(const nlohmann::json& _j, RailPoints& _c) {
    _j.at("points").get_to(_c.points);
    _j.at("radius").get_to(_c.radius);
    if (_j.contains("collisionRadiusOffset")) {
        _j.at("collisionRadiusOffset").get_to(_c.collisionRadiusOffset);
    }
    _j.at("segmentDivide").get_to(_c.segmentDivide);

    _j.at("texturePath").get_to(_c.texturePath);
}

RailPoints::RailPoints() : IComponent() {}
RailPoints::~RailPoints() {}

void RailPoints::Initialize(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/) {
    totalLength = CalcSplineLength(points);
    if (!texturePath.empty()) {
        textureIndex = AssetSystem::GetInstance()->GetManager<TextureAsset>()->LoadAsset(texturePath);
    }
}
void RailPoints::Finalize() {}

void RailPoints::Edit(OriGine::Scene* /*_scene*/, OriGine::EntityHandle /*_owner*/, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG

    DragGuiCommand<float>("Radius##" + _parentLabel, radius, 0.01f);
    DragGuiCommand<float>("Collision Radius Offset##" + _parentLabel, collisionRadiusOffset, 0.01f);
    DragGuiCommand<int32_t>("Segment Divide##" + _parentLabel, segmentDivide, 1, 1, 64);

    // --- ポイント追加ボタン ---
    std::string addLabel = "Add Point##" + _parentLabel;
    if (ImGui::Button(addLabel.c_str())) {
        // 新しいポイントを追加 (最後のポイントから少しずらした位置)
        Vec3f newPoint = Vec3f();
        if (!points.empty()) {
            newPoint = points.back() + Vec3f{0.f, 0.f, 1.0f};
        }
        auto command = std::make_unique<AddElementCommand<std::deque<Vec3f>>>(&points, newPoint);
        EditorController::GetInstance()->PushCommand(std::move(command));
        // スプライン長さを再計算
        totalLength = CalcSplineLength(points);
    }

    ImGui::Separator();

    // Drag&Drop用のペイロードタイプ
    static const char* kDragDropPayloadType = "RAILPOINTS_DND";
    static int dragSourceIndex              = -1;

    // --- ポイントリスト表示 ---
    for (int i = 0; i < static_cast<int>(points.size()); ++i) {
        ImGui::PushID(i);

        // ドラッグハンドル（順序入れ替え用）
        ImGui::Button("=");

        // --- Drag Source (ドラッグ開始) ---
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            dragSourceIndex = i;
            ImGui::SetDragDropPayload(kDragDropPayloadType, &i, sizeof(int));
            ImGui::Text("Move Point %d", i);
            ImGui::EndDragDropSource();
        }

        // --- Drag Target (ドロップ先) ---
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kDragDropPayloadType)) {
                int srcIndex = *static_cast<const int*>(payload->Data);
                int dstIndex = i;

                if (srcIndex != dstIndex && srcIndex >= 0 && srcIndex < static_cast<int>(points.size())) {
                    // 順序入れ替え用のコマンドを作成
                    // 元の配列をバックアップして新しい配列を作成
                    std::deque<Vec3f> newPoints = points;

                    // 要素を移動
                    Vec3f movedPoint = newPoints[srcIndex];
                    newPoints.erase(newPoints.begin() + srcIndex);
                    // srcIndex < dstIndex の場合、eraseにより dstIndex が1つずれる
                    int insertIndex = srcIndex < dstIndex ? dstIndex : dstIndex;
                    newPoints.insert(newPoints.begin() + insertIndex, movedPoint);

                    // SetterCommandでdeque全体を差し替え
                    auto command = std::make_unique<SetterCommand<std::deque<Vec3f>>>(
                        &points, newPoints,
                        [this](std::deque<Vec3f>* /*_val*/) {
                            totalLength = CalcSplineLength(points);
                        });
                    EditorController::GetInstance()->PushCommand(std::move(command));
                }
                dragSourceIndex = -1;
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::SameLine();

        // 削除ボタン
        std::string deleteLabel = "X##Point" + std::to_string(i) + _parentLabel;
        if (ImGui::Button(deleteLabel.c_str())) {
            auto command = std::make_unique<EraseElementCommand<std::deque<Vec3f>>>(&points, points.begin() + i);
            EditorController::GetInstance()->PushCommand(std::move(command));
            // スプライン長さを再計算
            totalLength = CalcSplineLength(points);
            ImGui::PopID();
            break; // リストが変更されたためループを抜ける
        }
        ImGui::SameLine();

        // ポイント位置編集
        std::string pointLabel = "Point " + std::to_string(i) + "##" + _parentLabel;
        DragGuiVectorCommand<3, float>(pointLabel, points[i], 0.1f, {}, {}, "%.3f",
            [this](Vector<3, float>* /*_val*/) {
                // 編集後にスプライン長さを再計算
                totalLength = CalcSplineLength(points);
            });

        ImGui::PopID();
    }

    ImGui::Separator();

    // --- 合計長さ表示 ---
    ImGui::Text("Total Length: %.3f", totalLength);

#endif // _DEBUG
}
