LIB_SRC 	:= rom_init.c
LIB_SRC 	+= lwip_rom_init.c
LIB_SRC 	+= freertos_rom_init.c

LIB_ASRC :=
LIBRARY_NAME := rom_connector
LOCAL_CFLAGS +=
LOCAL_AFLAGS +=

LOCAL_INC += -Icomponents/inc/bsp/$(CHIP_NAME)
LOCAL_INC += -Icomponents/inc/bsp
LOCAL_INC += -Icomponents/drv

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))

