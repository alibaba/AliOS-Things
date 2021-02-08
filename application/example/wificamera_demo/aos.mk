NAME := wificamera_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := connect to esp32-eye to capture video

$(NAME)_SOURCES := app_entry.c     \
                   wificamera_demo.c \
				   maintask.c

$(NAME)_COMPONENTS += netmgr littlefs cli http fatfs cjson ai_agent

GLOBAL_DEFINES += CONFIG_AOS_CLI CONFIG_AOS_FATFS_SUPPORT_MMC
$(NAME)_INCLUDES += graphics/

GLOBAL_DEFINES += USE_LWIPSOCK

#board config
USE_BOARD_HAAS100 ?= n
USE_BOARD_HAASEDK ?= y

ifeq ($(USE_BOARD_HAAS100),y)
#for haas 100
AOS_LCD_ILI9341 ?= y
AOS_LCD_ST7789v ?= n
AOS_USE_SDL2 ?= y
endif

ifeq ($(USE_BOARD_HAASEDK),y)
#for haas edk
AOS_LCD_ILI9341 ?= n
AOS_LCD_ST7789v ?= y
AOS_USE_SDL2 ?= n
AOS_USE_JPEG ?= y
endif

ifeq ($(AOS_LCD_ILI9341),y)
AOS_USE_LCD ?= y
endif
ifeq ($(AOS_LCD_ST7789v),y)
AOS_USE_LCD ?= y
endif