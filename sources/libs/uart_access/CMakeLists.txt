
cmake_minimum_required(VERSION 3.3)
project(uart_access)

enable_language( C CXX ASM )
add_compile_options(-march=armv6-m)

add_library(${PROJECT_NAME}
    STATIC
        src/uart_access.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/inc
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        pico_stdlib
        pico_stdio_usb
	ushell_core_config
)