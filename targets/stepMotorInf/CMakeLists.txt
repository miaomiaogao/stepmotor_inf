#THIS FILE IS AUTO GENERATED FROM THE TEMPLATE! DO NOT CHANGE!

if (NOT DEFINED CMAKE_SYSTEM_NAME)
        # Standalone we need set compiler settings (as a subdirectory we expect the outer CMakeLists.txt to do this)

        set(CMAKE_SYSTEM_NAME Generic)
        set(CMAKE_SYSTEM_VERSION 1)
        cmake_minimum_required(VERSION 3.16)

        # specify cross compilers and tools
        set(CMAKE_C_COMPILER arm-none-eabi-gcc)
        set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
        set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
        set(CMAKE_AR arm-none-eabi-ar)
        set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
        set(CMAKE_OBJDUMP arm-none-eabi-objdump)
        set(SIZE arm-none-eabi-size)
        set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

        set(CMAKE_CXX_STANDARD 17)
        set(CMAKE_C_STANDARD 11)
endif ()

# project settings
project(stepMotorInf C CXX ASM)



add_compile_options(-mcpu=cortex-m0plus -mthumb -mthumb-interwork)
add_compile_options(-ffunction-sections -fdata-sections -fno-common -fmessage-length=0)

# uncomment to mitigate c++17 absolute addresses warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-register")

########################################################################################################################

set(PROJECT_TARGET stepMotorInf)
set(PROJECT_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR})
message("PROJECT_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}")

include(../init.cmake)
include(../common.cmake)

########################################################################################################################

set(HEADERS Core/Inc Drivers/STM32L0xx_HAL_Driver/Inc Drivers/STM32L0xx_HAL_Driver/Inc/Legacy Drivers/CMSIS/Device/ST/STM32L0xx/Include Drivers/CMSIS/Include ${HEADERS})

#message(STATUS "HEADERS = ${HEADERS}")
#message(STATUS "SOURCES = ${SOURCES}")
#message(STATUS "LINKER_SCRIPT = ${LINKER_SCRIPT}")

include_directories(${HEADERS})

add_definitions(-DUSE_HAL_DRIVER -DSTM32L071xx)

add_link_options(-Wl,-gc-sections,--print-memory-usage,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map,-n)
add_link_options(-mcpu=cortex-m0plus -mthumb -mthumb-interwork)
add_link_options(-T ${LINKER_SCRIPT})
add_link_options(-L ${PROJECT_TARGET_DIR})  # For linkerscript

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE}
Building ${BIN_FILE}")
