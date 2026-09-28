cmake_minimum_required(VERSION 3.13)

include (pico_sdk_import.cmake)
project (z80spo256al2 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(SIMPLE_VGA_PATH ${PROJECT_SOURCE_DIR})

pico_sdk_init()
add_executable(z80spo256al2
)

pico_generate_pio_header(z80spo256al2  ${CMAKE_CURRENT_LIST_DIR}/z80io.pio)
target_sources(z80spo256al2  PRIVATE z80spo256al2.c)


target_link_libraries(z80spo256al2  PRIVATE
        pico_stdlib
        hardware_pio
	hardware_dma
	pico_multicore
	hardware_gpio
	hardware_pwm
)

pico_enable_stdio_usb(z80spo256al2  1)
pico_enable_stdio_uart(z80spo256al2  0)

pico_add_extra_outputs(z80spo256al2 )




