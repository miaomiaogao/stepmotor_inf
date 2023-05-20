message(STATUS "root")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_AR arm-none-eabi-ar)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(stepmotor C CXX ASM)

message(STATUS "Set target stepMotorInf")
add_subdirectory(targets/stepMotorInf)

########################################################################################################################
# Useful for debugging...

#message(STATUS "TARGET = ${PROJECT_TARGET}")
#message(STATUS "NAME   = ${PROJECT_NAME}")
#message(STATUS "CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")
#message(STATUS "CMAKE_CURRENT_BINARY_DIR = ${CMAKE_CURRENT_BINARY_DIR}")
#message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
#message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")

#message(STATUS "PROJECT_ROOT=${PROJECT_ROOT}")
#message(STATUS "PROJECT_ROOT_SRC=${PROJECT_ROOT_SRC}")
#message(STATUS "PROJECT_ROOT_SHARED=${PROJECT_ROOT_SHARED}")
#message(STATUS "PROJECT_TARGET_DIR=${PROJECT_TARGET_DIR}")
#message(STATUS "PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")
#message(STATUS "PROJECT_BINARY_DIR = ${PROJECT_BINARY_DIR}")

#message(STATUS "LINKER_SCRIPTNAME = ${LINKER_SCRIPTNAME}")
#message(STATUS "LINKER_SCRIPT = ${LINKER_SCRIPT}")

########################################################################################################################

