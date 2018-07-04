LIBA_TARGET := libiot_dm.a
HDR_REFS            += src/utils
HDR_REFS            += src/sdk-impl
HDR_REFS            += src/services/linkkit/cm/inc
HDR_REFS            += src/services/linkkit/dm/include

CFLAGS      += -DENABLE_THING_DEBUG