NAME := pwrmgmt

$(NAME)_MBINS_TYPE  := kernel
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY :=

$(NAME)_SOURCES     := cpu_pwr_lib.c
$(NAME)_SOURCES     += cpu_pwr_hal_lib.c
$(NAME)_SOURCES     += cpu_tickless.c
$(NAME)_SOURCES     += pwrmgmt_api.c
$(NAME)_SOURCES     += pwrmgmt_debug.c

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

GLOBAL_INCLUDES    += include
#This MACRO is defined when pwrmgmt module is selected.
GLOBAL_DEFINES     += AOS_COMP_PWRMGMT
#This MACRO is used to enable MCU low powr. MCU low power is enabled by defualt,
#when pwrmgmt module is selected, so it is set to 1 by default.
GLOBAL_DEFINES     += PWRMGMT_CONFIG_CPU_LOWPOWER=1
#This MACRO has the same meaning as PWRMGMT_CONFIG_CPU_LOWPOWER, so it should be set
#the same value. This MACRO is used by Rhino only, other module should not reference 
#it.
GLOBAL_DEFINES     += RHINO_CONFIG_PWRMGMT=1
AOS_COMP_PWRMGMT   := y
