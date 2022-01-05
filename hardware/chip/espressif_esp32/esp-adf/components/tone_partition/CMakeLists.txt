set(COMPONENT_ADD_INCLUDEDIRS include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES bootloader_support)
set(COMPONENT_PRIV_REQUIRES esp_actions esp_dispatcher audio_sal)

set(COMPONENT_SRCS ./tone_partition.c)

register_component()