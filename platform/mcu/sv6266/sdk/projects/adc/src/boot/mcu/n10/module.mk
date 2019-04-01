
LIB_SRC := init_mem.c
LIB_SRC += cache.c
LIB_SRC += boot.c
LIB_SRC += soc_init.c
LIB_SRC += bsp.c
LIB_SRC += nmi.c

LIB_ASRC := start.S  vector_32ivic.S
LIBRARY_NAME := boot
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=
LOCAL_INC += -I$(TOPDIR)/components/drv
LOCAL_INC += -I$(TOPDIR)/components/drv/wdog

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
