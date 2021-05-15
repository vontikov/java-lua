#include "jni_helpers.hpp"

namespace jni {

jint throwRuntimeException(JNIEnv *env, const char *message) {
  auto jc = env->FindClass("java/lang/RuntimeException");
  if (!jc) {
    return -1;
  }
  return env->ThrowNew(jc, message);
}

std::string jstring2string(JNIEnv *env, jobject jo) {
  if (!jo) {
    return "";
  }

  auto jc = env->GetObjectClass(jo);
  auto jm = env->GetMethodID(jc, "getBytes", "(Ljava/lang/String;)[B");
  auto b = (jbyteArray)env->CallObjectMethod(jo, jm, env->NewStringUTF("UTF-8"));

  auto p = env->GetByteArrayElements(b, NULL);
  auto sz = static_cast<size_t>(env->GetArrayLength(b));
  std::string ret{reinterpret_cast<char *>(p), sz};
  return ret;
}

double jdouble2double(JNIEnv *env, jobject jo) {
  if (!jo) {
    return 0;
  }

  auto jc = env->GetObjectClass(jo);
  auto jm = env->GetMethodID(jc, "doubleValue", "()D");
  return env->CallDoubleMethod(jo, jm);
}

bool jboolean2bool(JNIEnv *env, jobject jo) {
  if (!jo) {
    return 0;
  }

  auto jc = env->GetObjectClass(jo);
  auto jm = env->GetMethodID(jc, "booleanValue", "()Z");
  return env->CallBooleanMethod(jo, jm);
}

jobjectArray newObjectArray(JNIEnv *env, int size) {
  return env->NewObjectArray(size, env->FindClass("java/lang/Object"), nullptr);
}

jobject newDouble(JNIEnv *env, float v) {
  auto jc = env->FindClass("java/lang/Double");
  auto jm = env->GetMethodID(jc, "<init>", "(D)V");
  return env->NewObject(jc, jm, v);
}

jobject newBoolean(JNIEnv *env, bool v) {
  auto jc = env->FindClass("java/lang/Boolean");
  auto jm = env->GetMethodID(jc, "<init>", "(Z)V");
  return env->NewObject(jc, jm, v);
}

jobject newString(JNIEnv *env, const char *v) {
    return env->NewStringUTF(v);
}

} // namespace jni
