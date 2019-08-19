NAME := sk_gui

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := Starter Kit GUI demo
$(NAME)_SOURCES := littlevgl_starterkit.c AliOS_Things_logo.c

GLOBAL_DEFINES += LITTLEVGL_STARTERKIT

$(NAME)_COMPONENTS := yloop cli

$(NAME)_COMPONENTS += littlevGL
