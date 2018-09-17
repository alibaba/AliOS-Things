NAME := littlevgl_simulate

$(NAME)_SOURCES := littlevgl_simulate.c

GLOBAL_DEFINES += LITTLEVGL_SIMULATE

$(NAME)_COMPONENTS := yloop cli
$(NAME)_COMPONENTS += 3rdparty.experimental.gui.littlevGL
$(NAME)_COMPONENTS += example.littlevgl_simulate.lv_examples
$(NAME)_COMPONENTS += example.littlevgl_simulate.lv_drivers

GLOBAL_LDFLAGS += -lSDL2

GLOBAL_INCLUDES += ./