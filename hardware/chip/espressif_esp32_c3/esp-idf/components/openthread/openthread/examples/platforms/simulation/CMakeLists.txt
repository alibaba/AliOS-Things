#
#  Copyright (c) 2019, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

set(OT_PLATFORM_LIB "openthread-simulation" PARENT_SCOPE)

add_library(ot-simulation-config INTERFACE)

option(OT_SIMULATION_VIRTUAL_TIME "enable virtual time")
if(OT_SIMULATION_VIRTUAL_TIME)
    target_compile_definitions(ot-simulation-config INTERFACE "OPENTHREAD_SIMULATION_VIRTUAL_TIME=1")
endif()

option(OT_SIMULATION_VIRTUAL_TIME_UART "enable virtual time for UART")
if(OT_SIMULATION_VIRTUAL_TIME_UART)
    target_compile_definitions(ot-simulation-config INTERFACE "OPENTHREAD_SIMULATION_VIRTUAL_TIME_UART=1")
endif()

option(OT_SIMULATION_MAX_NETWORK_SIZE "set maximum network size (default: 33)")
if(OT_SIMULATION_MAX_NETWORK_SIZE)
    target_compile_definitions(ot-simulation-config INTERFACE "OPENTHREAD_SIMULATION_MAX_NETWORK_SIZE=${OT_SIMULATION_MAX_NETWORK_SIZE}")
endif()

if(NOT OT_CONFIG)
    set(OT_CONFIG "openthread-core-simulation-config.h" PARENT_SCOPE)
endif()

list(APPEND OT_PLATFORM_DEFINES
    "_BSD_SOURCE=1"
    "_DEFAULT_SOURCE=1"
    "OPENTHREAD_EXAMPLES_SIMULATION=1"
    "OPENTHREAD_CONFIG_NCP_HDLC_ENABLE=1"
)
set(OT_PLATFORM_DEFINES ${OT_PLATFORM_DEFINES} PARENT_SCOPE)

add_library(openthread-simulation
    alarm.c
    crypto.c
    diag.c
    entropy.c
    flash.c
    infra_if.c
    logging.c
    misc.c
    radio.c
    spi-stubs.c
    system.c
    trel.c
    uart.c
    virtual_time/alarm-sim.c
    virtual_time/platform-sim.c
    $<TARGET_OBJECTS:openthread-platform-utils>
)

find_library(LIBRT rt)
if(LIBRT)
    target_link_libraries(openthread-simulation PRIVATE ${LIBRT})
endif()

target_link_libraries(openthread-simulation PRIVATE
    openthread-platform
    ot-simulation-config
    ot-config
)

target_compile_options(openthread-simulation PRIVATE
    ${OT_CFLAGS}
)

target_include_directories(openthread-simulation PRIVATE
    ${OT_PUBLIC_INCLUDES}
    ${PROJECT_SOURCE_DIR}/examples/platforms
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/core
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CPACK_PACKAGE_NAME "openthread-simulation")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "OpenThread Authors <openthread-users@googlegroups.com")
    set(CPACK_PACKAGE_CONTACT "OpenThread Authors <openthread-users@googlegroups.com")
    include(CPack)
endif()
