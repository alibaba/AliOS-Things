NAME := cjson

$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := An ultra-lightweight, portable, single-file, simple-as-can-be ANSI-C compliant JSON parser, under MIT license.

GLOBAL_INCLUDES += include
# don't modify to L_CFLAGS, because CONFIG_CJSON_WITHOUT_DOUBLE should enable global
#$(NAME)_CFLAGS  += -DCONFIG_CJSON_WITHOUT_DOUBLE

$(NAME)_SOURCES := cJSON.c cJSON_Utils.c
