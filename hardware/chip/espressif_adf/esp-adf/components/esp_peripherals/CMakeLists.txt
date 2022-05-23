list(APPEND COMPONENT_ADD_INCLUDEDIRS ./include
                ./lib/adc_button
                ./lib/gpio_isr
                ./lib/button
                ./lib/blufi
                ./lib/IS31FL3216
                ./lib/aw2013
                ./lib/tca9554
                ./driver/i2c_bus)

list(APPEND COMPONENT_SRCS ./esp_peripherals.c
                ./periph_adc_button.c
                ./periph_button.c
                ./periph_console.c
                ./periph_gpio_isr.c
                ./periph_is31fl3216.c
                ./periph_led.c
                ./periph_spiffs.c
                ./periph_wifi.c
                ./periph_aw2013.c
                ./periph_ws2812.c
                ./lib/button/button.c
                ./lib/blufi/blufi_security.c
                ./lib/blufi/wifibleconfig.c
                ./lib/adc_button/adc_button.c
                ./lib/IS31FL3216/IS31FL3216.c
                ./lib/tca9554/tca9554.c
                ./driver/i2c_bus/i2c_bus.c
                ./lib/gpio_isr/gpio_isr.c)

IF (CONFIG_IDF_TARGET STREQUAL "esp32")
list(APPEND COMPONENT_ADD_INCLUDEDIRS ./lib/sdcard ./lib/touch)
list(APPEND COMPONENT_SRCS ./lib/sdcard/sdcard.c ./periph_sdcard.c ./periph_touch.c ./lib/touch/touch.c)

ELSEIF (CONFIG_IDF_TARGET STREQUAL "esp32s3")
list(APPEND COMPONENT_ADD_INCLUDEDIRS ./lib/sdcard ./lib/touch)
list(APPEND COMPONENT_SRCS ./lib/sdcard/sdcard.c ./periph_sdcard.c ./periph_touch.c ./lib/touch/touch.c)

ELSEIF((ONFIG_IDF_TARGET STREQUAL "esp32s2"))
list(APPEND COMPONENT_ADD_INCLUDEDIRS ./lib/touch)
list(APPEND COMPONENT_SRCS ./periph_touch.c ./lib/touch/touch.c)
ENDIF ()

set(COMPONENT_REQUIRES driver  audio_hal esp_adc_cal audio_sal fatfs console audio_pipeline audio_board spiffs display_service esp_dispatcher bt mbedtls wpa_supplicant)

register_component()
