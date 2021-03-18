NAME := trace

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := systemview trace

ifeq ($(AOS_COMP_TRACE),y)
$(NAME)_SOURCES := k_trace.c
$(NAME)_SOURCES += SEGGER/SEGGER_RTT.c
$(NAME)_SOURCES += SEGGER/SEGGER_SYSVIEW.c
$(NAME)_SOURCES += SEGGER/SEGGER_RTT_printf.c
$(NAME)_SOURCES += SEGGER/SEGGER_RTT_ASM_ARMv7M.S

$(NAME)_SOURCES += Config/SEGGER_SYSVIEW_Config_AliOSThings.c
$(NAME)_SOURCES += Config/SEGGER_SYSVIEW_AliOSThings.c

$(NAME)_SOURCES += uart_send.c

$(NAME)_INCLUDES += SEGGER/
$(NAME)_INCLUDES += Config/
endif

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
else ifeq ($(COMPILER),rvct)
GLOBAL_DEFINES += __BSD_VISIBLE
endif

GLOBAL_INCLUDES += . include
