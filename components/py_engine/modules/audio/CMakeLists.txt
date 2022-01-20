

list(APPEND MICROPY_DEF_MODULES_PORT MICROPY_PY_AUDIO=1)

# Collect audio micropython port source
if(ESP_PLATFORM)
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/esp32/modaudio.c)
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/esp32/audio_player.c)
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/esp32/audio_recorder.c)
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/esp32/vfs_stream.c)
else()
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/aos/modaudio.c)
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/aos/audio_player.c)
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/aos/audio_recorder.c)
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/aos/audio_snd.c)
    list(APPEND MICROPY_SOURCE_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/aos/audio_tts.c)
endif()

if(ESP_PLATFORM)
    # Collect ADF Library Sources
    file(GLOB AUDIO_SAL_SRC RELATIVE $ENV{ADF_PATH}/components/audio_sal "*.c")
    file(GLOB AUDIO_PIPLINE_SRC RELATIVE $ENV{ADF_PATH}/components/audio_pipeline "*.c")
    file(GLOB AUDIO_STREAM_SRC RELATIVE $ENV{ADF_PATH}/components/audio_stream "*.c")
    file(GLOB AUDIO_CODEC_SRC RELATIVE $ENV{ADF_PATH}/components/esp-adf-libs/esp_codec "*.c")
    file(GLOB AUDIO_BOARD_SRC RELATIVE $ENV{ADF_PATH}/components/audio_board/m5stack_core2 "*.c")
    file(GLOB AUDIO_HAL_SRC RELATIVE $ENV{ADF_PATH}/components/audio_hal "*.c")
    file(GLOB ESP_DISPATCHER_SRC RELATIVE $ENV{ADF_PATH}/components/esp_dispatcher "*.c")
    file(GLOB AUDIO_HAL_DRIVER_SRC RELATIVE $ENV{ADF_PATH}/components/audio_hal/driver/ns4168 "*.c")
endif()

list(APPEND MICROPY_SOURCE_MODULES_PORT 
    ${AUDIO_SAL_SRC}
    ${AUDIO_PIPLINE_SRC}
    ${AUDIO_STREAM_SRC}
    ${AUDIO_CODEC_SRC}
    ${AUDIO_BOARD_SRC}
    ${AUDIO_HAL_SRC}
    ${ESP_DISPATCHER_SRC}
    ${AUDIO_HAL_DRIVER_SRC}
)

if(ESP_PLATFORM)
    # append audio include
    list(APPEND MICROPY_INC_MODULES_PORT ${MICROPY_MODULES_PORT_DIR}/audio/esp32)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/esp-adf-libs/esp_audio/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/adf_utils/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/audio_hal/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/audio_pipeline/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/audio_sal/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/audio_stream/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/esp-adf-libs/audio_misc/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/esp-adf-libs/esp_codec/include/codec)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/esp-adf-libs/esp_codec/include/processing)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/esp-adf-libs/recorder_engine/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/audio_board/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/audio_board/m5stack_core2)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/esp-sr/lib/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/esp_dispatcher/include)
    list(APPEND MICROPY_INC_MODULES_PORT $ENV{ADF_PATH}/components/audio_hal/driver/include)
else()
    list(APPEND MICROPY_INC_MODULES_PORT ${AOS_COMPONENTS_DIR}/uvoice/internal)
    list(APPEND MICROPY_INC_MODULES_PORT ${AOS_COMPONENTS_DIR}/uvoice/include)
    list(APPEND MICROPY_INC_MODULES_PORT ${AOS_COMPONENTS_DIR}/uvoice/application/alicloudtts/include)
endif()

# add_prebuilt_library(esp_audio "$ENV{ADF_PATH}/components/esp-adf-libs/esp_audio/lib/esp32/libesp_audio.a"
#                  PRIV_REQUIRES esp-adf-libs)
# add_prebuilt_library(esp_codec "$ENV{ADF_PATH}/components/esp-adf-libs/esp_codec/lib/esp32/libesp_codec.a"
#                  PRIV_REQUIRES esp-adf-libs)
# add_prebuilt_library(esp_processing "$ENV{ADF_PATH}/components/esp-adf-libs/esp_codec/lib/esp32/libesp_processing.a"
#                  PRIV_REQUIRES esp-adf-libs)

# target_link_libraries(${MICROPY_TARGET} esp_processing esp_audio esp_codec)

# target_link_libraries(${COMPONENT_LIB} INTERFACE "-L$ENV{ADF_PATH}/components/esp-adf-libs/esp_audio/lib/esp32")
# target_link_libraries(${COMPONENT_LIB} INTERFACE "-L$ENV{ADF_PATH}/components/esp-adf-libs/esp_codec/lib/esp32")
# target_link_libraries(${COMPONENT_LIB} PUBLIC esp_processing)
