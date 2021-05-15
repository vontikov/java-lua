#include <sstream>

#include "lua_helpers.hpp"

namespace lua {

std::string dumpStack(lua_State *L) {
  std::stringstream ss;

  auto top = lua_gettop(L);
  for (auto i = 1; i <= top; i++) {
    ss << i << '\t' << luaL_typename(L, i) << '\t';
    switch (lua_type(L, i)) {
    case LUA_TNUMBER:
      ss << lua_tonumber(L, i) << std::endl;
      break;
    case LUA_TSTRING:
      ss << lua_tostring(L, i) << std::endl;
      break;
    case LUA_TBOOLEAN:
      ss << std::boolalpha << (lua_toboolean(L, i) ? true : false) << std::endl;
      break;
    case LUA_TNIL:
      ss << "nil" << std::endl;
      break;
    default:
      ss << lua_topointer(L, i) << std::endl;
      break;
    }
  }
  return ss.str();
}

} // namespace lua
