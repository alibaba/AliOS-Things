NAME := yts_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := AliOS Things test suite
$(NAME)_SOURCES := main.c maintask.c
$(NAME)_COMPONENTS := testcase ulog vfs yloop yunit

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable

ifneq (,$(findstring linux, $(BUILD_STRING)))
$(NAME)_COMPONENTS-$(AOS_BOARD_LINUXHOST) += lwip netmgr fatfs
$(NAME)_COMPONENTS-$(AOS_BOARD_LINUXHOST_DEMO) += lwip netmgr fatfs
$(NAME)_COMPONENTS-$(AOS_BOARD_ARMHFLINUX) += lwip netmgr fatfs

GLOBAL_LDFLAGS += -lreadline -lncurses -lrt
GLOBAL_DEFINES += CONFIG_AOS_MESHYTS DEBUG YTS_LINUX
else
$(NAME)_COMPONENTS += cli
endif

$(NAME)_INCLUDES += ./
