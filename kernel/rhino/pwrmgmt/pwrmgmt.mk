NAME := pwrmgmt

$(NAME)_TYPE 	    := kernel
$(NAME)_MBINS_TYPE  := kernel

$(NAME)_SOURCES     := cpu_pwr_lib.c
$(NAME)_SOURCES     += cpu_pwr_hal_lib.c
$(NAME)_SOURCES     += cpu_pwr_show.c
$(NAME)_SOURCES     += cpu_tickless.c

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
GLOBAL_DEFINES      += AOS_PWRMGMT

