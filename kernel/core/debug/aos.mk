NAME := debug

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.0
$(NAME)_SUMMARY    := coredumo and debug

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall  -Werror
endif


$(NAME)_SOURCES := debug_cpuusage.c  \
                   debug_overview.c  \
                   debug_panic.c     \
                   debug_backtrace.c \
                   debug_infoget.c   \
                   debug_lastword.c  \
                   debug_print.c     \
                   debug_test.c      \
                   debug_trace.c     \
                   debug_cli_cmd.c   \
                   debug_dumpsys.c   \
                   debug_cpuusage.c

GLOBAL_INCLUDES += ./ ./include

ifeq ($(AOS_BOARD_HAAS100_MK), y)
GLOBAL_CFLAGS += -DDEBUG_CONFIG_PUSH_FILE_ENABLE

$(NAME)_SOURCES += debug_ymodem.c    \
                   ymodem_adapt.c    \
                   ymodem_ringbuffer.c
endif
