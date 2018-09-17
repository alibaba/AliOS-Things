NAME := wifihalapp

$(NAME)_SOURCES := wifihalapp.c

$(NAME)_COMPONENTS += yloop cli

GLOBAL_INCLUDES += ./