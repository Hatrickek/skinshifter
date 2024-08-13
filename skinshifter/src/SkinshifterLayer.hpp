#pragma once
#include <Core/Layer.hpp>
#include <Scene/Scene.hpp>
#include <imgui.h>

namespace ss {
class SkinshifterLayer : public ox::Layer {
public:
  SkinshifterLayer();
  ~SkinshifterLayer() override = default;
  void on_attach(ox::EventDispatcher& dispatcher) override;
  void on_detach() override;
  void on_update(const ox::Timestep& delta_time) override;
  void on_imgui_render() override;

  static SkinshifterLayer* get() { return _instance; }

private:
  struct Skin {
    std::string name;
    ox::Shared<ox::Texture> image;
    std::string index_str;
  };

  struct Champ {
    std::string name;
    std::string skins_path;
  };

  ox::Unique<Champ> _selected_champ = {};

  ImGuiTextFilter name_filter;
  std::string _obsidian_path = {};
  std::string _characters_path = {};
  std::string _sounds_path = {};

  ox::Shared<ox::Scene> _scene;
  static SkinshifterLayer* _instance;
};
} // namespace ss
