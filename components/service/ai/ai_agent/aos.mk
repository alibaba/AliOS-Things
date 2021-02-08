NAME := ai_agent

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := ai agent

$(NAME)_SOURCES := ./src/ai_agent.c \
					./src/conversions/format/to_rgb565.c

$(NAME)_INCLUDES += ./include/
GLOBAL_INCLUDES += ./include/
GLOBAL_DEFINES += USE_LWIPSOCK

$(NAME)_COMPONENTS += http cjson

USE_WIFICAMERA ?= y

ifeq ($(USE_WIFICAMERA),y)
$(NAME)_SOURCES += ./src/frontend/wifi_camera.c
$(NAME)_INCLUDES += ./src/frontend/
endif

ifeq ($(UCLOUD_AI_FACEBODY_ENABLE),y)
$(NAME)_SOURCES += ./src/frontend/wifi_camera.c
$(NAME)_INCLUDES += ./src/frontend/
$(NAME)_COMPONENTS += ucloud_ai
endif

ifeq ($(UCLOUD_AI_OBJECTDET_ENABLE),y)
$(NAME)_SOURCES += ./src/frontend/wifi_camera.c
$(NAME)_INCLUDES += ./src/frontend/
$(NAME)_COMPONENTS += ucloud_ai
endif

ifeq ($(AOS_USE_JPEG), y)
$(NAME)_COMPONENTS += jpeg
$(NAME)_SOURCES += ./src/conversions/jpeg_decoder/jpeg_decoder.c
$(NAME)_INCLUDES += ./src/conversions/jpeg_decoder/
endif

ifeq ($(AOS_LCD_ILI9341), y)
$(NAME)_COMPONENTS += lcd
ifeq ($(AOS_USE_SDL2), y)
$(NAME)_COMPONENTS += SDL2 freetype253
GLOBAL_DEFINES += AOS_COMP_SDL2
endif
endif

ifeq ($(AOS_USE_LCD), y)
$(NAME)_COMPONENTS += lcd
ifeq ($(AOS_USE_SDL2), y)
$(NAME)_COMPONENTS += SDL2 freetype253
GLOBAL_DEFINES += AOS_COMP_SDL2
endif
$(NAME)_SOURCES  += ./src/backend/graphics/graphics.c
$(NAME)_INCLUDES += ./src/backend/graphics/
GLOBAL_INCLUDES  += ./src/backend/graphics/
endif

#for HaaS EDK
ifeq ($(AOS_OLED_SH1106), y)
$(NAME)_COMPONENTS += oled
GLOBAL_DEFINES += AOS_OLED_SH1106
endif

#for beeper output
ifeq ($(AOS_USE_BEEPER), y)
$(NAME)_SOURCES += ./src/backend/beeper/beeper.c
$(NAME)_INCLUDES += ./src/backend/beeper/
GLOBAL_INCLUDES += ./src/backend/beeper/
GLOBAL_DEFINES += AOS_USE_BEEPER
endif