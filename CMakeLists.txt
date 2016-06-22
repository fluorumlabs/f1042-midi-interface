cmake_minimum_required(VERSION 3.5)
project(f1042_midi_interface)

add_definitions(-DSTM32F042x6)

file(GLOB_RECURSE USER_SOURCES "Src/*.c")
file(GLOB_RECURSE HAL_SOURCES "Drivers/STM32F0xx_HAL_Driver/Src/*.c")

set_property(SOURCE Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f042x6.s PROPERTY LANGUAGE C)

add_library(CMSIS Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.c)
add_library(STARTUP Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f042x6.s)

include_directories(Inc)
include_directories(Drivers/STM32F0xx_HAL_Driver/Inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32F0xx/Include)

add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${HAL_SOURCES} ${LINKER_SCRIPT})

target_link_libraries(${PROJECT_NAME}.elf CMSIS STARTUP)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--cref,--no-wchar-size-warning,--gc-sections,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map")
set(HEX_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
set(ASM_FILE ${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.asm)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -S -O ihex --gap-fill=0 $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -S -O binary --gap-fill=0 $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP} --prefix-addresses -S -d $<TARGET_FILE:${PROJECT_NAME}.elf> | sed 's/^080/\\/\\/ 080/'>${ASM_FILE}
        COMMENT "[100%] Building ${HEX_FILE} \n[100%] Building ${BIN_FILE}")