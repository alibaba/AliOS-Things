NAME := littlevgl_starterkit

$(NAME)_SOURCES := littlevgl_starterkit.c AliOS_Things_logo.c

GLOBAL_DEFINES += LITTLEVGL_STARTERKIT

$(NAME)_COMPONENTS := yloop cli

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

$(NAME)_COMPONENTS += 3rdparty.experimental.gui.littlevGL

GLOBAL_INCLUDES += ./