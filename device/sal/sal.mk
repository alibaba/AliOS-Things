NAME := sal
GLOBAL_DEFINES += WITH_SAL # for sal general use
$(NAME)_TYPE := kernel
$(NAME)_SOURCES := sal_sockets.c sal_err.c sal_arch.c ip4_addr.c sal.c sal_device.c
GLOBAL_INCLUDES += ./include
