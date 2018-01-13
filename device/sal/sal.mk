NAME := sal

$(NAME)_TYPE := kernel
ifneq (1,$(at_adapter))
GLOBAL_DEFINES += WITH_SAL # for sal general use
$(NAME)_SOURCES := sal_sockets.c sal_err.c sal_arch.c ip4_addr.c sal.c sal_device.c
GLOBAL_INCLUDES += ./include
endif
