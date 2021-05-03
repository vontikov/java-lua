cmake_minimum_required(VERSION 3.10)

project(lua NONE)

include(ExternalProject)

ExternalProject_Add(spdlog
  URL "https://github.com/gabime/spdlog/archive/refs/tags/v1.8.5.tar.gz"
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/spdlog
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  BUILD_ALWAYS true
)

ExternalProject_Get_Property(spdlog install_dir)
set(SPDLOG_BINARY_DIRS "${install_dir}/lib" PARENT_SCOPE)
set(SPDLOG_INCLUDE_DIRS "${install_dir}/include" PARENT_SCOPE)
