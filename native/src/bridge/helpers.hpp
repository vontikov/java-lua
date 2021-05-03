#include <string>

#include "jni.h"

jint throwRuntimeException(JNIEnv *, const char *message);

// see: https://stackoverflow.com/questions/41820039/jstringjni-to-stdstringc-with-utf8-characters
std::string jstring2string(JNIEnv *, jstring);
