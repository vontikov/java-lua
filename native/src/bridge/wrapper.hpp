#include <map>
#include <memory>
#include <tuple>

#include "jni.h"
#include "lua.hpp"
#include "spdlog/spdlog.h"

namespace lua {

class Wrapper {

public:
  static constexpr auto LOGGER_NAME = "lua";
  static constexpr auto FUNC_NS = "java";
  static constexpr auto FUNC_NAME = "call";
  static constexpr auto SELF_GLOBAL = "_self_";

  using ns_map = std::map<const void *, std::string>;

  Wrapper(JNIEnv *, jclass, jobject);

  ~Wrapper();

  std::shared_ptr<spdlog::logger> logger();

  void logLevel(const std::string &);

  std::tuple<JNIEnv *, jclass, jobject> handle() const;

  int exec(const char *);

  int exec();

  int load(const char *);

  void error(const std::string error);

  const std::string &error() const;

  lua_State *state() const;

  const ns_map &ns() const;

  void registerFunction(const char *ns, const char *fn, const lua_CFunction);

private:
  JNIEnv *m_jenv;
  jclass m_jclass;
  jobject m_jobject;

  std::shared_ptr<spdlog::logger> m_logger;
  lua_State *L;
  std::string m_error;
  ns_map m_ns;
};

Wrapper *get(JNIEnv *, jobject);

extern "C" int callback(lua_State *);

jobjectArray prepareArgs(JNIEnv *env, lua_State *L, const char* ns, const char * fn);

} // namespace lua
