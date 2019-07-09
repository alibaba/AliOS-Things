NAME := jsengine_app

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := build for jsengine firmware
$(NAME)_SOURCES := jsengine_app.c

$(NAME)_COMPONENTS := ulog yloop cli linkkit_sdk_c cjson netmgr
$(NAME)_COMPONENTS += spiffs
$(NAME)_COMPONENTS += jsengine

GLOBAL_INCLUDES += ./

