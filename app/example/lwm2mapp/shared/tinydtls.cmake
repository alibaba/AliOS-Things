cmake_minimum_required (VERSION 3.0)

# List source files
set(TINYDTLS_SOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/tinydtls)
set(TINYDTLS_SOURCES
    ${TINYDTLS_SOURCES_DIR}/tinydtls.h
    ${TINYDTLS_SOURCES_DIR}/dtls.h
    ${TINYDTLS_SOURCES_DIR}/dtls.c
    ${TINYDTLS_SOURCES_DIR}/crypto.c
    ${TINYDTLS_SOURCES_DIR}/ccm.c
    ${TINYDTLS_SOURCES_DIR}/hmac.c
    ${TINYDTLS_SOURCES_DIR}/dtls_debug.c
    ${TINYDTLS_SOURCES_DIR}/netq.c
    ${TINYDTLS_SOURCES_DIR}/peer.c
    ${TINYDTLS_SOURCES_DIR}/dtls_time.c
    ${TINYDTLS_SOURCES_DIR}/session.c
    ${TINYDTLS_SOURCES_DIR}/sha2/sha2.c
    ${TINYDTLS_SOURCES_DIR}/aes/rijndael.c
    ${TINYDTLS_SOURCES_DIR}/sha2/sha2.c
    ${TINYDTLS_SOURCES_DIR}/ecc/ecc.c)

set(TINYDTLS_SOURCES_GENERATED
    ${TINYDTLS_SOURCES_DIR}/dtls_config.h)

# source files are only available after tinydtls submodule have been checked out.
# Create a target "submodule_update" for that purpose.
find_package(Git REQUIRED)
add_custom_command(OUTPUT ${TINYDTLS_SOURCES} COMMAND ${GIT_EXECUTABLE} submodule update)
add_custom_target(submodule_update SOURCES ${TINYDTLS_SOURCES})

get_filename_component( COMPILER_FILENAME "${CMAKE_C_COMPILER}" NAME )
string( REGEX REPLACE "-[^-]+$" ""
        TOOLCHAIN_NAME "${COMPILER_FILENAME}" )

# The tinydtls configure step will create some more source files (tinydtls.h etc).
# Use cmake "External Project" modul to call autoreconf and configure on tinydtls if necessary.
if (NOT EXISTS ${TINYDTLS_SOURCES_GENERATED})
    include(ExternalProject)
    ExternalProject_Add(external_tinydtls
        SOURCE_DIR  "${TINYDTLS_SOURCES_DIR}"
        DOWNLOAD_COMMAND ""
        UPDATE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        CONFIGURE_COMMAND ${TINYDTLS_SOURCES_DIR}/configure --host=${TOOLCHAIN_NAME}
        BUILD_IN_SOURCE 1
        LOG_DOWNLOAD 1
        LOG_CONFIGURE 1
        # Make the submodule_update target a dependency.
        DEPENDS submodule_update
        )

    ExternalProject_Add_Step(external_tinydtls autoheader
       COMMAND "autoheader" 
       ALWAYS 1
       WORKING_DIRECTORY "${TINYDTLS_SOURCES_DIR}"
       DEPENDERS configure
       DEPENDEES autoconf
    )

    ExternalProject_Add_Step(external_tinydtls autoconf
       COMMAND "autoconf" 
       ALWAYS 1
       WORKING_DIRECTORY "${TINYDTLS_SOURCES_DIR}"
       DEPENDERS autoheader
       DEPENDEES download
    )

    # Let cmake know that it needs to execute the external_tinydtls target to generate those files.
    add_custom_command(OUTPUT ${TINYDTLS_SOURCES_GENERATED} COMMAND ""  DEPENDS external_tinydtls)
endif()

set(TINYDTLS_SOURCES ${TINYDTLS_SOURCES} ${TINYDTLS_SOURCES_GENERATED})

# Compile definitions for tinydtls
set_source_files_properties(${TINYDTLS_SOURCES} PROPERTIES COMPILE_DEFINITIONS WITH_SHA256)

