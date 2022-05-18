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

set(OT_MBEDTLS_DEFAULT_CONFIG_FILE \"openthread-mbedtls-config.h\")

set(OT_MBEDTLS_CONFIG_FILE "" CACHE STRING "The mbedTLS config file")

set(ENABLE_TESTING OFF CACHE BOOL "Disable mbedtls test" FORCE)
set(ENABLE_PROGRAMS OFF CACHE BOOL "Disable mbetls program" FORCE)

find_program(UNIFDEFALL_EXE unifdefall)
find_program(UNIFDEF_EXE unifdef)
if(UNIFDEF_EXE)
    execute_process(COMMAND ${UNIFDEF_EXE} -V ERROR_VARIABLE VERSION_OUTPUT)
    string(REGEX MATCH "Version: unifdef-([0-9]+\\.[0-9]+)" VERSION_MATCH "${VERSION_OUTPUT}")
    set(UNIFDEF_VERSION ${CMAKE_MATCH_1})
endif()
find_program(SED_EXE sed)

string(REPLACE "-Wconversion" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "-Wconversion" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_subdirectory(repo)

if(UNIFDEFALL_EXE AND SED_EXE AND UNIFDEF_VERSION VERSION_GREATER_EQUAL 2.10)
    add_custom_command(OUTPUT openthread-mbedtls-config.h
        COMMAND ${UNIFDEFALL_EXE}
            "'-D$<JOIN:$<TARGET_PROPERTY:ot-config,INTERFACE_COMPILE_DEFINITIONS>,';'-D>'"
            "-I$<JOIN:$<TARGET_PROPERTY:ot-config,INTERFACE_INCLUDE_DIRECTORIES>,;-I>"
            "-I$<JOIN:${OT_PUBLIC_INCLUDES},;-I>"
            "-I${CMAKE_CURRENT_SOURCE_DIR}/repo/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/mbedtls-config.h" |
            ${SED_EXE} '/openthread-core-config\.h/d' >
            openthread-mbedtls-config.h
        MAIN_DEPENDENCY mbedtls-config.h
        COMMAND_EXPAND_LISTS
    )

    add_custom_target(openthread-mbedtls-config
        DEPENDS openthread-mbedtls-config.h)

    add_dependencies(ot-config openthread-mbedtls-config)
    add_dependencies(mbedtls openthread-mbedtls-config)
    add_dependencies(mbedx509 openthread-mbedtls-config)
    add_dependencies(mbedcrypto openthread-mbedtls-config)
else()
    configure_file(mbedtls-config.h openthread-mbedtls-config.h COPYONLY)
endif()

target_include_directories(ot-config
    INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/repo/include)

target_compile_definitions(mbedtls
    PUBLIC
        "MBEDTLS_CONFIG_FILE=$<IF:$<BOOL:${OT_MBEDTLS_CONFIG_FILE}>,${OT_MBEDTLS_CONFIG_FILE},${OT_MBEDTLS_DEFAULT_CONFIG_FILE}>"
    PRIVATE
        $<TARGET_PROPERTY:ot-config,INTERFACE_COMPILE_DEFINITIONS>
)
target_include_directories(mbedtls
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/repo/include>
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
        $<TARGET_PROPERTY:ot-config,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(mbedx509
    PUBLIC
        "MBEDTLS_CONFIG_FILE=$<IF:$<BOOL:${OT_MBEDTLS_CONFIG_FILE}>,${OT_MBEDTLS_CONFIG_FILE},${OT_MBEDTLS_DEFAULT_CONFIG_FILE}>"
    PRIVATE
        $<TARGET_PROPERTY:ot-config,INTERFACE_COMPILE_DEFINITIONS>
)
target_include_directories(mbedx509
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/repo/include>
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
        $<TARGET_PROPERTY:ot-config,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(mbedcrypto
    PUBLIC
        "MBEDTLS_CONFIG_FILE=$<IF:$<BOOL:${OT_MBEDTLS_CONFIG_FILE}>,${OT_MBEDTLS_CONFIG_FILE},${OT_MBEDTLS_DEFAULT_CONFIG_FILE}>"
    PRIVATE
        $<TARGET_PROPERTY:ot-config,INTERFACE_COMPILE_DEFINITIONS>
)
target_include_directories(mbedcrypto
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/repo/include>
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
        $<TARGET_PROPERTY:ot-config,INTERFACE_INCLUDE_DIRECTORIES>
)
