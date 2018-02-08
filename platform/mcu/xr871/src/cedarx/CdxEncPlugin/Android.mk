LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

include $(LOCAL_PATH)/../../../config.mk

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
		$(notdir $(wildcard $(LOCAL_PATH)/*.c)) \
		$(notdir $(wildcard $(LOCAL_PATH)/*.cpp))

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/../../../VE/include \
	$(LOCAL_PATH)/../../../MEMORY/include \
	$(LOCAL_PATH)/../../../ \


LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libVE \
	libMemAdapter

LOCAL_LDLIBS := -llog

LOCAL_LDFLAGS += $(LOCAL_PATH)/lib_a4.4/static/libaacenc.a \
				 $(LOCAL_PATH)/lib_a4.4/static/libadpcmenc.a \
				 $(LOCAL_PATH)/lib_a4.4/static/libmp3enc.a \
#
#				 $(LOCAL_PATH)/lib_a4.4/static/libpcmenc.a


LOCAL_MODULE:= libaencoder

include $(BUILD_SHARED_LIBRARY)
