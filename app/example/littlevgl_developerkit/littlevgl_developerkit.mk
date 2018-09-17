NAME := littlevgl_developerkit

$(NAME)_SOURCES := littlevgl_developerkit.c AliOS_Things_logo.c sensor_display.c

GLOBAL_DEFINES += LITTLEVGL_DEVELOPERKIT

$(NAME)_COMPONENTS := yloop cli

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

$(NAME)_COMPONENTS += 3rdparty.experimental.gui.littlevGL

GLOBAL_INCLUDES += ./