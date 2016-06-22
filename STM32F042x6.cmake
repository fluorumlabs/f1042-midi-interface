INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
# SET(CMAKE_C_COMPILER ${CMAKE_SOURCE_DIR}/Toolchain/bin/arm-none-eabi-gcc)
# SET(CMAKE_CXX_COMPILER ${CMAKE_SOURCE_DIR}/Toolchain/bin/arm-none-eabi-c++)
# SET(ENV{PATH} ${CMAKE_SOURCE_DIR}/Toolchain/lib/gcc/arm-none-eabi/5.3.1)
SET(ENV{PATH} ${CMAKE_SOURCE_DIR}/Toolchain/bin)
CMAKE_FORCE_C_COMPILER(arm-none-eabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(arm-none-eabi-g++ GNU)

SET(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F042F6Px_FLASH.ld)
SET(COMMON_FLAGS "-mcpu=cortex-m0 -mthumb -mfloat-abi=soft -ffunction-sections -fstrict-aliasing -fno-exceptions -fomit-frame-pointer -fdata-sections")
#SET(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16 -ffunction-sections -fdata-sections -g -fno-common -fmessage-length=0")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T${LINKER_SCRIPT}")