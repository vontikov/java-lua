cmake_minimum_required(VERSION 3.10)

project(lua NONE)

include(ExternalProject)

find_program(MAKE_EXE NAMES make)

ExternalProject_Add(lua
  URL "https://www.lua.org/ftp/lua-5.4.3.tar.gz"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ${MAKE_EXE} "MYCFLAGS=-fPIC"
  BUILD_ALWAYS true
  INSTALL_COMMAND ""
  BUILD_IN_SOURCE true
)

ExternalProject_Get_property(lua SOURCE_DIR)
set(LUA_INCLUDE_DIRS "${SOURCE_DIR}/src" PARENT_SCOPE)
set(LUA_BINARY_DIRS "${SOURCE_DIR}/src" PARENT_SCOPE)
