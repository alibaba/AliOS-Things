
LIB_SRC := drv_phy.c
LIB_SRC += turismo_common.c

LIB_ASRC :=
LIBRARY_NAME := phy
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC := -Icomponents/bsp
LOCAL_INC += -Icomponents/inc
LOCAL_INC += -Icomponents/drv
LOCAL_INC += -Icomponents/inc/bsp/$(CHIP_NAME)

RELEASE_SRC := 0
$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
