#include <iostream>

#include "gtest/gtest.h"
#include "jni_helpers.hpp"
#include "lua_helpers.hpp"
#include "test_helpers.hpp"
#include "wrapper.hpp"

int fooCallback(lua_State *L) {
  std::cout << lua::dumpStack(L) << std::endl;

  // signal to the test
  lua_pushinteger(L, 1000);
  lua_setglobal(L, "foo");

  // result to the script
  lua_pushinteger(L, 100);
  return 1;
}

int barCallback(lua_State *L) {
  std::cout << lua::dumpStack(L) << std::endl;

  // signal to the test
  lua_pushinteger(L, 2000);
  lua_setglobal(L, "bar");

  // result to the script
  lua_pushinteger(L, 200);
  return 1;
}

TEST(TestWrapper, ExecuteScript) {
  auto l = new lua::Wrapper(nullptr, 0, 0);
  auto L = l->state();
  EXPECT_EQ(0, lua_gettop(L));

  const char *script = R"lua(
    print(ns:foo('abc', 1, true))
    print(ns:bar('xyz', 2, false))
)lua";

  l->registerFunction("ns", "foo", fooCallback);
  EXPECT_EQ(0, lua_gettop(L));

  l->registerFunction("ns", "bar", barCallback);
  EXPECT_EQ(0, lua_gettop(L));

  auto r = l->load(script);
  EXPECT_EQ(LUA_OK, r);
  EXPECT_EQ(1, lua_gettop(L));
  EXPECT_TRUE(lua_isfunction(L, -1));

  r = l->exec();
  EXPECT_EQ(LUA_OK, r);
  EXPECT_EQ(1, lua_gettop(L));

  lua_getglobal(L, "foo");
  EXPECT_TRUE(lua_isnumber(L, -1));
  EXPECT_EQ(1000, lua_tonumber(L, -1)) << "the callback should be called";

  lua_getglobal(L, "bar");
  EXPECT_TRUE(lua_isnumber(L, -1));
  EXPECT_EQ(2000, lua_tonumber(L, -1)) << "the callback should be called";
}

TEST(TestWrapper, PrepareArgs) {
  auto vm = test::createJVM();
  EXPECT_TRUE(vm);
  auto [jvm, env] = *vm;

  auto L = luaL_newstate();
  luaL_openlibs(L);

  lua_pushstring(L, "abc");
  lua_pushnumber(L, 42);
  lua_pushboolean(L, true);
  lua_pushboolean(L, false);

  auto ja = lua::prepareArgs(env, L, "foo", "bar");
  EXPECT_NE(nullptr, ja);

  auto sz = env->GetArrayLength(ja);
  EXPECT_EQ(6, sz) << "namespace, method, args";

  test::expectStringElement(env, ja, 0, "foo");
  test::expectStringElement(env, ja, 1, "bar");
  test::expectStringElement(env, ja, 2, "abc");
  test::expectDoubleElement(env, ja, 3, 42.0);
  test::expectBooleanElement(env, ja, 4, true);
  test::expectBooleanElement(env, ja, 5, false);

  jvm->DestroyJavaVM();
}
