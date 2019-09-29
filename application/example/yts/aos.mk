NAME := yts

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := AliOS Things test suite
$(NAME)_SOURCES := main.c
$(NAME)_COMPONENTS := testcase ulog vfs yloop

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable

ifneq (,$(findstring linux, $(BUILD_STRING)))
$(NAME)_COMPONENTS += lwip umesh dda netmgr fatfs

GLOBAL_LDFLAGS += -lreadline -lncurses
GLOBAL_DEFINES += CONFIG_AOS_MESHYTS DEBUG YTS_LINUX
else
$(NAME)_COMPONENTS += cli
endif

$(NAME)_INCLUDES += ./
