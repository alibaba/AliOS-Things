NAME := app_runtime

$(NAME)_MBINS_TYPE := app

#GLOBAL_INCLUDES += include
# don't modify to L_CFLAGS, because CONFIG_CJSON_WITHOUT_DOUBLE should enable global
GLOBAL_LDFLAGS += -uapp_info

$(NAME)_SOURCES := app_runtime.c
