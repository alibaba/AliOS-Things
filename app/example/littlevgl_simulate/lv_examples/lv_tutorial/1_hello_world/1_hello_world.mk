NAME := 1_hello_world

$(NAME)_SOURCES += lv_tutorial_hello_world.c

$(NAME)_INCLUDES    += ../../
$(NAME)_INCLUDES    += ../

GLOBAL_INCLUDES     += ../../..