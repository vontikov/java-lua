#include <sstream>
#include <iostream>

#include "jni_helpers.hpp"
#include "lua_helpers.hpp"
#include "spdlog/cfg/env.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "wrapper.hpp"

namespace lua {

Wrapper::Wrapper(JNIEnv *env, jclass clazz, jobject obj)
    : m_jenv{env},m_jclass{clazz}, m_jobject{obj} {

  m_logger = std::make_shared<spdlog::logger>(LOGGER_NAME,
          std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

  L = luaL_newstate();
  luaL_openlibs(L);
  lua_pushnumber(L, reinterpret_cast<long>(this));
  lua_setglobal(L, SELF_GLOBAL);

  spdlog::cfg::load_env_levels();
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

const Wrapper::ns_map &Wrapper::ns() const { return m_ns; }

void Wrapper::registerFunction(const char *ns, const char *fn, const lua_CFunction fp) {
  if (m_logger->should_log(spdlog::level::debug)) {
    m_logger->debug("registering function: {}:{}", ns, fn);
  }
  if (lua_getglobal(L, ns); !lua_istable(L, -1)) {
    lua_newtable(L);
    lua_pushvalue(L, -1);
    lua_setglobal(L, ns);
    auto p = lua_topointer(L, -1);
    m_ns[p] = ns;
    if (m_logger->should_log(spdlog::level::debug)) {
      m_logger->debug("created namespace: {}", ns);
    }
  }

  lua_pushcfunction(L, fp);
  lua_setfield(L, -2, fn);
  lua_pop(L, lua_gettop(L));
}

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

jobjectArray prepareArgs(JNIEnv *env, lua_State *L, const char *ns, const char *fn) {
  auto sz = lua_gettop(L);
  auto ja = jni::newObjectArray(env, sz + 2);
  if (!ja) {
    return nullptr;
  }

  auto idx = 0;
  env->SetObjectArrayElement(ja, idx++, jni::newString(env, ns));
  env->SetObjectArrayElement(ja, idx++, jni::newString(env, fn));

  for (auto sp = 1; sp <= sz; sp++) {
    jobject o = nullptr;
    if (lua_isnumber(L, sp)) {
      auto v = lua_tonumber(L, sp);
      o = jni::newDouble(env, v);
    } else if (lua_isboolean(L, sp)) {
      auto v = lua_toboolean(L, sp);
      o = jni::newBoolean(env, v);
    } else if (lua_isstring(L, sp)) {
      auto v = lua_tostring(L, sp);
      o = jni::newString(env, v);
    }

    if (!o) {
      return nullptr;
    }
    env->SetObjectArrayElement(ja, idx++, o);
  }
  return ja;
}

extern "C" int callback(lua_State *L) {
  lua_getglobal(L, Wrapper::SELF_GLOBAL);
  auto top = lua_gettop(L);
  if (!lua_isnumber(L, top)) {
    lua_pop(L, top);
    return luaL_error(L, jni::ErrLuaWrapper);
  }
  auto l = reinterpret_cast<Wrapper *>((long)lua_tonumber(L, top));
  auto logger = l->logger();
  auto [env, jc, jo] = l->handle();

  if (logger->should_log(spdlog::level::debug)) {
    logger->debug("Lua stack:\n{}", lua::dumpStack(L));
  }

  if (!lua_istable(L, 1)) {
    lua_pop(L, top);
    return luaL_error(L, jni::ErrIncorrectCallNotation);
  }
  auto tp = lua_topointer(L, 1);

  lua_Debug info;
  if (!lua_getstack(L, 0, &info)) {
    lua_pop(L, lua_gettop(L));
    return luaL_error(L, jni::ErrNoStack);
  }
  lua_getinfo(L, "n", &info);

  // remove namespace and function name from stack
  lua_remove(L, -top);
  lua_remove(L, -1);

  auto ns = l->ns().at(tp).c_str();
  auto fn = info.name;
  auto ja = prepareArgs(env, L, ns, fn);
  auto jm = env->GetMethodID(jc, "callback", "([Ljava/lang/Object;)I");

  if (logger->should_log(spdlog::level::debug)) {
    logger->debug("calling external function: {}:{}", ns, fn);
  }
  auto r = env->CallIntMethod(jo, jm, ja);

  // call result to Lua
  if (logger->should_log(spdlog::level::debug)) {
    logger->debug("call result={}", r);
  }
  lua_pushinteger(L, r);
  return 1;
}

} // namespace lua
