#include "Module.hpp"

#include <entt/entt.hpp>
#include <sol/sol.hpp>
#include <Scripting/LuaHelpers.hpp>

namespace ox {
void skinshifterModule::register_components(sol::state* state, const entt::locator<entt::meta_ctx>::node_type& ctx) {
  entt::locator<entt::meta_ctx>::reset(ctx); // set local meta context
}

void skinshifterModule::unregister_components(sol::state* state, const entt::locator<entt::meta_ctx>::node_type& ctx) {
}
}