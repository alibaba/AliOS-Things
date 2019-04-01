
LIB_SRC :=
ifeq ($(strip $(SUPPORT_PARTITION_MP_TABLE)), 1)
LIB_SRC += sa_cfg.c
endif
ifeq ($(strip $(SUPPORT_PARTITION_CFG_TABLE)), 1)
LIB_SRC += mac_cfg.c
endif
ifeq ($(strip $(SUPPORT_PARTITION_USER_RAW)), 1)
LIB_SRC += user_cfg.c
endif

LIB_ASRC :=
LIBRARY_NAME := cfg
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC := -Icomponents/bsp
LOCAL_INC += -Icomponents/third_party/cJSON

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
