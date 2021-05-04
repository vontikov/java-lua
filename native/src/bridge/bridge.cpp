#include <stdio.h>

#include "com_githhub_vontikov_jl_LuaEngine.h"
#include "helpers.hpp"
#include "wrapper.hpp"

JNIEXPORT jlong JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_init(JNIEnv *env, jobject jo) {
  auto jc = env->GetObjectClass(jo);
  if (!jc) {
    throwRuntimeException(env, "java class not found");
    return 0;
  }

  static const luaL_Reg f[] = {
      {lua::Wrapper::FUNC_NAME, lua::callback},
      {NULL, NULL}
  };

  auto l = new lua::Wrapper(env, jc, jo);
  l->registerFunctions(lua::Wrapper::FUNC_NS, f);
  return reinterpret_cast<jlong>(l);
}

JNIEXPORT void JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_destroy(JNIEnv *env, jobject jo) {
  auto l = lua::get(env, jo);
  if (!l) {
    throwRuntimeException(env, "lua wrapper");
    return;
  }
  delete l;
}

JNIEXPORT void JNICALL Java_com_githhub_vontikov_jl_LuaEngine_logLevel(
    JNIEnv *env, jobject jo, jstring js) {
  const auto level = jstring2string(env, js);
  auto l = lua::get(env, jo);
  if (!l) {
    throwRuntimeException(env, "lua wrapper");
    return;
  }
  l->logLevel(level);
}

JNIEXPORT jint JNICALL Java_com_githhub_vontikov_jl_LuaEngine_exec__Ljava_lang_String_2(
    JNIEnv *env, jobject jo, jstring script) {
  auto l = lua::get(env, jo);
  if (!l) {
    throwRuntimeException(env, "lua wrapper");
    return 0;
  }
  const char *s = env->GetStringUTFChars(script, 0);
  const auto r = l->exec(s);
  env->ReleaseStringUTFChars(script, s);
  return static_cast<jint>(r);
}

JNIEXPORT jint JNICALL Java_com_githhub_vontikov_jl_LuaEngine_exec__(
    JNIEnv *env, jobject jo) {
  auto l = lua::get(env, jo);
  if (!l) {
    throwRuntimeException(env, "lua wrapper");
    return 0;
  }
  return static_cast<jint>(l->exec());
}

JNIEXPORT jint JNICALL Java_com_githhub_vontikov_jl_LuaEngine_loadScript(
    JNIEnv *env, jobject jo, jstring script) {
  auto l = lua::get(env, jo);
  if (!l) {
    throwRuntimeException(env, "lua wrapper");
    return 0;
  }
  const char *s = env->GetStringUTFChars(script, 0);
  const auto r = l->load(s);
  env->ReleaseStringUTFChars(script, s);
  return static_cast<jint>(r);
}

JNIEXPORT jstring JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_getLastError(JNIEnv *env, jobject jo) {
  auto l = lua::get(env, jo);
  if (!l) {
    throwRuntimeException(env, "lua wrapper");
    return 0;
  }
  return env->NewStringUTF(l->error().c_str());
}
