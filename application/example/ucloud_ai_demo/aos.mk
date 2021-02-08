NAME := ucloud_ai_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := connect to esp32-eye to capture video

$(NAME)_COMPONENTS += netmgr littlefs cli fatfs ai_agent oss
$(NAME)_COMPONENTS += libiot_devmodel libiot_dynreg

GLOBAL_DEFINES += CONFIG_AOS_CLI CONFIG_AOS_FATFS_SUPPORT_MMC

$(NAME)_SOURCES := app_entry.c     \
				   maintask.c \
				   linkkit_event.c \
				   ucloud_ai_demo.c

GLOBAL_INCLUDES += ./

UCLOUD_AI_FACEBODY_ENABLE ?= y
UCLOUD_AI_OBJECTDET_ENABLE ?= y
UCLOUD_AI_OCR_ENABLE ?= y

GLOBAL_DEFINES += USE_LWIPSOCK

#board config
USE_BOARD_HAAS100 ?= y
USE_BOARD_HAASEDK ?= n

ifeq ($(USE_BOARD_HAAS100),y)
#for haas 100
AOS_LCD_ILI9341 ?= y
AOS_LCD_ST7789v ?= n
AOS_OLED_SH1106 ?= n
AOS_USE_SDL2 ?= y
AOS_USE_BEEPER ?= n
endif

ifeq ($(USE_BOARD_HAASEDK),y)
#for haas edk
AOS_LCD_ILI9341 ?= n
AOS_LCD_ST7789v ?= y
AOS_OLED_SH1106 ?= y
AOS_USE_SDL2 ?= n
AOS_USE_BEEPER ?= y
AOS_USE_JPEG ?= y
endif

ifeq ($(AOS_LCD_ILI9341),y)
AOS_USE_LCD ?= y
endif
ifeq ($(AOS_LCD_ST7789v),y)
AOS_USE_LCD ?= y
endif