set(COMPONENT_ADD_INCLUDEDIRS include)
set(COMPONENT_PRIV_INCLUDEDIRS inc)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES bt mbedtls nvs_flash)
set(COMPONENT_PRIV_REQUIRES audio_sal esp_dispatcher esp_actions)

list(APPEND COMPONENT_SRCS src/esp_wifi_setting.c
                    src/wifi_service.c
                    src/wifi_ssid_manager.c
                    smart_config/smart_config.c
                    blufi_config/blufi_config.c
                    blufi_config/blufi_security.c)

IF (CONFIG_IDF_TARGET STREQUAL "esp32c3")
    register_component()
ELSE()
    list(APPEND COMPONENT_SRCS airkiss_config/airkiss_config.c)
    register_component()
    target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L${CMAKE_CURRENT_LIST_DIR}/airkiss_config")
    target_link_libraries(${COMPONENT_TARGET} INTERFACE airkiss_aes)
ENDIF()
