NAME = xm510

HOST_OPENOCD := xm510

# Host architecture is ARM Cortex A5
HOST_ARCH := Cortex-A5

$(NAME)_TYPE := kernel

$(NAME)_COMPONENTS += platform/arch/arm/armv7a
$(NAME)_COMPONENTS += libc rhino hal framework.common protocols.net rhino.cplusplus

#add the following line to the code if POSIX API is needed
$(NAME)_COMPONENTS += rhino.posix

#add the following lines to the code if POSIX DEVICE IO is needed
$(NAME)_COMPONENTS += rhino.vfs.posix
GLOBAL_DEFINES     += POSIX_DEVICE_IO_NEED

#sd card
$(NAME)_COMPONENTS += rhino.vfs 3rdparty.experimental.fs.fatfs
GLOBAL_DEFINES     += CONFIG_AOS_FATFS_SUPPORT_MMC

#jffs
$(NAME)_COMPONENTS += 3rdparty.experimental.fs.jffs2
#cramfs 
$(NAME)_COMPONENTS += 3rdparty.experimental.fs.cramfs

#ramfs
$(NAME)_COMPONENTS += kernel.rhino.fs.ramfs

# kv related if kv is enabled
GLOBAL_DEFINES      += CONFIG_AOS_KV_BLKBITS=16
# This value should be same as the kv partition length
GLOBAL_DEFINES      += CONFIG_AOS_KV_BUFFER_SIZE=131072

GLOBAL_INCLUDES += ../../arch/arm/armv7a/gcc/a5
GLOBAL_INCLUDES += cpu/include \
                   drivers \
                   time    \
                   hal

#-mthumb -mthumb-interwork
GLOBAL_CFLAGS += -mcpu=cortex-a5 \
                 -march=armv7-a \
                 -mfpu=vfpv3-d16 \
                 -mfloat-abi=softfp \
                 -mthumb-interwork \
                 -mlittle-endian

GLOBAL_CFLAGS += -fno-omit-frame-pointer
GLOBAL_CFLAGS += -Wall

GLOBAL_ASMFLAGS += -mcpu=cortex-a5 \
                   -march=armv7-a  \
                   -mlittle-endian \
                   -mthumb-interwork

GLOBAL_ASMFLAGS += -fno-omit-frame-pointer
GLOBAL_ASMFLAGS += -Wall

#--specs=nosys.specs
GLOBAL_LDFLAGS += -mcpu=cortex-a5        \
                  -mthumb-interwork \
                  -mlittle-endian \
                  -Wl,--gc-sections \
                  -nostartfiles \
                  --specs=nosys.specs \
                  -usystem_vectors

$(NAME)_CFLAGS += -Wall -Werror -Wno-unused-variable -Wno-unused-parameter -Wno-implicit-function-declaration
$(NAME)_CFLAGS += -Wno-type-limits -Wno-sign-compare -Wno-pointer-sign -Wno-uninitialized
$(NAME)_CFLAGS += -Wno-return-type -Wno-unused-function -Wno-unused-but-set-variable
$(NAME)_CFLAGS += -Wno-unused-value -Wno-strict-aliasing


$(NAME)_SOURCES := cpu/cpu.c          \
                   cpu/exception.c \
                   cpu/mmu_init.c \
                   cpu/interrupt.c \
                   cpu/vic.c  \
                   hal/hal_uart.c \
                   hal/spi_flash_ctl_alios.c \
                   applications/soc_init.c \
                   applications/aos_main.c \
                   applications/xm_cli.c \
                   applications/driver_init.c \
                   applications/cmd_file.c \
                   applications/cmd_date.c \
                   applications/cmd_net.c \
                   applications/cmd_ping.c \
                   applications/cmd_system.c \
                   applications/regs.c \
                   applications/soc_impl.c \
                   applications/hook_impl.c \
                   drivers/timer.c \
                   drivers/arasan.c \
                   drivers/sdio.c \
                   drivers/sdio/block_dev.c \
                   drivers/sdio/mmcsd_core.c \
                   drivers/sdio/sdio.c \
                   drivers/sdio/mmc.c \
                   drivers/sdio/sd.c \
                   time/clock_gettime.c \
                   libs/system.c

$(NAME)_SOURCES += hal/flash.c hal/wifi_port.c
$(NAME)_PREBUILT_LIBRARY := libs/libwifixm712.a

GLOBAL_LDS_FILES += platform/mcu/xm510/xm510.ld.S

GLOBAL_DEFINES += CONFIG_ARM


$(NAME)_SOURCES += test/aos_test.c
$(NAME)_SOURCES += test/cutest/cut.c
GLOBAL_INCLUDES += test/cutest