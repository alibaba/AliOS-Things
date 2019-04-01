
LIB_SRC := hal_i2s.c
LIB_SRC += drv_i2s.c

LIB_ASRC :=
LIBRARY_NAME := i2s
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC += -Icomponents/inc
LOCAL_INC += -Icomponents/inc/bsp/$(CHIP_NAME)
LOCAL_INC += -Icomponents/inc/bsp
LOCAL_INC += -Icomponents/drv/gpio
LOCAL_INC += -Icomponents/drv/pinmux
LOCAL_INC += -Icomponents/drv/dmac
LOCAL_INC += -Icomponents/drv/$(CHIP_NAME)


RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
