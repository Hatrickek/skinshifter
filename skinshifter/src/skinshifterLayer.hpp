#pragma once
#include <Core/Layer.hpp>
#include <Scene/Scene.hpp>

namespace $NAMESPACE {
class skinshifterLayer : public ox::Layer {
public:
  skinshifterLayer();
  ~skinshifterLayer() override = default;
  void on_attach(ox::EventDispatcher& dispatcher) override;
  void on_detach() override;
  void on_update(const ox::Timestep& delta_time) override;
  void on_imgui_render() override;

  static skinshifterLayer* get() { return _instance; }
  
private:
  ox::Shared<ox::Scene> _scene;
  static skinshifterLayer* _instance;
};
}