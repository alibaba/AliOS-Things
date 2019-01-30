NAME := canopen

ifeq ($(CANOPEN),canfestival)
$(NAME)_TYPE        := network
$(NAME)_MBINS_TYPE  := network

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

$(NAME)_SOURCES     += adapter/adapter_can.c
$(NAME)_SOURCES     += adapter/adapter_timer.c
$(NAME)_SOURCES     += api/canopen.c

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

GLOBAL_INCLUDES     += include . api
GLOBAL_DEFINES      += AOS_CANOPEN
endif
