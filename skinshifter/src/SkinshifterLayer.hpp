#pragma once
#include <Core/Layer.hpp>
#include <Scene/Scene.hpp>

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
  std::string _obsidian_path = {};

  ox::Shared<ox::Scene> _scene;
  static SkinshifterLayer* _instance;
};
}