NAME := mcu_xm510

HOST_OPENOCD := xm510

# Host architecture is ARM Cortex A5
HOST_ARCH := Cortex-A5

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu xm510

$(NAME)_COMPONENTS += arch_armv7a
$(NAME)_COMPONENTS += newlib_stub rhino lwip cplusplus

#add the following line to the code if POSIX API is needed
$(NAME)_COMPONENTS += osal_posix

#add the following lines to the code if POSIX DEVICE IO is needed
GLOBAL_DEFINES     += POSIX_DEVICE_IO_NEED

#sd card
$(NAME)_COMPONENTS += vfs fatfs
GLOBAL_DEFINES     += CONFIG_AOS_FATFS_SUPPORT_MMC

#jffs
$(NAME)_COMPONENTS += jffs2
#cramfs
$(NAME)_COMPONENTS += cramfs

#ramfs
$(NAME)_COMPONENTS += ramfs

GLOBAL_INCLUDES += ../../arch/arm/armv7a/gcc/a5
GLOBAL_INCLUDES += cpu/include \
                   drivers     \
                   time        \
                   hal

#-mthumb -mthumb-interwork
GLOBAL_CFLAGS += -mcpu=cortex-a5    \
                 -mfpu=vfpv3-d16-fp16 \
                 -mfloat-abi=softfp \
                 -mthumb-interwork  \
                 -mlittle-endian    \
                 -Wno-incompatible-pointer-types

GLOBAL_CFLAGS += -fno-omit-frame-pointer
GLOBAL_CFLAGS += -Wall -Wno-format

GLOBAL_ASMFLAGS += -mcpu=cortex-a5 \
                   -mfpu=vfpv3-d16-fp16 \
                   -mlittle-endian \
                   -mthumb-interwork

GLOBAL_ASMFLAGS += -fno-omit-frame-pointer
GLOBAL_ASMFLAGS += -Wall -Wno-format

#--specs=nosys.specs
GLOBAL_LDFLAGS += -mcpu=cortex-a5     \
                  -mthumb-interwork   \
                  -mlittle-endian     \
                  -Wl,--gc-sections   \
                  -nostartfiles       \
                  --specs=nosys.specs \
                  -usystem_vectors

$(NAME)_CFLAGS += -Wall -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing


$(NAME)_SOURCES := cpu/cpu.c                  \
                   cpu/exception.c            \
                   cpu/mmu_init.c             \
                   cpu/interrupt.c            \
                   cpu/vic.c                  \
                   hal/hal_uart.c             \
                   hal/spi_flash_ctl_alios.c  \
                   applications/soc_init.c    \
                   applications/aos_main.c    \
                   applications/xm_cli.c      \
                   applications/driver_init.c \
                   applications/cmd_file.c    \
                   applications/cmd_date.c    \
                   applications/cmd_net.c     \
                   applications/cmd_ping.c    \
                   applications/cmd_system.c  \
                   applications/regs.c        \
                   applications/soc_impl.c    \
                   applications/hook_impl.c   \
                   drivers/timer.c            \
                   drivers/arasan.c           \
                   drivers/sdio.c             \
                   drivers/sdio/block_dev.c   \
                   drivers/sdio/mmcsd_core.c  \
                   drivers/sdio/sdio.c        \
                   drivers/sdio/mmc.c         \
                   drivers/sdio/sd.c          \
                   time/clock_gettime.c       \
                   libs/system.c

$(NAME)_SOURCES          += hal/flash.c hal/wifi_port.c
$(NAME)_PREBUILT_LIBRARY := libs/libwifixm712.a

GLOBAL_LDS_FILES += platform/mcu/xm510/xm510.ld.S

GLOBAL_DEFINES += CONFIG_ARM
