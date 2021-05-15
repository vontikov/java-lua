#include <string>

#include "jni.h"
#include "jni_messages.hpp"

namespace jni {

jint throwRuntimeException(JNIEnv *, const char *message);

std::string jstring2string(JNIEnv *, jobject);

double jdouble2double(JNIEnv *, jobject);

bool jboolean2bool(JNIEnv *, jobject);

jobjectArray newObjectArray(JNIEnv *, int size);

jobject newDouble(JNIEnv *, float);

jobject newBoolean(JNIEnv *, bool);

jobject newString(JNIEnv *, const char*);

} // namespace jni
