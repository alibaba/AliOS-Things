LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

common_SRC_FILES := \
    png.c pngerror.c \
	pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c \
	pngset.c pngtrans.c pngwio.c pngwrite.c pngwtran.c pngwutil.c

ifeq ($(TARGET_ARCH_ABI),armeabi)
common_SRC_FILES += \
	arm/arm_init.c arm/filter_neon.S arm/filter_neon_intrinsics.c arm/palette_neon_intrinsics.c
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
common_SRC_FILES += \
	arm/arm_init.c arm/filter_neon.S arm/filter_neon_intrinsics.c arm/palette_neon_intrinsics.c
endif
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
common_SRC_FILES += \
	arm/arm_init.c arm/filter_neon.S arm/filter_neon_intrinsics.c arm/palette_neon_intrinsics.c
endif

common_CFLAGS := ## -fomit-frame-pointer

common_C_INCLUDES += \

# For the host
# =====================================================

LOCAL_SRC_FILES := $(common_SRC_FILES)
LOCAL_CFLAGS += $(common_CFLAGS)
LOCAL_C_INCLUDES += $(common_C_INCLUDES)

LOCAL_MODULE:= png

LOCAL_COPY_HEADERS_TO := $(common_COPY_HEADERS_TO)
LOCAL_COPY_HEADERS := $(common_COPY_HEADERS)

include $(BUILD_STATIC_LIBRARY)
#include $(BUILD_SHARED_LIBRARY)

