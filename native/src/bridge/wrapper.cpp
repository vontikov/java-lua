#include <sstream>

#include "spdlog/cfg/env.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "wrapper.hpp"

namespace lua {

Wrapper::Wrapper(JNIEnv *env, jclass clazz, jobject obj)
    : m_jenv{env}, m_jclass{clazz}, m_jobject{obj} {
  m_logger = std::make_shared<spdlog::logger>(
      LOGGER_NAME, std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

  spdlog::cfg::load_env_levels();

  L = luaL_newstate();
  luaL_openlibs(L);

  lua_pushnumber(L, reinterpret_cast<long>(this));
  lua_setglobal(L, SELF_GLOBAL);

  m_logger->info("created");
}

Wrapper::~Wrapper() {
  lua_close(L);
  m_logger->info("desroyed");
}

std::shared_ptr<spdlog::logger> Wrapper::logger() { return m_logger; }

void Wrapper::logLevel(const std::string &level) {
  if (level == "trace" || level == "all") {
    m_logger->set_level(spdlog::level::trace);
  } else if (level == "debug") {
    m_logger->set_level(spdlog::level::debug);
  } else if (level == "info") {
    m_logger->set_level(spdlog::level::info);
  } else if (level == "warn") {
    m_logger->set_level(spdlog::level::warn);
  } else if (level == "error") {
    m_logger->set_level(spdlog::level::err);
  } else if (level == "fatal") {
    m_logger->set_level(spdlog::level::critical);
  } else if (level == "off") {
    m_logger->set_level(spdlog::level::off);
  }
}

std::tuple<JNIEnv *, jclass, jobject> Wrapper::handle() const {
  return std::make_tuple(m_jenv, m_jclass, m_jobject);
}

void Wrapper::registerFunctions(const char *ns, const luaL_Reg *funcs) {
  lua_newtable(L);
  luaL_setfuncs(L, funcs, 0);
  lua_setglobal(L, ns);
}

int Wrapper::exec(const char *script) {
  if (auto r = load(script); r) {
    return r;
  }
  return exec();
}

int Wrapper::load(const char *script) {
  if (m_logger->should_log(spdlog::level::trace)) {
    m_logger->trace("load script:\n'''\n{}\n'''", script);
  }

  if (auto r = luaL_loadstring(L, script); r) {
    auto errMsg = lua_tostring(L, -1);
    m_logger->error("loadbuffer(): code={}, message={}", r, errMsg);
    this->error(errMsg);
    return r;
  }
  return LUA_OK;
}

int Wrapper::exec() {
  const auto top = lua_gettop(L);
  if (!top) {
    constexpr auto errMsg = "no script to execute";
    m_logger->error(errMsg);
    this->error(errMsg);
    return LUA_ERRRUN;
  }

  if (m_logger->should_log(spdlog::level::trace)) {
    m_logger->trace("execute script: top={}", top);
  }
  lua_pushvalue(L, -1);
  if (auto r = lua_pcall(L, 0, 0, 0); r) {
    auto errMsg = lua_tostring(L, -1);
    m_logger->error("pcall(): code={}, message={}", r, errMsg);
    this->error(errMsg);
    return r;
  }
  return LUA_OK;
}

void Wrapper::error(const std::string v) { m_error = v; }

const std::string &Wrapper::error() const { return m_error; }

lua_State *Wrapper::state() const { return L; }

Wrapper *get(JNIEnv *env, jobject jo) {
  constexpr auto objFieldName = "np";

  auto jc = env->GetObjectClass(jo);
  if (!jc) {
    return nullptr;
  }
  auto jfid = env->GetFieldID(jc, objFieldName, "J");
  if (!jfid) {
    return nullptr;
  }
  return reinterpret_cast<Wrapper *>(env->GetLongField(jo, jfid));
}

extern "C" int callback(lua_State *L) {
  const auto argc = lua_gettop(L);
  if (argc < 1) {
    return luaL_error(L, "expecting at least one argument");
  }

  if (lua_getglobal(L, lua::Wrapper::SELF_GLOBAL); !lua_isnumber(L, argc + 1)) {
    lua_settop(L, 0);
    return luaL_error(L, "wrapper not found");
  }

  auto l = reinterpret_cast<lua::Wrapper *>((long)lua_tonumber(L, argc + 1));
  auto logger = l->logger();
  auto [env, jc, jo] = l->handle();

  if (!lua_isstring(L, 1)) {
    lua_settop(L, 0);
    return luaL_error(
        L, "first argument must be an external function name (string)");
  }

  jobjectArray ja =
      env->NewObjectArray(argc, env->FindClass("java/lang/Object"), nullptr);
  jobject refs[argc];

  for (auto i = 1, idx = 0; i <= argc; i++, idx++) {
    jobject o;

    if (lua_isnumber(L, i)) {
      auto v = lua_tonumber(L, i);
      auto jc = env->FindClass("java/lang/Double");
      auto jm = env->GetMethodID(jc, "<init>", "(D)V");
      o = env->NewObject(jc, jm, jo, v);
      if (logger->should_log(spdlog::level::debug)) {
        logger->debug("number argument: value={}", v);
      }
    } else if (lua_isboolean(L, i)) {
      auto v = lua_toboolean(L, i);
      auto jc = env->FindClass("java/lang/Boolean");
      auto jm = env->GetMethodID(jc, "<init>", "(Z)V");
      o = env->NewObject(jc, jm, jo, v);
      if (logger->should_log(spdlog::level::debug)) {
        logger->debug("boolean argument: value={}", v ? "true" : "false");
      }
    } else if (lua_isstring(L, i)) {
      auto v = lua_tostring(L, i);
      o = env->NewStringUTF(v);
      if (logger->should_log(spdlog::level::debug)) {
        if (i == 0) {
          logger->debug("calling external function: name={}", v);
        } else {
          logger->debug("string argument: value={}", v);
        }
      }
    } else {
      std::stringstream ss;
      ss << "unsupported argument type: '" << lua_typename(L, i) << "'";
      auto errMsg = ss.str();
      l->error(errMsg);
      lua_settop(L, 0);
      return luaL_error(L, errMsg.c_str());
    }

    env->SetObjectArrayElement(ja, idx, o);
    refs[idx] = o;
  }

  auto jm = env->GetMethodID(jc, "callback", "([Ljava/lang/Object;)I");
  auto r = env->CallIntMethod(jo, jm, ja);
  logger->info("java callback called: result={}", r);

  // clean up
  for (auto o : refs) {
    env->DeleteLocalRef(o);
  }

  // call result to Lua
  lua_pushinteger(L, r);
  return 1;
}

} // namespace lua
