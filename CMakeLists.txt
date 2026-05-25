cmake_minimum_required(VERSION 4.2)
project(transfer_file_over_socket)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

add_custom_command(
        TARGET tfos POST_BUILD
        COMMENT "Copying DLLs"
        COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:tfos> $<TARGET_RUNTIME_DLLS:tfos>
        COMMAND_EXPAND_LISTS
)
