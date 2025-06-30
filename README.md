# uRpiPicoTemplate

Template for building shell based firmwares for RaspberryPi pico boards

## 1. Install Required Packages

    sudo apt update
    sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib build-essential git

## 2. Download the Pico SDK

    git clone https://github.com/raspberrypi/pico-sdk.git ~/pico-sdk
    cd ~/pico-sdk
    git submodule update --init

## 3. Set the environment variable

    echo 'export PICO_SDK_PATH=~/pico-sdk' >> ~/.bashrc
    source ~/.bashrc

## 4. Create a Sample Project

    mkdir -p ~/pico-projects/helloworld
    cd ~/pico-projects/helloworld


## 5. Create a file main.c:

    #include <stdio.h>
    #include "pico/stdlib.h"

    int main() {
        stdio_init_all();
        while (true) {
            printf("Hello, world!\n");
            sleep_ms(1000);
        }
    }

## 6. Create a CMakeLists.txt:

    cmake_minimum_required(VERSION 3.13)
    include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

    project(helloworld C CXX ASM)
    pico_sdk_init()

    add_executable(helloworld main.c)
    target_link_libraries(helloworld pico_stdlib)
    pico_add_extra_outputs(helloworld)

## 7. Build the Project

    mkdir build
    cd build
    cmake ..
    make -j$(nproc)

    This will generate a helloworld.uf2 file.

## 8. Flash the Pico

    Hold the BOOTSEL button on the Pico.
    Plug it into your PC via USB.
    Release the button.

    Copy the .uf2 file:
    cp helloworld.uf2 /media/$USER/RPI-RP2/


## Optional: Enable Serial Output

    To view serial output:
    sudo usermod -a -G dialout $USER
    sudo apt install minicom
    minicom -b 115200 -o -D /dev/ttyACM0
