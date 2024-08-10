#include <Core/EntryPoint.hpp>
#include <Core/App.hpp>

#include "skinshifterLayer.hpp"

#include <filesystem>

namespace ox {
class skinshifterApp : public ox::App {
public:
  skinshifterApp(const ox::AppSpec& spec) : App(spec) { }
};

App* create_application(const AppCommandLineArgs& args) {
  AppSpec spec;
  spec.name = "skinshifter";
  spec.working_directory = std::filesystem::current_path().string();
  spec.command_line_args = args;
  spec.assets_path = "Assets";

  const auto app = new skinshifterApp(spec);
  app->push_layer(new $NAMESPACE::skinshifterLayer());

  return app;
}
}