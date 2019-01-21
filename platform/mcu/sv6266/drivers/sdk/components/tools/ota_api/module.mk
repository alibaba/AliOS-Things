
LIB_SRC := ota_api.c md5.c

LIB_ASRC :=
LIBRARY_NAME := ota_api
LOCAL_CFLAGS :=
LOCAL_AFLAGS :=

LOCAL_INC += -Icomponents/tools/ota_api
LOCAL_INC += -Icomponents/drv
LOCAL_INC += -Icomponents/osal
LOCAL_INC += -Icomponents/third_party/httpclient/inc
LOCAL_INC += -Icomponents/third_party/tftp
LOCAL_INC += -Icomponents/third_party/spiffs

RELEASE_SRC := 1

$(eval $(call link-lib,$(LIBRARY_NAME),$(LIB_SRC),$(LIB_ASRC),$(LOCAL_CFLAGS),$(LOCAL_INC),$(LOCAL_AFLAGS),$(MYDIR)))
