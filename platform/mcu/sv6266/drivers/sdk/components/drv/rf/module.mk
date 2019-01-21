
LIB_SRC := rf_api.c

LIB_ASRC :=
LIBRARY_NAME := rf_api
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC += -Icomponents/inc
LOCAL_INC += -Icomponents/tools/atcmd
LOCAL_INC += -I$(PROJ_DIR)/src/cfg


RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
