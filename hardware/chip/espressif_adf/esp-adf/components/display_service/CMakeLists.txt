set(COMPONENT_ADD_INCLUDEDIRS include led_indicator/include led_bar/include)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES audio_sal audio_board audio_hal esp_peripherals)

set(COMPONENT_SRCS ./display_service.c led_indicator/led_indicator.c led_bar/led_bar_is31x.c led_bar/led_bar_aw2013.c led_bar/led_bar_ws2812.c)

register_component()
