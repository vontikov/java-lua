#include "test_helpers.hpp"
#include "jni_helpers.hpp"
#include "gtest/gtest.h"

namespace test {

std::optional<std::tuple<JavaVM *, JNIEnv *>> createJVM() {
  JavaVMOption opt[1];

  JavaVMInitArgs vmArgs;
  vmArgs.version = JNI_VERSION_1_2;
  vmArgs.ignoreUnrecognized = JNI_TRUE;
  vmArgs.nOptions = 1;
  vmArgs.options = opt;

  JavaVM *jvm;
  JNIEnv *env;
  if (JNI_CreateJavaVM(&jvm, (void **)&env, &vmArgs) == JNI_ERR) {
    return std::nullopt;
  }
  return {{jvm, env}};
}

void expectStringElement(JNIEnv *env, jobjectArray ja, int idx, std::string expected) {
  auto o = env->GetObjectArrayElement(ja, idx);

  auto clazz = env->FindClass("java/lang/String");
  EXPECT_EQ(true, env->IsInstanceOf(o, clazz));

  auto s = jni::jstring2string(env, (jstring)o);
  EXPECT_EQ(expected, s);
}

void expectDoubleElement(JNIEnv *env, jobjectArray ja, int idx, double expected) {
  auto o = env->GetObjectArrayElement(ja, idx);

  auto clazz = env->FindClass("java/lang/Double");
  EXPECT_EQ(true, env->IsInstanceOf(o, clazz));

  auto v = jni::jdouble2double(env, o);
  EXPECT_EQ(expected, v);
}

void expectBooleanElement(JNIEnv *env, jobjectArray ja, int idx, bool expected){
  auto o = env->GetObjectArrayElement(ja, idx);

  auto clazz = env->FindClass("java/lang/Boolean");
  EXPECT_EQ(true, env->IsInstanceOf(o, clazz));

  auto v = jni::jboolean2bool(env, o);
  EXPECT_EQ(expected, v);
}

} // namespace test
