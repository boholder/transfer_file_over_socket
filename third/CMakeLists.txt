# Extra CMake bindings for third party libraries that don't have them.

set(SPDLOG_ROOT ${CMAKE_SOURCE_DIR}/third/spdlog)
add_subdirectory(${SPDLOG_ROOT} EXCLUDE_FROM_ALL)

set(SOCKPP_ROOT ${CMAKE_SOURCE_DIR}/third/sockpp)
add_subdirectory(${SOCKPP_ROOT} EXCLUDE_FROM_ALL)

set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/third/googletest)
add_subdirectory(${GTEST_ROOT} EXCLUDE_FROM_ALL)
