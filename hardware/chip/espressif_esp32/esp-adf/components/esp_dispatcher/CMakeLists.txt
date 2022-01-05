set(COMPONENT_ADD_INCLUDEDIRS include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES audio_sal)

set(COMPONENT_SRCS ./audio_service.c ./esp_dispatcher.c ./periph_service.c ./esp_delegate.c)

register_component()
