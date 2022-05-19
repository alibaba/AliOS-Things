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

set(OT_PLATFORM_LIB "openthread-posix" PARENT_SCOPE)

add_library(ot-posix-config INTERFACE)

option(OT_DAEMON "Enable daemon mode" OFF)
if(OT_DAEMON)
    target_compile_definitions(ot-posix-config
        INTERFACE "OPENTHREAD_POSIX_CONFIG_DAEMON_ENABLE=1"
    )
endif()

option(OT_POSIX_VIRTUAL_TIME "enable virtual time" OFF)
if(OT_POSIX_VIRTUAL_TIME)
    target_compile_definitions(ot-posix-config
        INTERFACE "OPENTHREAD_POSIX_VIRTUAL_TIME=1"
    )
endif()

option(OT_POSIX_MAX_POWER_TABLE  "enable max power table" OFF)
if(OT_POSIX_MAX_POWER_TABLE)
    target_compile_definitions(ot-posix-config
        INTERFACE "OPENTHREAD_POSIX_CONFIG_MAX_POWER_TABLE_ENABLE=1"
    )
endif()

set(OT_POSIX_CONFIG_RCP_BUS "" CACHE STRING "RCP bus type")
if(OT_POSIX_CONFIG_RCP_BUS)
    target_compile_definitions(ot-posix-config
        INTERFACE "OPENTHREAD_POSIX_CONFIG_RCP_BUS=OT_POSIX_RCP_BUS_${OT_POSIX_CONFIG_RCP_BUS}"
    )
endif()

if(NOT OT_CONFIG)
    set(OT_CONFIG "openthread-core-posix-config.h" PARENT_SCOPE)
endif()

add_library(openthread-posix
    alarm.cpp
    backbone.cpp
    daemon.cpp
    entropy.cpp
    hdlc_interface.cpp
    infra_if.cpp
    logging.cpp
    mainloop.cpp
    memory.cpp
    misc.cpp
    multicast_routing.cpp
    netif.cpp
    radio.cpp
    radio_url.cpp
    settings.cpp
    spi_interface.cpp
    system.cpp
    trel_udp6.cpp
    udp.cpp
    virtual_time.cpp
)

target_link_libraries(openthread-posix
    PUBLIC
        openthread-platform
    PRIVATE
        openthread-url
        ot-config
        ot-posix-config
        util
        $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Linux>:rt>
)

target_compile_definitions(openthread-posix
    PUBLIC
        ${OT_PUBLIC_DEFINES}
    PRIVATE
        OPENTHREAD_FTD=1
        OPENTHREAD_MTD=0
)

target_compile_options(openthread-posix PRIVATE
    ${OT_CFLAGS}
)

target_include_directories(openthread-posix PRIVATE
    ${OT_PUBLIC_INCLUDES}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/core
    ${PROJECT_SOURCE_DIR}/third_party/mbedtls/repo/include
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src/posix/platform/include
)
