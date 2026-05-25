# Extra CMake bindings for 3rdparty libraries that don't have them.

set(SPDLOG_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/spdlog)
add_subdirectory(${SPDLOG_ROOT} EXCLUDE_FROM_ALL)

set(SOCKPP_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/sockpp)
add_subdirectory(${SOCKPP_ROOT} EXCLUDE_FROM_ALL)

set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/3rdparty/googletest)
add_subdirectory(${GTEST_ROOT} EXCLUDE_FROM_ALL)
