set(COMPONENT_ADD_INCLUDEDIRS include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES clouds mbedtls audio_board esp-adf-libs audio_hal audio_sal esp_peripherals)

set(COMPONENT_SRCS ./dueros_service.c ./esp_audio_device_info.c)

set(COMPONENT_EMBED_FILES duer_profile)

register_component()
