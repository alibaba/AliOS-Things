NAME := littlevgl_simulate

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := littlevgl_simulate.c

GLOBAL_DEFINES += LITTLEVGL_SIMULATE

$(NAME)_COMPONENTS := yloop cli
$(NAME)_COMPONENTS += 3rdparty.experimental.gui.littlevGL
$(NAME)_COMPONENTS += app.example.littlevgl_simulate.lv_examples
$(NAME)_COMPONENTS += app.example.littlevgl_simulate.lv_drivers

GLOBAL_LDFLAGS += -lSDL2

GLOBAL_INCLUDES += ./