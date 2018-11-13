NAME := wifihalapp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := wifihalapp.c

$(NAME)_COMPONENTS += yloop cli halwifi

GLOBAL_INCLUDES += ./
