#include "skinshifterLayer.hpp"

#include <Core/App.hpp>
#include <Modules/ModuleUtil.hpp>

namespace $NAMESPACE {
skinshifterLayer* skinshifterLayer::_instance = nullptr;

skinshifterLayer::skinshifterLayer() : Layer("Game Layer") {
  _instance = this;
}

void skinshifterLayer::on_attach(ox::EventDispatcher& dispatcher) {
  ox::ModuleUtil::load_module("skinshifterModule", "skinshifterModule");

  _scene = ox::create_shared<ox::Scene>();
  _scene->on_runtime_start();
}

void skinshifterLayer::on_detach() {
  ox::ModuleUtil::unload_module("skinshifterModule");
}

void skinshifterLayer::on_update(const ox::Timestep& delta_time) {
  _scene->on_runtime_update(delta_time);
}

void skinshifterLayer::on_imgui_render() {
  _scene->on_imgui_render(ox::App::get_timestep());
}
}