NAME := yts

$(NAME)_SOURCES := main.c
$(NAME)_COMPONENTS := testcase rhino.test log vfs yloop hal

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable

ifneq (,$(findstring linux, $(BUILD_STRING)))
$(NAME)_COMPONENTS += protocols.net netmgr modules.fs.fatfs framework.common

GLOBAL_LDFLAGS += -lreadline -lncurses
GLOBAL_DEFINES += CONFIG_AOS_MESHYTS DEBUG YTS_LINUX
else
$(NAME)_COMPONENTS += cli
endif

