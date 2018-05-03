
LIB_SRC := ota_c.c

LIB_ASRC := ota.S
LIBRARY_NAME := ota_boot
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
