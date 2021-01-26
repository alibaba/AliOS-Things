NAME := game_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := game demo
$(NAME)_SOURCES := maintask.c appdemo.c
$(NAME)_SOURCES += ./sdl2-tetris/tetris.c \
					./sdl2-tetris/input.c \
					./sdl2-tetris/graphics.c \
					./sdl2-tetris/logsys.c

$(NAME)_COMPONENTS += osal_aos
$(NAME)_COMPONENTS += SDL2
$(NAME)_COMPONENTS += freetype253
$(NAME)_COMPONENTS += littlefs
$(NAME)_COMPONENTS += cli
$(NAME)_COMPONENTS += netmgr
$(NAME)_COMPONENTS += lcd

GLOBAL_DEFINES += __ALIOS__
AOS_LCD_ILI9341 ?= y