# component name
NAME := canopen
# component information
$(NAME)_MBINS_TYPE  := kernel
$(NAME)_VERSION     := 1.0.2
$(NAME)_SUMMARY     := canfestival canopen stack
# source files and the folder of internal include files
GLOBAL_INCLUDES     += wrap adapter include
# armcc & iar without -Wall and -Werror
#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES      += __BSD_VISIBLE
endif
ifeq ($(HOST_ARCH),linux)
$(NAME)_DEFINES     += IO_NEED_TRAP
endif
# the folder of API files
GLOBAL_INCLUDES += ../../../include/bus/canopen

ifeq ($(CONFIG_CANFESTIVAL_ENABLED),y)
$(NAME)_SOURCES     := src/dcf.c \
                       src/emcy.c \
                       src/lifegrd.c \
                       src/lss.c \
                       src/nmtMaster.c \
                       src/nmtSlave.c \
                       src/objacces.c \
                       src/pdo.c \
                       src/sdo.c \
                       src/states.c \
                       src/sync.c \
                       src/timer.c \
                       adapter/co_adapter_can.c \
                       adapter/co_adapter_timer.c \
                       wrap/canopen_wrap.c
endif
