NAME := littlevgl_simulate

$(NAME)_SOURCES := littlevgl_simulate.c

GLOBAL_DEFINES += LITTLEVGL_SIMULATE

$(NAME)_COMPONENTS := yloop cli
$(NAME)_COMPONENTS += framework.GUI.littlevGL
$(NAME)_COMPONENTS += example.littlevgl_simulate.lv_examples
$(NAME)_COMPONENTS += example.littlevgl_simulate.lv_drivers

GLOBAL_INCLUDES     += ../framework/GUI/littlevGL/
GLOBAL_LDFLAGS += -lSDL2
