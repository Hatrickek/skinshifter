#include "SkinshifterLayer.hpp"

#include <Assets/AssetManager.hpp>
#include <Assets/Texture.hpp>
#include <Core/App.hpp>
#include <Core/Base.hpp>
#include <Core/FileSystem.hpp>
#include <Render/RenderPipeline.hpp>
#include <Render/Renderer.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneRenderer.hpp>
#include <UI/OxUI.hpp>
#include <Utils/FileDialogs.hpp>
#include <Utils/Log.hpp>
#include <Utils/StringUtils.hpp>

#include <filesystem>
#include <icons/IconsMaterialDesignIcons.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <string>

#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader.h>

namespace ss {
SkinshifterLayer* SkinshifterLayer::_instance = nullptr;

SkinshifterLayer::SkinshifterLayer() : Layer("Game Layer") { _instance = this; }

void SkinshifterLayer::on_attach(ox::EventDispatcher& dispatcher) {
  struct SRP : public ox::RenderPipeline {
    explicit SRP(const std::string& name) : RenderPipeline(name) {}

    void init(vuk::Allocator& allocator) {}
    void shutdown() {}
    vuk::Value<vuk::ImageAttachment> on_render(vuk::Allocator& frame_allocator,
                                               vuk::Value<vuk::ImageAttachment> target,
                                               vuk::Extent3D ext) {
      return target;
    }
  };

  ox::Shared<SRP> simple_rp = ox::create_shared<SRP>("SimpleRenderPipeline");
  _scene = ox::create_shared<ox::Scene>(simple_rp);
}

void SkinshifterLayer::on_detach() {}

void SkinshifterLayer::on_update(const ox::Timestep& delta_time) {}

void SkinshifterLayer::on_imgui_render() {
  using namespace ox;

  ImGui::ShowDemoWindow();
  static ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                                  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);

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
    ui::input_text("##path", &_obsidian_path, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    if (ui::button(ox::StringUtils::from_char8_t(ICON_MDI_FOLDER), {30.f, 0.f})) {
      _obsidian_path = fs::preferred_path(App::get_system<ox::FileDialogs>()->open_dir());

      _characters_path = fs::append_paths(_obsidian_path, "assets/characters");
      _sounds_path = fs::append_paths(_obsidian_path, "assets/sounds/wwise2016/sfx");

      if (!std::filesystem::exists(std::filesystem::path(_characters_path))) {
        OX_LOG_ERROR("Characters path doesn't exist!");
      }
      if (!std::filesystem::exists(std::filesystem::path(_sounds_path))) {
        OX_LOG_ERROR("Sounds path doesn't exist!");
      }
    }

    if (!_characters_path.empty()) {
      ImGui::BeginChild("##Characters");
      const float filter_cursor_pos_x = ImGui::GetCursorPosX();

      name_filter.Draw("##char_filter",
                       ImGui::GetContentRegionAvail().x -
                         (ui::get_icon_button_size(ICON_MDI_PLUS, "").x + 2.0f * ImGui::GetStyle().FramePadding.x));

      if (!name_filter.IsActive()) {
        ImGui::SameLine();
        ImGui::SetCursorPosX(filter_cursor_pos_x + ImGui::GetFontSize() * 0.5f);
        ImGui::TextUnformatted(StringUtils::from_char8_t(ICON_MDI_MAGNIFY " Search..."));
      }

      if (!_selected_champ) {
        for (auto dir : std::filesystem::directory_iterator{_characters_path}) {
          if (dir.is_directory()) {
            const auto name = dir.path().filename().string();
            const auto path = dir.path().string();
            if (!name_filter.PassFilter(name.c_str())) {
              continue;
            }
            if (ui::button(name.c_str())) {
              const auto skins_path = fs::append_paths(path, "skins");
              _selected_champ = ox::create_unique<Champ>(name, skins_path);
              name_filter.Clear();
              break;
            }
          }
        }
      }

      // check to see if it's a skin or a chroma since we don't support chromas
      // for now we do it by checking if it has a loadscreen image inside

      std::vector<Skin> skins = {};

      if (_selected_champ) {
        for (auto skin_dir : std::filesystem::directory_iterator{_selected_champ->skins_path}) {
          if (skin_dir.path().filename() == "base" ||
              !name_filter.PassFilter(skin_dir.path().filename().string().c_str())) {
            continue;
          }
          constexpr auto skin_len = 4;
          constexpr auto index_len = 2;
          const auto skin_index_str = skin_dir.path().filename().string().substr(skin_len, index_len);
          const auto skin_index = std::stoi(skin_index_str);
          const auto load_screen_file_name = _selected_champ->name + "loadscreen" + "_" + std::to_string(skin_index) +
                                             ".dds";
          const auto load_screen_path = std::filesystem::path(fs::append_paths(skin_dir.path().string(),
                                                                               load_screen_file_name));
          if (std::filesystem::exists(load_screen_path)) {
            tinyddsloader::DDSFile dds = {};
            dds.Load(load_screen_path.string().c_str());
            ox::TextureLoadInfo info{
              .extent = {dds.GetWidth(), dds.GetHeight(), 1u},
              .format = vuk::Format::eBc1RgbUnormBlock,
              .data = dds.GetImageData()->m_mem,
            };

            skins.emplace_back(Skin{
              .name = skin_dir.path().filename().string(),
              .image = ox::AssetManager::get_texture_asset(load_screen_path.filename().string(), info),
              .index_str = skin_index_str,
            });
          }
        }

        ImGui::SeparatorText(_selected_champ->name.c_str());
        if (ui::button("Back")) {
          _selected_champ = nullptr;
        }

        for (const auto& s : skins) {
          if (ui::button(s.name.c_str())) {
            std::string path = "assets/characters/" + s.name + "/skins/skin" + s.index_str;
            // TODO: ask for a save directory then create the directories with `path`
            // in all dds files(except loading screen) replace "base" with "skin + index_str"
            // profit.
          }
          if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay)) {
            ImGui::BeginTooltip();
            ui::image(*s.image.get(), {(float)s.image->get_extent().width, (float)s.image->get_extent().height});
            ImGui::EndTooltip();
          }
        }
      }

      ImGui::EndChild();
    }

    ImGui::End();
  }
}
} // namespace ss
