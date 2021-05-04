#include <gtest/gtest.h>
#include <iostream>

#include "wrapper.hpp"

TEST(TestWrapper, LoadScript) {
  auto l = new lua::Wrapper(nullptr, 0, 0);

  auto L = l->state();
  EXPECT_EQ(0, lua_gettop(L));

  const char *script = R"lua(
function hi()
  print('hi!')
end

hi()
)lua";

  int r;

  r = l->load(script);
  EXPECT_EQ(LUA_OK, r);
  EXPECT_EQ(1, lua_gettop(L));
  EXPECT_TRUE(lua_isfunction(L, -1));

  r = l->exec();
  EXPECT_EQ(LUA_OK, r);
  EXPECT_EQ(1, lua_gettop(L));
}
