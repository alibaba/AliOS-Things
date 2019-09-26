NAME := mbmaster

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := modbus master

$(NAME)_SOURCES := api/api_readholdingregisters.c
$(NAME)_SOURCES += pdu/readholdingregisters.c
$(NAME)_SOURCES += adu/rtu/rtu.c
$(NAME)_SOURCES += adu/rtu/mbcrc.c
$(NAME)_SOURCES += physical/serial.c
$(NAME)_SOURCES += auxiliary/log.c
$(NAME)_SOURCES += auxiliary/other.c
$(NAME)_SOURCES += api/mbm.c

ifeq ($(HOST_ARCH),linux)
$(NAME)_DEFINES += IO_NEED_TRAP
endif

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS += -Wall -Werror
else ifeq ($(COMPILER),armcc)
GLOBAL_DEFINES += __BSD_VISIBLE
endif

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_COMP_MBMASTER
AOS_COMP_MBMASTER   := y
