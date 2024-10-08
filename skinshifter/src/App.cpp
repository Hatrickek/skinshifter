#include <Core/App.hpp>
#include <Core/EntryPoint.hpp>

#include "SkinshifterLayer.hpp"

#include <filesystem>

namespace ox {
class SkinshifterApp : public ox::App {
public:
  SkinshifterApp(const ox::AppSpec& spec) : App(spec) {}
};

App* create_application(const AppCommandLineArgs& args) {
  AppSpec spec;
  spec.name = "skinshifter";
  spec.working_directory = std::filesystem::current_path().string();
  spec.command_line_args = args;
  spec.assets_path = "Assets";
  spec.default_window_size = {400, 600};

  const auto app = new SkinshifterApp(spec);
  app->push_layer(new ss::SkinshifterLayer());

  return app;
}
} // namespace ox
