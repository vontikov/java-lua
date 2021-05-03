cmake_minimum_required(VERSION 3.10)

project(jni)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(DIST_DIRS ${CMAKE_BINARY_DIR}/dist)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIST_DIRS})
set(CMAKE_SKIP_BUILD_RPATH true)

option(BUILD_TESTING "" ON)

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

add_subdirectory(third_party/spdlog)
add_subdirectory(third_party/lua)

add_library(jl SHARED
  bridge/bridge.cpp
  bridge/wrapper.cpp
  bridge/helpers.cpp
)

add_dependencies(jl lua)
add_dependencies(jl spdlog)

target_include_directories(jl PRIVATE ${JNI_INCLUDE_DIRS})
target_include_directories(jl PRIVATE ${LUA_INCLUDE_DIRS})
target_include_directories(jl PRIVATE ${SPDLOG_INCLUDE_DIRS})
target_include_directories(jl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(jl
  ${CMAKE_DL_LIBS}
  ${LUA_BINARY_DIRS}/liblua.a
  ${SPDLOG_BINARY_DIRS}/libspdlog.a
)

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(third_party/google-test)
  add_executable(tests
    tests/example_test.cpp
  )
  target_link_libraries(tests
    jl
    GTest::GTest
    GTest::Main
  )
  add_test(test_all tests)
endif()
