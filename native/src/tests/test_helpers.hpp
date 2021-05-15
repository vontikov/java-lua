#include <optional>
#include <string>
#include <tuple>

#include "jni.h"

namespace test {

std::optional<std::tuple<JavaVM *, JNIEnv *>> createJVM();

void expectStringElement(JNIEnv *env, jobjectArray ja, int idx, std::string expected);

void expectDoubleElement(JNIEnv *env, jobjectArray ja, int idx, double expected);

void expectBooleanElement(JNIEnv *env, jobjectArray ja, int idx, bool expected);

} // namespace test
