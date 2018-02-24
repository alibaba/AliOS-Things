NAME := xr871

HOST_OPENOCD := xr871

$(NAME)_TYPE := kernel

no_with_lwip := 0
no_with_xip := 0
no_with_image_compress := 0
no_with_ota := 0

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_MMC
GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT

$(NAME)_INCLUDES += .
GLOBAL_INCLUDES += aos
GLOBAL_INCLUDES += include
GLOBAL_INCLUDES += include/net/lwip
GLOBAL_INCLUDES += include/driver/cmsis
GLOBAL_INCLUDES += project
GLOBAL_INCLUDES += project/main
GLOBAL_INCLUDES += project/common/framework
GLOBAL_INCLUDES += project/common/board/xr871_evb_main

$(NAME)_COMPONENTS += platform/arch/arm/armv7m
$(NAME)_COMPONENTS += rhino hal protocols.net yloop framework.common  netmgr mbedtls modules.fs.fatfs vcall libc digest_algorithm
$(NAME)_COMPONENTS += platform/mcu/xr871/src/driver/chip
$(NAME)_COMPONENTS += platform/mcu/xr871/src/image
$(NAME)_COMPONENTS += platform/mcu/xr871/src/net/udhcp
$(NAME)_COMPONENTS += platform/mcu/xr871/src/net/wlan
$(NAME)_COMPONENTS += platform/mcu/xr871/src/ota
$(NAME)_COMPONENTS += platform/mcu/xr871/src/pm
$(NAME)_COMPONENTS += platform/mcu/xr871/src/efpg
$(NAME)_COMPONENTS += platform/mcu/xr871/src/sys
$(NAME)_COMPONENTS += platform/mcu/xr871/src/xz
$(NAME)_COMPONENTS += platform/mcu/xr871/src/net/lwip
$(NAME)_COMPONENTS += platform/mcu/xr871/src/console
$(NAME)_COMPONENTS += platform/mcu/xr871/project
$(NAME)_COMPONENTS += platform/mcu/xr871/aos

$(NAME)_COMPONENTS += platform/mcu/xr871/src/audio/audio_manager
$(NAME)_COMPONENTS += platform/mcu/xr871/src/audio/audio_pcm
$(NAME)_COMPONENTS += platform/mcu/xr871/src/cedarx
$(NAME)_COMPONENTS += platform/mcu/xr871/lib/libmp3
#$(NAME)_COMPONENTS += platform/mcu/xr871/lib/libamr
#$(NAME)_COMPONENTS += platform/mcu/xr871/lib/libamren

GLOBAL_ASMFLAGS += -mcpu=cortex-m4     \
                 -mthumb             \
                 -mfpu=fpv4-sp-d16  \
                 -mfloat-abi=softfp

GLOBAL_CFLAGS += -mcpu=cortex-m4     \
                 -mthumb             \
                 -mfpu=fpv4-sp-d16  \
                 -mfloat-abi=softfp

GLOBAL_CFLAGS += -include prj_config.h
GLOBAL_CFLAGS += -w
GLOBAL_CFLAGS  += -fno-common \
                  -fmessage-length=0 \
                  -fno-exceptions \
                  -ffunction-sections \
                  -fdata-sections \
                  -fomit-frame-pointer \
                  -Wall \
                  -Werror \
                  -Wno-error=unused-function \
                  -MMD -MP \
                  -Os -DNDEBUG

GLOBAL_CFLAGS  += -D__CONFIG_OS_USE_YUNOS
GLOBAL_CFLAGS  += -D__CONFIG_CHIP_XR871
GLOBAL_CFLAGS  += -D__CONFIG_CPU_CM4F
GLOBAL_CFLAGS  += -D__CONFIG_ARCH_DUAL_CORE
GLOBAL_CFLAGS  += -D__CONFIG_ARCH_APP_CORE
GLOBAL_CFLAGS  += -D__CONFIG_LIBC_REDEFINE_GCC_INT32_TYPE
GLOBAL_CFLAGS  += -D__CONFIG_LIBC_PRINTF_FLOAT
GLOBAL_CFLAGS  += -D__CONFIG_LIBC_SCANF_FLOAT
GLOBAL_CFLAGS  += -D__CONFIG_MALLOC_USE_STDLIB
GLOBAL_CFLAGS  += -D__CONFIG_MBUF_IMPL_MODE=0
GLOBAL_CFLAGS  += -D__PRJ_CONFIG_WLAN_STA_AP

ifneq ($(no_with_xip),1)
GLOBAL_CFLAGS  += -D__PRJ_CONFIG_XIP
endif

ifneq ($(no_with_image_compress),1)
GLOBAL_CFLAGS  += -D__PRJ_CONFIG_IMG_COMPRESS
endif

GLOBAL_LDFLAGS += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
GLOBAL_LDFLAGS += -Wl,--gc-sections --specs=nano.specs
GLOBAL_LDFLAGS += -Wl,--wrap,main
GLOBAL_LDFLAGS += -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys

ifneq ($(no_with_xip),1)
GLOBAL_LINK_SCRIPT += platform/mcu/xr871/xr871-xip.ld
else
GLOBAL_LINK_SCRIPT += platform/mcu/xr871/xr871.ld
endif
