NAME := developerkitgui

$(NAME)_SOURCES := developerkitgui.c AliOS_Things_logo.c sensor_display.c house.c weather.c

$(NAME)_COMPONENTS := yloop cli

ENABLE_IRDA_HAL := 1

ENABLE_CAMERA_HAL := 1

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

$(NAME)_INCLUDES    += .

$(NAME)_COMPONENTS += 3rdparty.experimental.gui.littlevGL

GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT

GLOBAL_INCLUDES += ./
