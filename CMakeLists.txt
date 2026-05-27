cmake_minimum_required(VERSION 4.2)
project(transfer_file_over_socket)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- main executable start ---
add_executable(tfos
        src/main.cpp
        src/constant.h
        src/tcp_server.h
        src/file_getter.h
)

target_compile_features(tfos PUBLIC cxx_std_23)

add_subdirectory(third)
target_link_libraries(tfos
        PRIVATE
        spdlog::spdlog
        Sockpp::sockpp
)
# --- main executable end ---

# --- unit test start ---
add_executable(tests
        test/test_file_getter.cpp
)

target_compile_features(tests PUBLIC cxx_std_23)

target_link_libraries(tests
        gtest
        gtest_main
        spdlog::spdlog
)

add_custom_command(
        TARGET tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/test_resources"
)

add_custom_command(
        TARGET tests POST_BUILD
        COMMENT "Copying Test Resources"
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_SOURCE_DIR}/test/resources" "${CMAKE_BINARY_DIR}/test_resources"
)
# --- unit test end ---
