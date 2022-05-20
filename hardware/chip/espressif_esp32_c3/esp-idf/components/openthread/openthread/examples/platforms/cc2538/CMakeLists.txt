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

set(OT_PLATFORM_LIB "openthread-cc2538" PARENT_SCOPE)

if(NOT OT_CONFIG)
    set(OT_CONFIG "openthread-core-cc2538-config.h")
    set(OT_CONFIG ${OT_CONFIG} PARENT_SCOPE)
endif()

list(APPEND OT_PLATFORM_DEFINES
    "OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE=\"openthread-core-cc2538-config-check.h\""
    "OPENTHREAD_CONFIG_NCP_HDLC_ENABLE=1"
)
set(OT_PLATFORM_DEFINES ${OT_PLATFORM_DEFINES} PARENT_SCOPE)

list(APPEND OT_PLATFORM_DEFINES "OPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"${OT_CONFIG}\"")

add_library(openthread-cc2538
    alarm.c
    diag.c
    entropy.c
    flash.c
    misc.c
    radio.c
    startup-gcc.c
    system.c
    logging.c
    uart.c
    $<TARGET_OBJECTS:openthread-platform-utils>
)

target_link_libraries(openthread-cc2538
    PRIVATE
        ot-config
    PUBLIC
        -T${PROJECT_SOURCE_DIR}/examples/platforms/cc2538/cc2538.ld
        -Wl,--gc-sections -Wl,-Map=$<TARGET_PROPERTY:NAME>.map
)

target_compile_definitions(openthread-cc2538
    PUBLIC
        ${OT_PLATFORM_DEFINES}
)

target_compile_options(openthread-cc2538 PRIVATE
    ${OT_CFLAGS}
)

target_include_directories(openthread-cc2538 PRIVATE
    ${OT_PUBLIC_INCLUDES}
    ${PROJECT_SOURCE_DIR}/examples/platforms
    ${PROJECT_SOURCE_DIR}/src/core
)
