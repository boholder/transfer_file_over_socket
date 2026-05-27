set(SPDLOG_ROOT ${CMAKE_SOURCE_DIR}/third/spdlog)
add_subdirectory(${SPDLOG_ROOT} EXCLUDE_FROM_ALL)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    # build static type library of sockpp
    # ref: https://github.com/fpagliughi/sockpp#build-options
    set(SOCKPP_BUILD_SHARED OFF CACHE BOOL "Build sockpp shared library" FORCE)
    set(SOCKPP_BUILD_STATIC ON CACHE BOOL "Build sockpp static library" FORCE)
endif ()
set(SOCKPP_ROOT ${CMAKE_SOURCE_DIR}/third/sockpp)
add_subdirectory(${SOCKPP_ROOT} EXCLUDE_FROM_ALL)

set(GTEST_ROOT ${CMAKE_SOURCE_DIR}/third/googletest)
add_subdirectory(${GTEST_ROOT} EXCLUDE_FROM_ALL)
