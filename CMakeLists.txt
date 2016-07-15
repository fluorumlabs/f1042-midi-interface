cmake_minimum_required(VERSION 3.5)
project(f1042-midi-interface)

set(MCU STM32F042x6)

add_definitions(-D${MCU})

file(GLOB_RECURSE MX_SOURCES_MAIN "Src/main.c")
file(GLOB_RECURSE MX_SOURCES_IT "Src/stm32f0xx_it.c")
file(GLOB_RECURSE MX_SOURCES_HAL_MSP "Src/stm32f0xx_hal_msp.c")
file(GLOB_RECURSE APPLICATION_SOURCES "Application/*.c")
file(GLOB_RECURSE LOWLEVEL_SOURCES "Protoplasm/lowlevel/*.c")
file(GLOB_RECURSE UART_SOURCES "Protoplasm/uart/*.c")
file(GLOB_RECURSE I2C_SOURCES "Protoplasm/i2c/*.c")
file(GLOB_RECURSE PROTOPLASM_SOURCES "Protoplasm/protoplasm/*.c")
file(GLOB_RECURSE PROTOPLASM_USB_SOURCES "Protoplasm/usbdevice/*.c")
file(GLOB_RECURSE HAL_SOURCES "Drivers/STM32F0xx_HAL_Driver/Src/*.c")
#file(GLOB_RECURSE USB_MIDI_SOURCES "Middlewares/USB_MIDI/*.c")
file(GLOB_RECURSE USB_SOURCES "Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c")

set_property(SOURCE Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f042x6.s PROPERTY LANGUAGE C)

add_library(CMSIS Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.c)
add_library(STARTUP Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f042x6.s)

#include_directories(Middlewares/USB_MIDI)
include_directories(Inc)
include_directories(Application)
include_directories(Protoplasm/global)
include_directories(Protoplasm/lowlevel)
include_directories(Protoplasm/uart)
include_directories(Protoplasm/i2c)
include_directories(Protoplasm/usbdevice)
include_directories(Protoplasm/protoplasm)
include_directories(Drivers/STM32F0xx_HAL_Driver/Inc)
include_directories(Drivers/CMSIS/Include)
include_directories(Drivers/CMSIS/Device/ST/STM32F0xx/Include)
include_directories(Middlewares/ST/STM32_USB_Device_Library/Core/Inc)
#include_directories(Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc)

add_executable(${PROJECT_NAME}.elf ${MX_SOURCES_MAIN} ${MX_SOURCES_IT} ${MX_SOURCES_HAL_MSP} ${APPLICATION_SOURCES} ${LOWLEVEL_SOURCES} ${UART_SOURCES} ${I2C_SOURCES} ${PROTOPLASM_SOURCES} ${PROTOPLASM_USB_SOURCES} ${HAL_SOURCES} ${USB_SOURCES} ${LINKER_SCRIPT})

target_link_libraries(${PROJECT_NAME}.elf CMSIS STARTUP)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--cref,--no-wchar-size-warning,--gc-sections,-Map=${PROJECT_SOURCE_DIR}/build/${PROJECT_NAME}.map --specs=nano.specs")
set(HEX_FILE ${PROJECT_SOURCE_DIR}/Build/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_SOURCE_DIR}/Build/${PROJECT_NAME}.bin)
set(ASM_FILE ${PROJECT_SOURCE_DIR}/Build/${PROJECT_NAME}.asm)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -S -O ihex --gap-fill=0 $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -S -O binary --gap-fill=0 $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP} --prefix-addresses -S -D $<TARGET_FILE:${PROJECT_NAME}.elf> | sed 's/^080/\\/\\/ 080/'>${ASM_FILE}
        COMMENT "[100%] Building ${HEX_FILE} \n[100%] Building ${BIN_FILE}")

# Use CLANG for static analysis
set(CLANG "clang")

set(ANALYZER_FLAGS -Xanalyzer -analyzer-checker=alpha,core,deadcode,nullability,optin,security -Xanalyzer -analyzer-disable-checker=alpha.core.FixedAddr -Xanalyzer -analyzer-opt-analyze-nested-blocks)

get_property(ALL_INCLUDE_DIRECTORIES DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
set(INCLUDE_FLAGS)
foreach (INCLUDE_DIRECTORY ${ALL_INCLUDE_DIRECTORIES})
    list(APPEND INCLUDE_FLAGS "-I${INCLUDE_DIRECTORY}")
endforeach ()

foreach (SOURCE_FILE ${MX_SOURCES_MAIN} ${MX_SOURCES_IT} ${MX_SOURCES_HAL_MSP} ${APPLICATION_SOURCES} ${LOWLEVEL_SOURCES} ${UART_SOURCES} ${I2C_SOURCES} ${PROTOPLASM_SOURCES} ${PROTOPLASM_USB_SOURCES})
    add_custom_command(TARGET ${PROJECT_NAME}.elf
            PRE_LINK
            COMMAND ${CLANG} -I${PROJECT_SOURCE_DIR}/Toolchain/arm-none-eabi/include -Weverything -Reverything --analyze ${ANALYZER_FLAGS} ${INCLUDE_FLAGS} -std=gnu99 -D${MCU}
            ${SOURCE_FILE})
endforeach ()
