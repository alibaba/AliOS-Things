NAME := mcu_xr871
HOST_OPENOCD := xr871

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu xr871

include $(SOURCE_ROOT)/platform/mcu/xr871/config.mk

$(NAME)_COMPONENTS += arch_armv7m rhino

$(NAME)_INCLUDES += ./drivers

GLOBAL_INCLUDES += drivers/include \
                   drivers/include/net/lwip \
                   drivers/include/driver/cmsis \
                   drivers/project \
                   drivers/project/main \
                   drivers/project/common/framework \
                   drivers/project/common/board/xr871_evb_main

$(NAME)_SOURCES += hal/soc/adc.c \
                   hal/soc/flash.c \
                   hal/soc/gpio.c \
                   hal/soc/i2c.c \
                   hal/soc/pwm.c \
                   hal/soc/rtc.c \
                   hal/soc/sd.c \
                   hal/soc/timer.c \
                   hal/soc/uart.c \
                   hal/soc/wdg.c \
                   hal/soc/spi.c \
                   hal/wifi_port.c \
                   hal/wifi.c \
                   hal/hal.c \

include $(SOURCE_ROOT)/platform/mcu/xr871/sdk_files.mk

$(NAME)_SOURCES += $(XR871_CHIP_FILES) \
                   $(XR871_IMAGE_FILES) \
                   $(XR871_WLAN_FILES) \
                   $(XR871_OTA_FILES) \
                   $(XR871_PM_FILES) \
                   $(XR871_EFPG_FILES) \
                   $(XR871_SYS_FILES) \
                   $(XR871_LWIP_FILES) \
                   $(XR871_CONSOLE_FILES) \
                   $(XR871_PROJECT_FILES) \
                   $(XR871_AUDIO_FILES)


$(NAME)_PREBUILT_LIBRARY := \
                            drivers/lib/libamr.a \
                            drivers/lib/libsc_assistant.a \
                            drivers/lib/libxz/libxz.a

$(NAME)_CFLAGS += -include platform/mcu/xr871/drivers/project/common/prj_conf_opt.h
$(NAME)_CFLAGS += -include platform/mcu/xr871/drivers/project/main/prj_config.h

GLOBAL_CFLAGS += -mcpu=cortex-m4     \
                 -mthumb             \
                 -mfpu=fpv4-sp-d16  \
                 -mfloat-abi=softfp \
                 -w \
                 -fno-common \
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
GLOBAL_CFLAGS  += -D__CONFIG_BIN_COMPRESS
GLOBAL_CFLAGS  += -D__PRJ_CONFIG_IMG_COMPRESS
endif

GLOBAL_LDFLAGS += -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp
GLOBAL_LDFLAGS += -Wl,--gc-sections --specs=nano.specs -u _printf_float
GLOBAL_LDFLAGS += -Wl,--wrap,main
GLOBAL_LDFLAGS += -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys
