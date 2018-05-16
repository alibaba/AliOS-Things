NAME := framework_runtime

$(NAME)_TYPE := framework

#GLOBAL_INCLUDES += include
# don't modify to L_CFLAGS, because CONFIG_CJSON_WITHOUT_DOUBLE should enable global
GLOBAL_LDFLAGS += -uframework_info

$(NAME)_SOURCES := framework_runtime.c
