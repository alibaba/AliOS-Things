NAME := debug

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := coredump and fatal err debug

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
endif


$(NAME)_SOURCES := debug_cpuusage.c  \
                   debug_overview.c  \
                   debug_panic.c     \
                   debug_backtrace.c \
                   debug_infoget.c

GLOBAL_DEFINES += AOS_COMP_DEBUG
GLOBAL_ASMFLAGS += -DAOS_COMP_DEBUG

GLOBAL_INCLUDES  += ./  ./include


