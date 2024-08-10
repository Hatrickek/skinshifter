#include "SkinshifterLayer.hpp"

#include <Core/App.hpp>
#include <Core/Base.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/Renderer.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneRenderer.hpp>
#include <UI/OxUI.hpp>
#include <Utils/StringUtils.hpp>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <icons/IconsMaterialDesignIcons.h>

namespace ss {
SkinshifterLayer *SkinshifterLayer::_instance = nullptr;

SkinshifterLayer::SkinshifterLayer() : Layer("Game Layer") { _instance = this; }

void SkinshifterLayer::on_attach(ox::EventDispatcher &dispatcher) {
  struct SRP : public ox::RenderPipeline {
    explicit SRP(const std::string &name) : RenderPipeline(name) {}

    void init(vuk::Allocator &allocator) {}
    void shutdown() {}
    vuk::Value<vuk::ImageAttachment>
    on_render(vuk::Allocator &frame_allocator,
              vuk::Value<vuk::ImageAttachment> target, vuk::Extent3D ext) {
      return target;
    }
  };

  ox::Shared<SRP> simple_rp = ox::create_shared<SRP>("SimpleRenderPipeline");
  _scene = ox::create_shared<ox::Scene>(simple_rp);
}

void SkinshifterLayer::on_detach() {}

void SkinshifterLayer::on_update(const ox::Timestep &delta_time) {}

void SkinshifterLayer::on_imgui_render() {
  using namespace ox;

  ImGui::ShowDemoWindow();
  static bool use_work_area = true;
  static ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration |
      ImGuiWindowFlags_MenuBar;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
  ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

  if (ImGui::Begin("main_window", nullptr, flags)) {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("Options")) {
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

  ImGui::AlignTextToFramePadding();
    ImGui::Text("Obsidian files directory: ");
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 40.0f);
    ui::input_text("##Tag", &_obsidian_path,
                   ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::SameLine();
    if (ui::button(ox::StringUtils::from_char8_t(ICON_MDI_FOLDER), {30.f, 0.f})) {
    }

    ImGui::End();
  }
}
} // namespace ss