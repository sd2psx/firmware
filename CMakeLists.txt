cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ext/pico-sdk)
include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_conf.h CACHE STRING "" FORCE)
add_subdirectory(ext/lvgl EXCLUDE_FROM_ALL)
add_subdirectory(ext/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI EXCLUDE_FROM_ALL)

add_executable(sd2psx
    src/main.c
    src/memory_card.c
    src/debug.c
    src/tonyhax.c
    src/gui.c
    src/input.c
    src/pio_spi.c
    src/psram.c
    src/sd.c
    src/sd_hw_config.c
    src/ui_menu.c
    src/ui_theme_mono.c

    ext/pico-ssd1306/ssd1306.c
)

target_compile_definitions(
    sd2psx PUBLIC
    PICO_XOSC_STARTUP_DELAY_MULTIPLIER=64
)

target_include_directories(sd2psx PUBLIC
    ext/pico-ssd1306
)

pico_generate_pio_header(sd2psx ${CMAKE_CURRENT_LIST_DIR}/src/psx_spi.pio)
pico_generate_pio_header(sd2psx ${CMAKE_CURRENT_LIST_DIR}/src/spi.pio)

target_link_libraries(sd2psx
    pico_stdlib
    pico_multicore
    hardware_pio
    hardware_i2c
    lvgl::lvgl
    FatFs_SPI
)

pico_add_extra_outputs(sd2psx)
