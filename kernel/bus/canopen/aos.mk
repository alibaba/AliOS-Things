NAME := canopen

ifeq ($(CANFESTIVAL_ENABLED),y)
$(NAME)_MBINS_TYPE  := kernel
$(NAME)_VERSION     := 1.0.0
$(NAME)_SUMMARY     := canfestival canopen stack

$(NAME)_SOURCES     := src/dcf.c
$(NAME)_SOURCES     += src/emcy.c
$(NAME)_SOURCES     += src/lifegrd.c
$(NAME)_SOURCES     += src/lss.c
$(NAME)_SOURCES     += src/nmtMaster.c
$(NAME)_SOURCES     += src/nmtSlave.c
$(NAME)_SOURCES     += src/objacces.c
$(NAME)_SOURCES     += src/pdo.c
$(NAME)_SOURCES     += src/sdo.c
$(NAME)_SOURCES     += src/states.c
$(NAME)_SOURCES     += src/sync.c
$(NAME)_SOURCES     += src/timer.c

$(NAME)_SOURCES     += adapter/co_adapter_can.c
$(NAME)_SOURCES     += adapter/co_adapter_timer.c
$(NAME)_SOURCES     += wrap/canopen_wrap.c

ifeq ($(HOST_ARCH),linux)
$(NAME)_DEFINES     += IO_NEED_TRAP
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif

GLOBAL_INCLUDES     += wrap adapter include
GLOBAL_DEFINES      += AOS_CANOPEN
endif
