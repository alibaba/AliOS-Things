SDL_IMAGE_LOCAL_PATH := $(call my-dir)


# Enable this if you want to support loading JPEG images
# The library path should be a relative path to this directory.
SUPPORT_JPG ?= true
JPG_LIBRARY_PATH := external/jpeg-9b

# Enable this if you want to support loading PNG images
# The library path should be a relative path to this directory.
SUPPORT_PNG ?= true
PNG_LIBRARY_PATH := external/libpng-1.6.37

# Enable this if you want to support loading WebP images
# The library path should be a relative path to this directory.
SUPPORT_WEBP ?= true
WEBP_LIBRARY_PATH := external/libwebp-1.0.2


# Build the library
ifeq ($(SUPPORT_JPG),true)
    include $(SDL_IMAGE_LOCAL_PATH)/$(JPG_LIBRARY_PATH)/Android.mk
endif

# Build the library
ifeq ($(SUPPORT_PNG),true)
    include $(SDL_IMAGE_LOCAL_PATH)/$(PNG_LIBRARY_PATH)/Android.mk
endif

# Build the library
ifeq ($(SUPPORT_WEBP),true)
    include $(SDL_IMAGE_LOCAL_PATH)/$(WEBP_LIBRARY_PATH)/Android.mk
endif


# Restore local path
LOCAL_PATH := $(SDL_IMAGE_LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_MODULE := SDL2_image

LOCAL_SRC_FILES :=  \
    IMG.c           \
    IMG_bmp.c       \
    IMG_gif.c       \
    IMG_jpg.c       \
    IMG_lbm.c       \
    IMG_pcx.c       \
    IMG_png.c       \
    IMG_pnm.c       \
    IMG_svg.c       \
    IMG_tga.c       \
    IMG_tif.c       \
    IMG_webp.c      \
    IMG_WIC.c       \
    IMG_xcf.c       \
    IMG_xpm.c.arm   \
    IMG_xv.c        \
    IMG_xxx.c

LOCAL_CFLAGS := -DLOAD_BMP -DLOAD_GIF -DLOAD_LBM -DLOAD_PCX -DLOAD_PNM \
                -DLOAD_SVG -DLOAD_TGA -DLOAD_XCF -DLOAD_XPM -DLOAD_XV
LOCAL_LDLIBS :=
LOCAL_STATIC_LIBRARIES :=
LOCAL_SHARED_LIBRARIES := SDL2

ifeq ($(SUPPORT_JPG),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(JPG_LIBRARY_PATH)
    LOCAL_CFLAGS += -DLOAD_JPG
    LOCAL_STATIC_LIBRARIES += jpeg
endif

ifeq ($(SUPPORT_PNG),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PNG_LIBRARY_PATH)
    LOCAL_CFLAGS += -DLOAD_PNG
    LOCAL_STATIC_LIBRARIES += png
    LOCAL_LDLIBS += -lz
endif

ifeq ($(SUPPORT_WEBP),true)
    LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(WEBP_LIBRARY_PATH)/src
    LOCAL_CFLAGS += -DLOAD_WEBP
    LOCAL_STATIC_LIBRARIES += webp
endif

LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)

include $(BUILD_SHARED_LIBRARY)
