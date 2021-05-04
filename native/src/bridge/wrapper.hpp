#include <memory>
#include <tuple>
#include <jni.h>

#include "lua.hpp"
#include "spdlog/spdlog.h"

namespace lua {

class Wrapper {

public:
  static constexpr auto LOGGER_NAME = "lua";
  static constexpr auto FUNC_NS = "java";
  static constexpr auto FUNC_NAME = "call";
  static constexpr auto SELF_GLOBAL = "_self_";

  Wrapper(JNIEnv*, jclass, jobject);

  ~Wrapper();

  std::shared_ptr<spdlog::logger> logger();

  void logLevel(const std::string&);

  std::tuple<JNIEnv *, jclass, jobject> handle() const;

  void registerFunctions(const char *ns, const luaL_Reg *funcs);

  int exec(const char *);

  int exec();

  int load(const char *);

  void error(const std::string error);

  const std::string& error() const;

  lua_State *state() const;

private:
  JNIEnv *m_jenv;
  jclass m_jclass;
  jobject m_jobject;

  std::shared_ptr<spdlog::logger> m_logger;
  lua_State *L;
  std::string m_error{""};
};

Wrapper *get(JNIEnv *, jobject);

extern "C" int callback(lua_State *);

} // namespace lua
