NAME := libiot_sal

#GLOBAL_DEFINES += WITH_SAL
$(NAME)_SOURCES := sal_sockets.c sal_err.c sal_arch.c ip4_addr.c sal.c
GLOBAL_INCLUDES += ./include
