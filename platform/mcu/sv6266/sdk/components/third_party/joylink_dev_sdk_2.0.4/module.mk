LIB_SRC := 
LIB_SRC += auth/joylink_aes.c
LIB_SRC += auth/joylink_auth_md5.c
LIB_SRC += auth/joylink_crypt.c
LIB_SRC += auth/joylink_auth_uECC.c
LIB_SRC += auth/joylink3_auth_uECC.c
LIB_SRC += auth/joylink_auth_crc.c

LIB_SRC += json/joylink_json_sub_dev.c
LIB_SRC += json/joylink_json.c
LIB_SRC += json/cJSON.c

LIB_SRC += list/joylink_list.c

LIB_SRC += example/joylink_extern.c
LIB_SRC += example/joylink_extern_sub_dev.c
LIB_SRC += example/joylink_extern_json.c

LIB_SRC += joylink/joylink_packets.c
LIB_SRC += joylink/joylink_utils.c
LIB_SRC += joylink/joylink_dev_lan.c
LIB_SRC += joylink/joylink_join_packet.c
LIB_SRC += joylink/joylink_dev_server.c
LIB_SRC += joylink/joylink_sub_dev.c
LIB_SRC += joylink/joylink_dev_timer.c
LIB_SRC += joylink/joylink_security.c
LIB_SRC += joylink/joylink_dev_sdk.c



LIB_ASRC :=
LIBRARY_NAME := joylink_dev_sdk
#LOCAL_CFLAGS += -Wno-address -D__MTK_7687__ -D_GET_HOST_BY_NAME_
LOCAL_CFLAGS += -Wno-address -D__ICOMM_6166__ -D_GET_HOST_BY_NAME_
LOCAL_AFLAGS +=

LOCAL_INC += -I$(TOPDIR)/components/fsal
LOCAL_INC += -I$(TOPDIR)/components/third_party/joylink_dev_sdk_2.0.4/auth
LOCAL_INC += -I$(TOPDIR)/components/third_party/joylink_dev_sdk_2.0.4/json
LOCAL_INC += -I$(TOPDIR)/components/third_party/joylink_dev_sdk_2.0.4/list
LOCAL_INC += -I$(TOPDIR)/components/third_party/joylink_dev_sdk_2.0.4/example
LOCAL_INC += -I$(TOPDIR)/components/third_party/joylink_dev_sdk_2.0.4/joylink
LOCAL_INC += -I$(PROJ_DIR)/src/cfg

RELEASE_SRC := 2

$(eval $(call build-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))