cmake_minimum_required(VERSION 3.22)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Library core sources
set(lwesp_core_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_cli.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_dhcp.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_dns.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_hostname.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_ping.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_utils.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_ap.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_buff.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_conn.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_debug.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_evt.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_input.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_int.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_mdns.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_mem.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_parser.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_pbuf.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_server.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_sntp.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_sta.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_threads.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_timeout.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_unicode.c
    ${CMAKE_CURRENT_LIST_DIR}/src/lwesp/lwesp_wps.c
    )
    
# API sources
set(lwesp_api_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/api/lwesp_netconn.c
    )

# HTTP server
set(lwesp_httpsrv_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/apps/http_server/lwesp_http_server.c
    ${CMAKE_CURRENT_LIST_DIR}/src/apps/http_server/lwesp_http_server_fs.c
    )

# MQTT
set(lwesp_mqtt_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/apps/mqtt/lwesp_mqtt_client.c
    ${CMAKE_CURRENT_LIST_DIR}/src/apps/mqtt/lwesp_mqtt_client_api.c
    ${CMAKE_CURRENT_LIST_DIR}/src/apps/cayenne/lwesp_cayenne.c
    ${CMAKE_CURRENT_LIST_DIR}/src/apps/cayenne/lwesp_cayenne_evt.c
    )

# CLI
set(lwesp_cli_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/src/cli/cli.c
    ${CMAKE_CURRENT_LIST_DIR}/src/cli/cli_input.c
    )

# All apps source files
set(lwesp_allapps_SRCS
    ${lwesp_mqtt_SRCS}
    ${lwesp_httpsrv_SRCS}
    ${lwesp_cli_SRCS}
    )

# Setup include directories
set(lwesp_include_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    )

# Register core library to the system
add_library(lwesp INTERFACE)
target_sources(lwesp PUBLIC ${lwesp_core_SRCS})
target_include_directories(lwesp INTERFACE ${lwesp_include_DIRS})

# Register API to the system
add_library(lwesp_api INTERFACE)
target_sources(lwesp_api PUBLIC ${lwesp_api_SRCS})
target_include_directories(lwesp_api INTERFACE ${lwesp_include_DIRS})

# Register apps to the system
add_library(lwesp_apps INTERFACE)
target_sources(lwesp_apps PUBLIC ${lwesp_allapps_SRCS})
target_include_directories(lwesp_apps INTERFACE ${lwesp_include_DIRS})

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
