NAME := cjson

$(NAME)_TYPE := share
GLOBAL_INCLUDES += include
# don't modify to L_CFLAGS, because CONFIG_CJSON_WITHOUT_DOUBLE should enable global
#$(NAME)_CFLAGS  += -DCONFIG_CJSON_WITHOUT_DOUBLE

$(NAME)_SOURCES := cJSON.c cJSON_Utils.c
