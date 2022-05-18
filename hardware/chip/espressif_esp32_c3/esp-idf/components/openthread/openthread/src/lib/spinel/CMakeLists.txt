#
#  Copyright (c) 2020, The OpenThread Authors.
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

add_library(openthread-spinel-ncp)
add_library(openthread-spinel-rcp)

target_compile_definitions(openthread-spinel-ncp PRIVATE
    OPENTHREAD_FTD=1
    OPENTHREAD_CONFIG_NCP_HDLC_ENABLE=1
    PUBLIC OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=1
)

target_compile_definitions(openthread-spinel-rcp PRIVATE
    OPENTHREAD_RADIO=1
    OPENTHREAD_CONFIG_NCP_HDLC_ENABLE=1
    PUBLIC OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=0
)

target_compile_options(openthread-spinel-ncp PRIVATE
    ${OT_CFLAGS}
)

target_compile_options(openthread-spinel-rcp PRIVATE
    ${OT_CFLAGS}
)

set(COMMON_INCLUDES
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/core
)

set(COMMON_SOURCES
    spinel.c
    spinel_buffer.cpp
    spinel_decoder.cpp
    spinel_encoder.cpp
)

target_include_directories(openthread-spinel-ncp PUBLIC ${OT_PUBLIC_INCLUDES} PRIVATE ${COMMON_INCLUDES})
target_include_directories(openthread-spinel-rcp PUBLIC ${OT_PUBLIC_INCLUDES} PRIVATE ${COMMON_INCLUDES})

target_sources(openthread-spinel-ncp PRIVATE ${COMMON_SOURCES})
target_sources(openthread-spinel-rcp PRIVATE ${COMMON_SOURCES})

target_link_libraries(openthread-spinel-ncp PRIVATE ot-config)
target_link_libraries(openthread-spinel-rcp PRIVATE ot-config)
