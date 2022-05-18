# CMakeLists.txt for libcoap
#
# Copyright (C) 2020 Carlos Gomes Martinho <carlos.gomes_martinho@siemens.com>
# Copyright (C) 2020-2021 Jon Shallow <supjps-libcoap@jpshallow.com>
#
# SPDX-License-Identifier: BSD-2-Clause
#
# This file is part of the CoAP library libcoap. Please see README for terms
# of use.

cmake_minimum_required(VERSION 3.10)

project(
  libcoap
  VERSION 4.3.0
  LANGUAGES CXX C)

set(LIBCOAP_API_VERSION 3)
set(COAP_LIBRARY_NAME "coap-${LIBCOAP_API_VERSION}")

option(
  BUILD_SHARED_LIBS
  "Build shared libs"
  OFF)

add_library(${COAP_LIBRARY_NAME})

#
# options to tweak the library
#

option(
  ENABLE_DTLS
  "Enable building with DTLS support"
  ON)
set(DTLS_BACKEND
    "default"
    CACHE
      STRING
      "\
Name of the dtls backend, only relevant if `ENABLE_DTLS` is ON which is default. \
Possible values: default, gnutls, openssl, tinydtls and mbedtls. \
If specified then this library will be searched and if found also used. \
If not found then the cmake configuration will stop with an error. \
If not specified, then cmake will try to use the first one found in the following order: \
gnutls, openssl, tinydtls, mbedtls \
    ")
set_property(
  CACHE DTLS_BACKEND
  PROPERTY STRINGS
           default
           openssl
           gnutls
           tinydtls
           mbedtls)
option(
  USE_VENDORED_TINYDTLS
  "compile with the tinydtls project in the submodule if on, otherwise try to find the compiled lib with find_package"
  ON)
option(
  WITH_EPOLL
  "compile with epoll support"
  ON)
option(
  ENABLE_SMALL_STACK
  "Define if the system has small stack size"
  OFF)
option(
  ENABLE_TCP
  "Enable building with TCP support"
  ON)
option(
  ENABLE_TESTS
  "build also tests"
  OFF)
option(
  ENABLE_EXAMPLES
  "build also examples"
  ON)
option(
  ENABLE_DOCS
  "build also doxygen documentation"
  ON)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(APPLE)
  add_definitions(-D__APPLE_USE_RFC_3542=1)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CheckCSourceCompiles)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckSymbolExists)
include(CheckTypeSize)
include(TestBigEndian)

# check for headers
check_include_file(byteswap.h HAVE_BYTESWAP_H)
check_include_file(inttypes.h HAVE_INTTYPES_H)
check_include_file(limits.h HAVE_LIMITS_H)
check_include_file(memory.h HAVE_MEMORY_H)
check_include_file(strings.h HAVE_STRINGS_H)
check_include_file(string.h HAVE_STRING_H)
check_include_file(sys/sysctl.h HAVE_SYS_SYSCTL_H)
check_include_file(net/if.h HAVE_NET_IF_H)
check_include_file(netinet/in.h HAVE_NETINET_IN_H)
check_include_file(sys/epoll.h HAVE_EPOLL_H)
check_include_file(sys/timerfd.h HAVE_TIMERFD_H)
check_include_file(arpa/inet.h HAVE_ARPA_INET_H)
check_include_file(stdbool.h HAVE_STDBOOL_H)
check_include_file(netdb.h HAVE_NETDB_H)
check_include_file(pthread.h HAVE_PTHREAD_H)
check_include_file(stdlib.h HAVE_STDINT_H)
check_include_file(stdint.h HAVE_STDLIB_H)
check_include_file(syslog.h HAVE_SYSLOG_H)
check_include_file(sys/ioctl.h HAVE_SYS_IOCTL_H)
check_include_file(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_file(sys/stat.h HAVE_SYS_STAT_H)
check_include_file(sys/time.h HAVE_SYS_TIME_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(sys/unistd.h HAVE_SYS_UNISTD_H)
check_include_file(time.h HAVE_TIME_H)
check_include_file(unistd.h HAVE_UNISTD_H)
check_include_file(float.h HAVE_FLOAT_H)
check_include_file(stddef.h HAVE_STDDEF_H)
check_include_file(winsock2.h HAVE_WINSOCK2_H)
check_include_file(ws2tcpip.h HAVE_WS2TCPIP_H)

# check for functions
check_function_exists(malloc HAVE_MALLOC)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(select HAVE_SELECT)
check_function_exists(socket HAVE_SOCKET)
check_function_exists(strcasecmp HAVE_STRCASECMP)
check_function_exists(pthread_mutex_lock HAVE_PTHREAD_MUTEX_LOCK)
check_function_exists(getaddrinfo HAVE_GETADDRINFO)
check_function_exists(strnlen HAVE_STRNLEN)
check_function_exists(strrchr HAVE_STRRCHR)
check_function_exists(getrandom HAVE_GETRANDOM)
check_function_exists(if_nametoindex HAVE_IF_NAMETOINDEX)

# check for symbols
if(WIN32)
  set(HAVE_STRUCT_CMSGHDR 1)
else()
  check_symbol_exists(
    cmsghdr
    sys/socket.h
    HAVE_STRUCT_CMSGHDR)
endif()

if(${WITH_EPOLL}
   AND ${HAVE_EPOLL_H}
   AND ${HAVE_TIMERFD_H})
  set(COAP_EPOLL_SUPPORT "1")
  message(STATUS "compiling with epoll support")
else()
  message(STATUS "compiling without epoll support")
endif()

if(ENABLE_SMALL_STACK)
  set(ENABLE_SMALL_STACK "${ENABLE_SMALL_STACK}")
  message(STATUS "compiling with small stack support")
endif()

set(WITH_GNUTLS OFF)
set(WITH_OPENSSL OFF)
set(WITH_TINYDTLS OFF)
set(WITH_MBEDTLS OFF)

function(compile_tinydtls)
  set(TINYDTLS_SOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/ext/tinydtls)
  set(TINYDTLS_SOURCES_GENERATED ${TINYDTLS_SOURCES_DIR}/dtls_config.h)

  message(STATUS "compiling the tinydtls lib")

  include(ExternalProject)

  externalproject_add(
    external_tinydtls
    SOURCE_DIR "${TINYDTLS_SOURCES_DIR}"
    BUILD_IN_SOURCE 1
    DOWNLOAD_COMMAND ""
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND
      ${TINYDTLS_SOURCES_DIR}/configure
      --disable-manpages
      --prefix=${CMAKE_BINARY_DIR}
    BUILD_COMMAND make install
    INSTALL_COMMAND ""
    LOG_DOWNLOAD 1
    LOG_CONFIGURE 1)

  externalproject_add_step(
    external_tinydtls autoreconf
    COMMAND autoreconf --force --install
    ALWAYS 1
    WORKING_DIRECTORY "${TINYDTLS_SOURCES_DIR}"
    DEPENDERS configure
    DEPENDEES download)

  # Let cmake know that it needs to execute the external_tinydtls target to generate those files.
  add_custom_command(
    OUTPUT ${TINYDTLS_SOURCES_GENERATED}
    WORKING_DIRECTORY "${TINYDTLS_SOURCES_DIR}"
    COMMAND "make install"
    DEPENDS external_tinydtls)

  add_dependencies(${COAP_LIBRARY_NAME} external_tinydtls)

  if(BUILD_SHARED_LIBS)
    set(LIBTINYDTLS_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib/libtinydtls.so")
  else()
    set(LIBTINYDTLS_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib/libtinydtls.a")
  endif()

  add_library(
    tinydtls
    UNKNOWN
    IMPORTED)
  set_target_properties(
    tinydtls
    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
               "${CMAKE_CURRENT_BINARY_DIR}/include"
               IMPORTED_LINK_INTERFACE_LANGUAGES "C"
               IMPORTED_LOCATION "${LIBTINYDTLS_PATH}")

endfunction()

if(ENABLE_DTLS)
  message(STATUS "compiling with DTLS support")
  message(STATUS "DTLS_BACKEND: ${DTLS_BACKEND}")

  if(DTLS_BACKEND
     STREQUAL
     "default")
    # try to find a crypto lib and use it, use the first one found

    # libgnutls (e.g. debian libgnutls28-dev)
    find_package(GnuTLS)
    if(GnuTLS_FOUND)
      set(WITH_GNUTLS ON)
      message(STATUS "compiling with gnutls support")
      set(HAVE_LIBGNUTLS 1)
    else()
      # gnutls not found
      find_package(OpenSSL)
      if(OpenSSL_FOUND)
        set(WITH_OPENSSL ON)
        message(STATUS "compiling with openssl support")
        set(HAVE_OPENSSL 1)
      else()
        # openssl not found
        # libmbedtls (e.g. debian libmbedtls-dev)
        find_package(MbedTLS)
        if(MbedTLS_FOUND)
          set(WITH_MBEDTLS ON)
          message(STATUS "compiling with mbedtls support")
          set(HAVE_MBEDTLS 1)
        else()
          # mbedtls not found
          if(USE_VENDORED_TINYDTLS)
            compile_tinydtls()
          else()
            find_package(TinyDTLS)
            if(TINYDTLS_FOUND)

            else()
              # no cryto lib found
              message(
                FATAL_ERROR
                  "cannot find any cryto lib, either install one or compile without DTLS support"
              )
            endif()

          endif()

          set(WITH_TINYDTLS ON)
          message(STATUS "compiling with tinydtls support")
          set(HAVE_LIBTINYDTLS 1)

        endif()

      endif()

    endif()

  else()
    # DTLS_BACKEND variable is not empty, so set all to false and set the only right to true
    set(WITH_GNUTLS OFF)
    set(WITH_TINYDTLS OFF)
    set(WITH_MBEDTLS OFF)
    set(WITH_OPENSSL OFF)

    if(DTLS_BACKEND
       STREQUAL
       "gnutls")
      # libgnutls (e.g. debian libgnutls28-dev)
      find_package(GnuTLS REQUIRED)
      set(WITH_GNUTLS ON)
      message(STATUS "compiling with gnutls support")
      set(HAVE_LIBGNUTLS 1)
    endif()

    if(DTLS_BACKEND
       STREQUAL
       "openssl")
      # libssl (e.g. debian libssl1.0-dev)
      find_package(OpenSSL REQUIRED)
      set(WITH_OPENSSL ON)
      message(STATUS "compiling with openssl support")
      set(HAVE_OPENSSL 1)
    endif()

    if(DTLS_BACKEND
       STREQUAL
       "mbedtls")
      # libmbedtls (e.g. debian libmbedtls-dev)
      find_package(MbedTLS REQUIRED)
      set(WITH_MBEDTLS ON)
      message(STATUS "compiling with mbedtls support")
      set(HAVE_MBEDTLS 1)
    endif()

    if(DTLS_BACKEND
       STREQUAL
       "tinydtls")

      if(USE_VENDORED_TINYDTLS)
        compile_tinydtls()
      else(USE_VENDORED_TINYDTLS)
        find_package(TinyDTLS REQUIRED)
      endif(USE_VENDORED_TINYDTLS)

      message(STATUS "compiling with tinydtls support")
      set(WITH_TINYDTLS ON)
      set(HAVE_LIBTINYDTLS 1)

    endif()

  endif()

endif()

message(STATUS "ENABLE_DTLS:.....................${ENABLE_DTLS}")
message(STATUS "ENABLE_TCP:......................${ENABLE_TCP}")
message(STATUS "ENABLE_DOCS:.....................${ENABLE_DOCS}")
message(STATUS "ENABLE_EXAMPLES:.................${ENABLE_EXAMPLES}")
message(STATUS "DTLS_BACKEND:....................${DTLS_BACKEND}")
message(STATUS "WITH_GNUTLS:.....................${WITH_GNUTLS}")
message(STATUS "WITH_TINYDTLS:...................${WITH_TINYDTLS}")
message(STATUS "WITH_OPENSSL:....................${WITH_OPENSSL}")
message(STATUS "WITH_MBEDTLS:....................${WITH_MBEDTLS}")
message(STATUS "HAVE_LIBTINYDTLS:................${HAVE_LIBTINYDTLS}")
message(STATUS "HAVE_LIBGNUTLS:..................${HAVE_LIBGNUTLS}")
message(STATUS "HAVE_OPENSSL:....................${HAVE_OPENSSL}")
message(STATUS "HAVE_MBEDTLS:....................${HAVE_MBEDTLS}")
message(STATUS "COAP_EPOLL_SUPPORT:..............${COAP_EPOLL_SUPPORT}")
message(STATUS "CMAKE_C_COMPILER:................${CMAKE_C_COMPILER}")
message(STATUS "BUILD_SHARED_LIBS:...............${BUILD_SHARED_LIBS}")
message(STATUS "CMAKE_BUILD_TYPE:................${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR:..........${CMAKE_SYSTEM_PROCESSOR}")

set(PACKAGE_URL "https://libcoap.net/")
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_TARNAME "${PROJECT_NAME}")
set(PACKAGE_STRING "${PROJECT_NAME} ${PROJECT_VERSION}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(LIBCOAP_PACKAGE_VERSION "${PROJECT_VERSION}")
set(LIBCOAP_PACKAGE_URL "${PACKAGE_URL}")
set(LIBCOAP_PACKAGE_NAME "${PROJECT_NAME}")
set(LIBCOAP_PACKAGE_STRING "TODO")
set(LIBCOAP_PACKAGE_BUGREPORT "libcoap-developers@lists.sourceforge.net")
set(PACKAGE_BUGREPORT "libcoap-developers@lists.sourceforge.net")
set(top_srcdir "${CMAKE_CURRENT_LIST_DIR}")
set(top_builddir "${CMAKE_CURRENT_BINARY_DIR}")
if(ENABLE_TCP)
  set(COAP_DISABLE_TCP 0)
else(ENABLE_TCP)
  set(COAP_DISABLE_TCP 1)
endif(ENABLE_TCP)

# creates config header file in build directory
configure_file(${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/include/coap${LIBCOAP_API_VERSION}/coap.h)

configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake_coap_config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/coap_config.h)

configure_file(${CMAKE_CURRENT_LIST_DIR}/tests/test_common.h.in
               ${CMAKE_CURRENT_LIST_DIR}/tests/test_common.h)

#
# sources
#

target_sources(
  ${COAP_LIBRARY_NAME}
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/address.c
          ${CMAKE_CURRENT_LIST_DIR}/src/net.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_debug.c
          ${CMAKE_CURRENT_LIST_DIR}/src/encode.c
          ${CMAKE_CURRENT_LIST_DIR}/src/uri.c
          ${CMAKE_CURRENT_LIST_DIR}/src/subscribe.c
          ${CMAKE_CURRENT_LIST_DIR}/src/resource.c
          ${CMAKE_CURRENT_LIST_DIR}/src/str.c
          ${CMAKE_CURRENT_LIST_DIR}/src/option.c
          ${CMAKE_CURRENT_LIST_DIR}/src/async.c
          ${CMAKE_CURRENT_LIST_DIR}/src/block.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_asn1.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_cache.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_debug.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_event.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_hashkey.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_io.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_notls.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_prng.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_session.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_tcp.c
          ${CMAKE_CURRENT_LIST_DIR}/src/coap_time.c
          ${CMAKE_CURRENT_LIST_DIR}/src/encode.c
          ${CMAKE_CURRENT_LIST_DIR}/src/mem.c
          ${CMAKE_CURRENT_LIST_DIR}/src/net.c
          ${CMAKE_CURRENT_LIST_DIR}/src/option.c
          ${CMAKE_CURRENT_LIST_DIR}/src/pdu.c
          ${CMAKE_CURRENT_LIST_DIR}/src/resource.c
          ${CMAKE_CURRENT_LIST_DIR}/src/str.c
          ${CMAKE_CURRENT_LIST_DIR}/src/subscribe.c
          ${CMAKE_CURRENT_LIST_DIR}/src/uri.c
          # no need to parse those files if we do not need them
          $<$<BOOL:${HAVE_OPENSSL}>:${CMAKE_CURRENT_LIST_DIR}/src/coap_openssl.c>
          $<$<BOOL:${HAVE_LIBTINYDTLS}>:${CMAKE_CURRENT_LIST_DIR}/src/coap_tinydtls.c>
          $<$<BOOL:${HAVE_LIBGNUTLS}>:${CMAKE_CURRENT_LIST_DIR}/src/coap_gnutls.c>
          $<$<BOOL:${HAVE_MBEDTLS}>:${CMAKE_CURRENT_LIST_DIR}/src/coap_mbedtls.c>
          # headers
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/address.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/async.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/block.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_cache.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_debug.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_dtls.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_event.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_hashkey.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_io.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_session.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_time.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/encode.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/libcoap.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/lwippools.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/mem.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/net.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/option.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/pdu.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/coap_prng.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/resource.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/str.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/subscribe.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/uri.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/uthash.h
          ${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}/utlist.h)
target_include_directories(
  ${COAP_LIBRARY_NAME}
  PUBLIC # config headers are generated during configuration time
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/>
         $<INSTALL_INTERFACE:include/>
         $<$<AND:$<BOOL:${HAVE_LIBTINYDTLS}>,$<BOOL:${USE_VENDORED_TINYDTLS}>>:${CMAKE_BINARY_DIR}/include/tinydtls>
         $<$<BOOL:${HAVE_LIBGNUTLS}>:${GNUTLS_INCLUDE_DIR}>
         $<$<BOOL:${HAVE_MBEDTLS}>:${MBEDTLS_INCLUDE_DIRS}>)
target_link_libraries(
  ${COAP_LIBRARY_NAME}
  PUBLIC $<$<BOOL:${HAVE_OPENSSL}>:OpenSSL::SSL>
         $<$<BOOL:${HAVE_OPENSSL}>:OpenSSL::Crypto>
         $<$<BOOL:${HAVE_LIBGNUTLS}>:${GNUTLS_LIBRARIES}>
         $<$<BOOL:${HAVE_LIBTINYDTLS}>:tinydtls>
         $<$<BOOL:${HAVE_MBEDTLS}>:${MBEDTLS_LIBRARY}>
         $<$<BOOL:${HAVE_MBEDTLS}>:${MBEDX509_LIBRARY}>
         $<$<BOOL:${HAVE_MBEDTLS}>:${MBEDCRYPTO_LIBRARY}>)

add_library(
  ${PROJECT_NAME}::${COAP_LIBRARY_NAME}
  ALIAS
  ${COAP_LIBRARY_NAME})

#
# compiler options
#

add_compile_options(
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-pedantic>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wcast-qual>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wextra>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wformat-security>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Winline>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wmissing-declarations>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wmissing-prototypes>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wnested-externs>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wpointer-arith>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wshadow>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wstrict-prototypes>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wswitch-default>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wswitch-enum>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wunused>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wwrite-strings>)

#
# tests
#

if(ENABLE_TESTS)
  add_executable(
    testdriver
    ${CMAKE_CURRENT_LIST_DIR}/tests/testdriver.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_common.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_encode.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_encode.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_error_response.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_error_response.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_options.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_options.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_pdu.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_pdu.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_sendqueue.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_sendqueue.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_session.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_session.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_tls.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_tls.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_uri.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_uri.h
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_wellknown.c
    ${CMAKE_CURRENT_LIST_DIR}/tests/test_wellknown.h)
  # tests require libcunit (e.g. debian libcunit1-dev)
  target_link_libraries(testdriver PUBLIC ${PROJECT_NAME}::${COAP_LIBRARY_NAME}
                                          -lcunit)
endif()

#
# examples
#

if(ENABLE_EXAMPLES)
  add_executable(coap-client ${CMAKE_CURRENT_LIST_DIR}/examples/coap-client.c)
  target_link_libraries(coap-client
                        PUBLIC ${PROJECT_NAME}::${COAP_LIBRARY_NAME})

  add_executable(coap-rd ${CMAKE_CURRENT_LIST_DIR}/examples/coap-rd.c)
  target_include_directories(coap-rd
    PRIVATE
         $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/coap${LIBCOAP_API_VERSION}>)
  target_link_libraries(coap-rd PUBLIC ${PROJECT_NAME}::${COAP_LIBRARY_NAME})

  add_executable(coap-server ${CMAKE_CURRENT_LIST_DIR}/examples/coap-server.c)
  target_link_libraries(coap-server
                        PUBLIC ${PROJECT_NAME}::${COAP_LIBRARY_NAME})

  if(NOT WIN32)
    add_executable(etsi_iot_01 ${CMAKE_CURRENT_LIST_DIR}/examples/etsi_iot_01.c)
    target_link_libraries(etsi_iot_01
                          PUBLIC ${PROJECT_NAME}::${COAP_LIBRARY_NAME})

    add_executable(tiny ${CMAKE_CURRENT_LIST_DIR}/examples/tiny.c)
    target_link_libraries(tiny PUBLIC ${PROJECT_NAME}::${COAP_LIBRARY_NAME})
  endif()
endif()

#
# docs
#

if(ENABLE_DOCS)
  find_package(Doxygen)

  if(Doxygen_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # Make necessary temporary directories
    file(MAKE_DIRECTORY  ${CMAKE_BINARY_DIR}/doc/man_tmp)
    file(MAKE_DIRECTORY  ${CMAKE_BINARY_DIR}/doc/man_html)

    # request to configure the file
    configure_file(
      ${DOXYGEN_IN}
      ${DOXYGEN_OUT}
      @ONLY)

    # note the option ALL which allows to build the docs together with the
    # application
    add_custom_target(
      doc_doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM)

    message(STATUS "Setup up the Doxygen documention build")

  else(Doxygen_FOUND)
    message(
      WARNING
        "Doxygen need to be installed to generate the doxygen documentation")
  endif(Doxygen_FOUND)

endif()

#
# install
#

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(LIBCOAP_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
install(
  TARGETS ${COAP_LIBRARY_NAME}
  EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib)

install(
  EXPORT ${PROJECT_NAME}Targets
  DESTINATION ${LIBCOAP_CONFIG_INSTALL_DIR}
  NAMESPACE ${PROJECT_NAME}::
  COMPONENT dev)

configure_package_config_file(
  cmake/Config.cmake.in
  ${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION
  ${LIBCOAP_CONFIG_INSTALL_DIR})
write_basic_package_version_file(
  ${PROJECT_NAME}ConfigVersion.cmake
  COMPATIBILITY SameMajorVersion)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${LIBCOAP_CONFIG_INSTALL_DIR}
  COMPONENT dev)

install(
  DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT dev
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "coap.h" EXCLUDE
  PATTERN "coap_riot.h" EXCLUDE
  PATTERN "lwippools.h" EXCLUDE
  PATTERN "utlist.h" EXCLUDE
  PATTERN "uthash.h" EXCLUDE
  PATTERN "*_internal.h" EXCLUDE)
install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT dev
  FILES_MATCHING
  PATTERN "*.h")
