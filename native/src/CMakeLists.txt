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

target_include_directories(jl PRIVATE
  ${JNI_INCLUDE_DIRS}
  ${LUA_INCLUDE_DIRS}
  ${SPDLOG_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(jl
  ${CMAKE_DL_LIBS}
  ${LUA_BINARY_DIRS}/liblua.a
  ${SPDLOG_BINARY_DIRS}/libspdlog.a
)

if(BUILD_TESTING)
  enable_testing()
  list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
  add_subdirectory(third_party/google-test)
  add_executable(tests
    tests/wrapper_test.cpp
  )
  target_include_directories(tests PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
    ${SPDLOG_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bridge
  )
  target_link_libraries(tests
    GTest::GTest
    GTest::Main
    jl
  )
  add_test(test_all tests)
  set_tests_properties(test_all PROPERTIES
    ENVIRONMENT LD_LIBRARY_PATH=${DIST_DIRS}
  )
endif()
