NAME := yts

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=
$(NAME)_SOURCES := main.c
$(NAME)_COMPONENTS := testcase rhino.test log kernel.fs.vfs yloop

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable

ifneq (,$(findstring linux, $(BUILD_STRING)))
$(NAME)_COMPONENTS += network.lwip network.umesh dda netmgr 3rdparty.experimental.fs.fatfs middleware.common

GLOBAL_LDFLAGS += -lreadline -lncurses
GLOBAL_DEFINES += CONFIG_AOS_MESHYTS DEBUG YTS_LINUX
else
$(NAME)_COMPONENTS += cli
endif

GLOBAL_INCLUDES += ./
