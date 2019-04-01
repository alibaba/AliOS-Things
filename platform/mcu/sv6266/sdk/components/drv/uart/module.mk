
LIB_SRC := hal_uart.c
LIB_SRC += drv_uart.c

LIB_ASRC :=
LIBRARY_NAME := uart
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC :=

RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
