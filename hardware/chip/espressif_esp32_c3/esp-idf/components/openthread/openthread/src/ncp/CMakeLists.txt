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

set(COMMON_INCLUDES
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/src/core
)

set(COMMON_SOURCES
    changed_props_set.cpp
    ncp_base.cpp
    ncp_base_dispatcher.cpp
    ncp_base_radio.cpp
    ncp_spi.cpp
    ncp_hdlc.cpp
)

set(OT_NCP_VENDOR_HOOK_SOURCE "" CACHE STRING "set vendor hook source file for NCP")
if(OT_NCP_VENDOR_HOOK_SOURCE)
    target_compile_definitions(ot-config INTERFACE "OPENTHREAD_ENABLE_NCP_VENDOR_HOOK=1")
    list(APPEND COMMON_SOURCES ${OT_NCP_VENDOR_HOOK_SOURCE_DIR}${OT_NCP_VENDOR_HOOK_SOURCE})
endif()

set(COMMON_NCP_SOURCES
    ${COMMON_SOURCES}
    ncp_base_ftd.cpp
    ncp_base_mtd.cpp
)
option(OT_NCP_SPI "enable NCP SPI support")
if(OT_NCP_SPI)
    target_compile_definitions(ot-config INTERFACE "OPENTHREAD_CONFIG_NCP_SPI_ENABLE=1")
else()
    target_compile_definitions(ot-config INTERFACE "OPENTHREAD_CONFIG_NCP_HDLC_ENABLE=1")
endif()

if(OT_FTD)
    include(ftd.cmake)
endif()

if(OT_MTD)
    include(mtd.cmake)
endif()

if(OT_RCP)
    include(radio.cmake)
endif()

set_property(SOURCE ncp_base_mtd.cpp
    APPEND PROPERTY COMPILE_DEFINITIONS "PACKAGE_VERSION=\"${OT_PACKAGE_VERSION}\""
)
