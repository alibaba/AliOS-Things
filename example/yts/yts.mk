NAME := yts

$(NAME)_SOURCES := main.c
$(NAME)_COMPONENTS := testcase rhino.test log vfs yloop hal

$(NAME)_CFLAGS += -Wall -Werror

ifneq (,$(findstring linux, $(BUILD_STRING)))
$(NAME)_COMPONENTS += protocols.net netmgr modules.fs.fatfs framework.common

GLOBAL_DEFINES += YTS_LINUX
endif

