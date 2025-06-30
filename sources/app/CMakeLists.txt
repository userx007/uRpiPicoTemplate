
cmake_minimum_required(VERSION 3.10)
project(rpicoshell C CXX ASM)

enable_language( C CXX ASM )
add_compile_options(-march=armv6-m)

add_executable(${PROJECT_NAME}
        src/main.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        uart_access
        ushell_core
        ushell_core_utils
        ushell_user_root
)

pico_add_extra_outputs(rpicoshell)

add_custom_command(TARGET rpicoshell POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/rpicoshell.elf
        ${CMAKE_CURRENT_BINARY_DIR}/rpicoshell.bin
        ${CMAKE_CURRENT_BINARY_DIR}/rpicoshell.uf2
        ${CMAKE_BINARY_DIR}/bin
)