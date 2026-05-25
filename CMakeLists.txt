cmake_minimum_required(VERSION 4.2)
project(transfer_file_over_socket)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tfos
        src/main.cpp
)

target_compile_features(tfos PUBLIC cxx_std_23)

add_subdirectory(third)
target_link_libraries(tfos
        PRIVATE
        spdlog::spdlog
        Sockpp::sockpp
)