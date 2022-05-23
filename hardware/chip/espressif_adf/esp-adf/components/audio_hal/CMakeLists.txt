set(COMPONENT_ADD_INCLUDEDIRS ./include
                            ./driver/es8388
                            ./driver/es8374
                            ./driver/es8311
                            ./driver/es7243
                            ./driver/es7148
                            ./driver/es7210
                            ./driver/tas5805m
                            ./driver/zl38063
                            ./driver/zl38063/api_lib
                            ./driver/zl38063/example_apps
                            ./driver/zl38063/firmware
                            ./driver/include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES audio_sal audio_board mbedtls esp_peripherals display_service esp_dispatcher)

set(COMPONENT_SRCS ./audio_hal.c
                    ./driver/es8388/es8388.c
                    ./driver/es8388/headphone_detect.c
                    ./driver/es8374/es8374.c
                    ./driver/es8311/es8311.c
                    ./driver/es7243/es7243.c
                    ./driver/es7148/es7148.c
                    ./driver/es7210/es7210.c
                    ./driver/tas5805m/tas5805m.c
                    ./driver/zl38063/zl38063.c
                    ./driver/zl38063/api_lib/vprocTwolf_access.c
                    ./driver/zl38063/api_lib/vproc_common.c
                    ./driver/zl38063/example_apps/tw_hal_verify.c
                    ./driver/zl38063/example_apps/tw_ldcfg.c
                    ./driver/zl38063/example_apps/tw_ldfw.c
                    ./driver/zl38063/example_apps/tw_ldfwcfg.c
                    ./driver/zl38063/example_apps/tw_spi_access.c)

register_component()
IF (NOT (CONFIG_IDF_TARGET STREQUAL "esp32c3"))
target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L${CMAKE_CURRENT_LIST_DIR}/driver/zl38063/firmware")
target_link_libraries(${COMPONENT_TARGET} INTERFACE firmware)
ENDIF()
