#include <stdio.h>

#include "com_githhub_vontikov_jl_LuaEngine.h"
#include "jni_helpers.hpp"
#include "wrapper.hpp"

JNIEXPORT jlong JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_init(JNIEnv *env, jobject jo) {
  if (auto jc = env->GetObjectClass(jo); jc) {
    auto l = new lua::Wrapper(env, jc, jo);
    return reinterpret_cast<jlong>(l);
  }
  jni::throwRuntimeException(env, jni::ErrClassNotFound);
  return 0;
}

JNIEXPORT void JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_destroy(JNIEnv *env, jobject jo) {
  if (auto l = lua::get(env, jo); l) {
    delete l;
    return;
  }
  jni::throwRuntimeException(env, jni::ErrLuaWrapper);
}

JNIEXPORT void JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_logLevel(JNIEnv *env, jobject jo, jstring js) {
  if (auto l = lua::get(env, jo); l) {
    const auto level = jni::jstring2string(env, js);
    l->logLevel(level);
    return;
  }
  jni::throwRuntimeException(env, jni::ErrLuaWrapper);
}

JNIEXPORT jint JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_exec__Ljava_lang_String_2(JNIEnv *env, jobject jo, jstring script) {
  if (auto l = lua::get(env, jo); l) {
    const char *s = env->GetStringUTFChars(script, 0);
    const auto r = l->exec(s);
    env->ReleaseStringUTFChars(script, s);
    return static_cast<jint>(r);
  }
  jni::throwRuntimeException(env, jni::ErrLuaWrapper);
  return 0;
}

JNIEXPORT void JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_registerFunction (JNIEnv *env, jobject jo, jstring jns, jstring jfn) {
  if (auto l = lua::get(env, jo); l) {
    const auto ns = env->GetStringUTFChars(jns, 0);
    const auto fn = env->GetStringUTFChars(jfn, 0);
    l->registerFunction(ns, fn, lua::callback);
    env->ReleaseStringUTFChars(jns, ns);
    env->ReleaseStringUTFChars(jfn, fn);
    return;
  }
  jni::throwRuntimeException(env, jni::ErrLuaWrapper);
}

JNIEXPORT jint JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_exec__(JNIEnv *env, jobject jo) {
  if (auto l = lua::get(env, jo); l) {
    return static_cast<jint>(l->exec());
  }
  jni::throwRuntimeException(env, jni::ErrLuaWrapper);
  return 0;
}

JNIEXPORT jint JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_loadScript(JNIEnv *env, jobject jo, jstring script) {
  if (auto l = lua::get(env, jo); l) {
    const auto s = env->GetStringUTFChars(script, 0);
    const auto r = l->load(s);
    env->ReleaseStringUTFChars(script, s);
    return static_cast<jint>(r);
  }
  jni::throwRuntimeException(env, jni::ErrLuaWrapper);
  return 0;
}

JNIEXPORT jstring JNICALL
Java_com_githhub_vontikov_jl_LuaEngine_getLastError(JNIEnv *env, jobject jo) {
  if (auto l = lua::get(env, jo); l) {
    return env->NewStringUTF(l->error().c_str());
  }
  jni::throwRuntimeException(env, jni::ErrLuaWrapper);
  return 0;
}
