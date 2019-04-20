NAME := pwrmgmt

$(NAME)_MBINS_TYPE  := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES     := cpu_pwr_lib.c
$(NAME)_SOURCES     += cpu_pwr_hal_lib.c
$(NAME)_SOURCES     += cpu_pwr_show.c
$(NAME)_SOURCES     += cpu_tickless.c
$(NAME)_SOURCES     += wifi_pwr_lib.c

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
GLOBAL_DEFINES     += AOS_COMP_PWRMGMT
GLOBAL_DEFINES     += RHINO_CONFIG_CPU_PWR_MGMT=1
AOS_COMP_PWRMGMT   := y
