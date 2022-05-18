#
#  Copyright (c) 2021, The OpenThread Authors.
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

project("TCPlp" C)

set(src_tcplp
    lib/bitmap.c
    lib/cbuf.c
    lib/lbuf.c
)

set(tcplp_static_target "tcplp")

add_library(${tcplp_static_target} STATIC ${src_tcplp})
set_target_properties(${tcplp_static_target} PROPERTIES OUTPUT_NAME tcplp)
target_include_directories(${tcplp_static_target}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/bsdtcp
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
)

target_link_libraries(${tcplp_static_target}
    PRIVATE
        ot-config
)

# TCPlp calls functions that are defined by the core OpenThread (like
# "otMessageWrite()"), so we need to add the core library (FTD or MTD, as
# appropriate) as a link dependency.

if(OT_FTD)
    target_link_libraries(${tcplp_static_target}
        PRIVATE
            openthread-ftd
    )
endif()

if(OT_MTD)
    target_link_libraries(${tcplp_static_target}
        PRIVATE
            openthread-mtd
    )
endif()
