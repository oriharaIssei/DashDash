#include "BulletSpawner.h"

#ifdef _DEBUG
#define RESOURCE_DIRECTORY
#include "EngineInclude.h"

#include "component/effect/particle/emitter/EmitterEditor.h"
#include "myFileSystem/MyFileSystem.h"
#include "myGui/MyGui.h"
#endif

using namespace OriGine;

void BulletSpawner::Initialize(Scene* _scene, EntityHandle /*_entity*/) {
    emitter_.Initialize();
    emitter_.ResolveParent(_scene);
}

void BulletSpawner::Finalize() {}

void BulletSpawner::Edit([[maybe_unused]] Scene* _scene, [[maybe_unused]] EntityHandle _entity, [[maybe_unused]] const std::string& _parentLabel) {
#ifdef _DEBUG
    // ── テンプレート選択 ──────────────────────────────────────
    ImGui::SeparatorText("Bullet Template");
    ImGui::Text("%s", templateTypeName_.empty() ? "(none)" : templateTypeName_.c_str());
    ImGui::SameLine();
    if (ImGui::Button(("Select##BulletTemplate" + _parentLabel).c_str())) {
        std::string directory, filename;
        if (MyFileSystem::SelectFileDialog(kApplicationResourceDirectory, directory, filename, {"ent"})) {
            auto dot          = filename.rfind('.');
            templateTypeName_ = (dot != std::string::npos) ? filename.substr(0, dot) : filename;
        }
    }

    // ── Emitter 共通 UI ──────────────────────────────
    ImGui::Spacing();
    ImGui::SeparatorText("Spawn Controller");
    ImGui::Spacing();

    EmitterEditor::Draw(emitter_, _parentLabel, _scene);
#endif
}

// ── Serialization ──────────────────────────────────────────────────────────

void OriGine::to_json(nlohmann::json& _j, const BulletSpawner& _comp) {
    _j["templateTypeName"] = _comp.templateTypeName_;
    _j["emitter"]           = _comp.emitter_;
}

void OriGine::from_json(const nlohmann::json& _j, BulletSpawner& _comp) {
    _j.at("templateTypeName").get_to(_comp.templateTypeName_);
    _j.at("emitter").get_to(_comp.emitter_);
}
