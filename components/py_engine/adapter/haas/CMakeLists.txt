cmake_minimum_required(VERSION 3.12)

# Set main target and component locations
set(MICROPY_TARGET micropython)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Define the top-level project
project(${MICROPY_TARGET})

set(HAAS_PLATFORM TRUE)

# Set location of base MicroPython directory.
if(NOT MICROPY_DIR)
    get_filename_component(MICROPY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../engine ABSOLUTE)
endif()

# Set the location of AOS components directory.
if(NOT AOS_COMPONENTS_DIR)
    get_filename_component(AOS_COMPONENTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../ ABSOLUTE)
endif()

# Set the location of AOS hardware directory.
if(NOT AOS_HARDWARE_DIR)
    get_filename_component(AOS_HARDWARE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../hardware ABSOLUTE)
endif()

# Set the location of AOS hardware directory.
if(NOT AOS_KERNEL_DIR)
    get_filename_component(AOS_KERNEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../kernel ABSOLUTE)
endif()

# Set the location of AOS components directory.
if(NOT HAAS_ENGINE_DIR)
    get_filename_component(HAAS_ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ ABSOLUTE)
endif()

# Include core extmod components.
include(${MICROPY_DIR}/../modules/portmod.cmake)
include(${MICROPY_DIR}/../external/external.cmake)

# Include core source components.
include(${MICROPY_DIR}/py/py.cmake)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(${MICROPY_DIR}/py/usermod.cmake)
    include(${MICROPY_DIR}/extmod/extmod.cmake)
endif()

set(MICROPY_QSTRDEFS_PORT
    ${PROJECT_SOURCE_DIR}/qstrdefsport.h
)

set(MICROPY_SOURCE_SHARED
    ${MICROPY_DIR}/shared/readline/readline.c
    ${MICROPY_DIR}/shared/netutils/netutils.c
    ${MICROPY_DIR}/shared/timeutils/timeutils.c
    ${MICROPY_DIR}/shared/runtime/interrupt_char.c
    ${MICROPY_DIR}/shared/runtime/stdout_helpers.c
    ${MICROPY_DIR}/shared/runtime/sys_stdio_mphal.c
    ${MICROPY_DIR}/shared/runtime/pyexec.c
    ${MICROPY_DIR}/shared/runtime/gchelper_generic.c
)

set(MICROPY_SOURCE_LIB
    # ${MICROPY_DIR}/lib/littlefs/lfs1.c
    # ${MICROPY_DIR}/lib/littlefs/lfs1_util.c
    # ${MICROPY_DIR}/lib/littlefs/lfs2.c
    # ${MICROPY_DIR}/lib/littlefs/lfs2_util.c
    # ${MICROPY_DIR}/lib/mbedtls_errors/mp_mbedtls_errors.c
    # ${MICROPY_DIR}/lib/oofatfs/ff.c
    # ${MICROPY_DIR}/lib/oofatfs/ffunicode.c
)

set(MICROPY_SOURCE_DRIVERS
    ${MICROPY_DIR}/drivers/bus/softspi.c
    ${MICROPY_DIR}/drivers/dht/dht.c
)

set(MICROPY_SOURCE_PORT
    ${PROJECT_SOURCE_DIR}/gccollect.c
    ${PROJECT_SOURCE_DIR}/help.c
    ${PROJECT_SOURCE_DIR}/machine_adc.c
    ${PROJECT_SOURCE_DIR}/machine_dac.c
    ${PROJECT_SOURCE_DIR}/machine_hw_i2c.c
    ${PROJECT_SOURCE_DIR}/machine_hw_spi.c
    ${PROJECT_SOURCE_DIR}/machine_pin.c
    ${PROJECT_SOURCE_DIR}/machine_pwm.c
    ${PROJECT_SOURCE_DIR}/machine_rtc.c
    ${PROJECT_SOURCE_DIR}/machine_sw_timer.c
    ${PROJECT_SOURCE_DIR}/machine_timer.c
    ${PROJECT_SOURCE_DIR}/machine_touchpad.c
    ${PROJECT_SOURCE_DIR}/machine_uart.c
    ${PROJECT_SOURCE_DIR}/machine_wdt.c
    ${PROJECT_SOURCE_DIR}/modbattery.c
    ${PROJECT_SOURCE_DIR}/modcharger.c
    ${PROJECT_SOURCE_DIR}/modipc.c
    ${PROJECT_SOURCE_DIR}/modmachine.c
    ${PROJECT_SOURCE_DIR}/modpm.c
    ${PROJECT_SOURCE_DIR}/modsocket.c
    ${PROJECT_SOURCE_DIR}/modsysteminfo.c
    ${PROJECT_SOURCE_DIR}/moduos.c
    ${PROJECT_SOURCE_DIR}/modutime.c
    ${PROJECT_SOURCE_DIR}/mphalport.c
    ${PROJECT_SOURCE_DIR}/mpsalport.c
    ${PROJECT_SOURCE_DIR}/mpstdinport.c
    ${PROJECT_SOURCE_DIR}/mpthreadport.c
    ${PROJECT_SOURCE_DIR}/utility.c
)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_USERMOD}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_PORT}
    ${MICROPY_SOURCE_MODULES_PORT}
    ${MICROPY_SOURCE_EXTERNAL}
    ${LV_SRC}
)

set(AOS_SDK_COMPONENTS
    mbedtls
)

# Define mpy-cross flags, for use with frozen code.
set(MICROPY_CROSS_FLAGS -march=armv7m)

# Set the location of this port's directory.
set(MICROPY_PORT_DIR ${CMAKE_SOURCE_DIR})

# Set the board if it's not already set.
if(NOT MICROPY_BOARD)
    set(MICROPY_BOARD HaaSEDUk1)
endif()

# Set the board directory and check that it exists.
if(NOT MICROPY_BOARD_DIR)
    set(MICROPY_BOARD_DIR ${MICROPY_PORT_DIR}/boards/${MICROPY_BOARD})
endif()
if(NOT EXISTS ${MICROPY_BOARD_DIR}/mpconfigboard.cmake)
    message(FATAL_ERROR "Invalid MICROPY_BOARD specified: ${MICROPY_BOARD}")
endif()

# Include board config
include(${MICROPY_BOARD_DIR}/mpconfigboard.cmake) 

add_library(${MICROPY_TARGET})

# Set compile options for this port.
target_compile_definitions(${MICROPY_TARGET} PUBLIC
    ${MICROPY_DEF_MODULES_PORT}
    ${MICROPY_DEF_CORE}
    FFCONF_H=\"${MICROPY_OOFATFS_DIR}/ffconf.h\"
    LFS1_NO_MALLOC LFS1_NO_DEBUG LFS1_NO_WARN LFS1_NO_ERROR LFS1_NO_ASSERT
    LFS2_NO_MALLOC LFS2_NO_DEBUG LFS2_NO_WARN LFS2_NO_ERROR LFS2_NO_ASSERT
)

if (NOT MICROPY_FROZEN_MANIFEST)
    set(MICROPY_FROZEN_MANIFEST ${PROJECT_SOURCE_DIR}/../boards/manifest.py)
endif()

target_sources(${MICROPY_TARGET} PRIVATE
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_DRIVERS}
    ${MICROPY_SOURCE_PORT}
    ${MICROPY_SOURCE_USERMOD}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_MODULES_PORT}
    ${MICROPY_SOURCE_EXTERNAL}
    ${LV_SRC}
)

target_include_directories(${MICROPY_TARGET} PRIVATE
    ${MICROPY_INC_CORE}
    ${MICROPY_INC_USERMOD}
    ${MICROPY_BOARD_DIR}
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MICROPY_INC_MODULES_PORT}
    ${MICROPY_INC_EXTERNAL}
)

target_link_libraries(${MICROPY_TARGET}
    ${AOS_SDK_COMPONENTS}
)

# Disable some warnings to keep the build output clean.
target_compile_options(${MICROPY_TARGET} PUBLIC
    -Wno-clobbered
    -Wno-deprecated-declarations
    -Wno-missing-field-initializers
)

target_link_libraries(${MICROPY_TARGET} micropy_extmod_btree)
target_link_libraries(${MICROPY_TARGET} usermod)

# Collect all the include directories and compile definitions for the aos-sdk components.
foreach(comp ${AOS_SDK_COMPONENTS})
    micropy_gather_target_properties(${comp})
endforeach()

target_include_directories(${MICROPY_TARGET} PRIVATE 
    ${AOS_COMPONENTS_DIR}/mbedtls/include
    ${AOS_COMPONENTS_DIR}/mbedtls/platform/include
    ${AOS_COMPONENTS_DIR}/mbedtls/platform/aos/include
    ${AOS_COMPONENTS_DIR}/lwip/lwip2.0.0/src/include
    ${AOS_COMPONENTS_DIR}/lwip/lwip2.0.0/include
    ${AOS_COMPONENTS_DIR}/fatfs/include
    ${AOS_COMPONENTS_DIR}/drivers/peripheral/uart/include
    ${AOS_COMPONENTS_DIR}/drivers/peripheral/gpio/include
    ${AOS_COMPONENTS_DIR}/drivers/core/base/include
    ${AOS_COMPONENTS_DIR}/osal_aos/include/
    ${AOS_HARDWARE_DIR}/chip/haas1000/aos/arch/
    ${AOS_HARDWARE_DIR}/chip/haas1000/aos
    ${AOS_HARDWARE_DIR}/chip/haas1000/drivers/rtos/rhino
    ${AOS_HARDWARE_DIR}/chip/haas1000/drivers/rtos/rhino/include
    ${AOS_KERNEL_DIR}/rhino/include
    ${AOS_HARDWARE_DIR}/chip/haas1000/drivers/platform/hal
    ${AOS_HARDWARE_DIR}/arch/armv7m/include
    ${AOS_HARDWARE_DIR}/chip/haas1000
)

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)
