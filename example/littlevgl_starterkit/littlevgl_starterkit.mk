NAME := littlevgl_starterkit

$(NAME)_SOURCES := littlevgl_starterkit.c

GLOBAL_DEFINES += LITTLEVGL_STARTERKIT

$(NAME)_COMPONENTS := yloop cli

ifeq ($(BENCHMARKS),1)
$(NAME)_COMPONENTS  += benchmarks
GLOBAL_DEFINES      += CONFIG_CMD_BENCHMARKS
endif

$(NAME)_COMPONENTS += framework.GUI.littlevGL
GLOBAL_INCLUDES     += ../framework/GUI/littlevGL/
