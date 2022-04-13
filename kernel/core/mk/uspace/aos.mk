NAME := uspace

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := user space basic function

$(NAME)_COMPONENTS += usyscall

ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall
endif

$(NAME)_SOURCES += src/utask.c
$(NAME)_SOURCES += src/uassert.c
$(NAME)_SOURCES += src/debug/debug_print.c src/debug/debug_api.c src/debug/debug_test.c
$(NAME)_SOURCES += src/umm/u_mm.c src/umm/u_mm_debug.c src/umm/u_mm_blk.c

$(NAME)_SOURCES += src/utimer.c

ifeq ($(RHINO_CONFIG_URES_TASK),y)
$(NAME)_SOURCES += src/res_task.c
endif

ifeq ($(RHINO_CONFIG_UCLI_TASK),y)
$(NAME)_SOURCES += src/cli_task.c
endif

ifeq ($(RHINO_CONFIG_UCALLBACK_TASK),y)
$(NAME)_SOURCES += src/callback_task.c
endif

$(NAME)_INCLUDES += src/umm
$(NAME)_INCLUDES += ../syscall/common/include

GLOBAL_INCLUDES += ./include
GLOBAL_INCLUDES += ../kspace/include
GLOBAL_INCLUDES += ../syscall/include

GLOBAL_DEFINES += AOS_COMPILE_APP

ifeq ($(AOS_BOARD_HAAS100_MK), y)
$(NAME)_PREBUILT_LIBRARY += lib/armv8m/ubt.a
else
$(NAME)_PREBUILT_LIBRARY += lib/armv7a/ubt.a
endif
