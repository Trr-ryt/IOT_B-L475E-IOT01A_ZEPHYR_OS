# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(iot-box LANGUAGES C CXX ASM)

target_include_directories(app PRIVATE code/app_mcu/01.inc
                                       code/app_mcu/02.cfg
                                       code/modules
                                       code/modules/bsw
                                        )

target_sources(app PRIVATE code/app_mcu/00.src/app_main.cpp)

FILE(GLOB DRV_FILES code/app_mcu/00.src/drv/*.cpp)
target_sources(app PRIVATE ${DRV_FILES})

FILE(GLOB UTL_FILES code/app_mcu/00.src/utl/*.cpp)
target_sources(app PRIVATE ${UTL_FILES})

FILE(GLOB TSK_FILES code/app_mcu/00.src/tsk/*.cpp)
target_sources(app PRIVATE ${TSK_FILES})

FILE(GLOB DT_FILES code/app_mcu/00.src/mw/*.cpp)
target_sources(app PRIVATE ${DT_FILES})

FILE(GLOB DT_FILES code/app_mcu/02.cfg/dt/*.cpp)
target_sources(app PRIVATE ${DT_FILES})

add_definitions(-D__GTEST=0 -D__mcu__=1 -D__BYTE_ORDER=__LITTLE_ENDIAN)
zephyr_compile_options(-fdiagnostics-color=always)      # Enable color for diagnostic result
zephyr_compile_options(-Wall)
