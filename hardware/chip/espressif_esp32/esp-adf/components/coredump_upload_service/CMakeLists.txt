set(COMPONENT_ADD_INCLUDEDIRS include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES esp_http_client espcoredump spi_flash)
set(COMPONENT_PRIV_REQUIRES audio_sal esp_dispatcher)

set(COMPONENT_SRCS ./coredump_upload_service.c)

register_component()
