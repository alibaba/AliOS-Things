NAME := mbmaster

$(NAME)_MBINS_TYPE  := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY :=

$(NAME)_SOURCES     := functions/mbfunccoils.c
$(NAME)_SOURCES     += functions/mbfuncholding.c
$(NAME)_SOURCES     += port/port_event.c
$(NAME)_SOURCES     += port/port_other.c
$(NAME)_SOURCES     += port/port_timer.c
$(NAME)_SOURCES     += rtu/mbcrc.c
$(NAME)_SOURCES     += rtu/mbrtu.c
$(NAME)_SOURCES     += mb.c

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

GLOBAL_INCLUDES     += include
GLOBAL_DEFINES      += AOS_MBMASTER

