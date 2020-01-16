HOST_OPENOCD := dahua

NAME := mcu_dahua

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := driver & sdk for platform/mcu dahua

LWIP := 1

$(NAME)_COMPONENTS += arch_cskyv2-l
$(NAME)_COMPONENTS += rhino netmgr

ifeq ($(LWIP),1)
$(NAME)_COMPONENTS += lwip
no_with_lwip       := 0
GLOBAL_DEFINES     += WITH_LWIP
endif

GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT

GLOBAL_INCLUDES += csi/include

GLOBAL_ASMFLAGS += -mcpu=ck803 -ffunction-sections -fdata-sections
GLOBAL_CFLAGS   += -mcpu=ck803 -c -Wa,-melrw

ifeq ($(CONFIG_HARD_FLOAT), y)
GLOBAL_CFLAGS += -mhard-float
endif
GLOBAL_CFLAGS += $(CONFIG_DEBUG_LEVEL) $(CONFIG_OPTIMIZE_LEVEL) $(CONFIG_ENDIAN_MODE)
GLOBAL_CFLAGS += -Wall
GLOBAL_CFLAGS += -ffunction-sections
GLOBAL_CFLAGS += -fdata-sections
ifeq ($(CONFIG_HAVE_VIC), y)
GLOBAL_CFLAGS += -mistack
endif
GLOBAL_CFLAGS += $(INCLUDEDIRS)

GLOBAL_LDFLAGS += -mcpu=ck803

#$(NAME)_PREBUILT_LIBRARY += ./csi/csi_driver/dahua/common/tee/ck803/libcsiteeca.a
$(NAME)_INCLUDE := csi/csi/csi_driver/include

$(NAME)_SOURCES := hal/uart.c
$(NAME)_SOURCES += aos/aos.c                                    \
                   modules/libc/minilibc_port.c                 \
                   hal/ringbuffer.c                             \
                   hal/i2c.c                                    \
                   hal/flash.c                                  \
                   csi/csi/csi_kernel/rhino/adapter/csi_rhino.c \
                   csi/csi/csi_kernel/rhino/driver/systick.c    \
                   csi/csi/csi_kernel/rhino/driver/hook_impl.c  \
                   libs/posix/time/clock_gettime.c              \
                   hal_init/hal_init.c

ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
$(NAME)_SOURCES += hal/wifi_port.c
endif

ifeq ($(LWIP),1)
$(NAME)_SOURCES += hal/eth_port.c
endif

GLOBAL_INCLUDES += csi/csi/csi_core/include/core    \
                   csi/csi/csi_core/include         \
                   csi/csi/csi_core/include/csi_gcc \
                   include                          \
                   csi/drivers/flash                \
                   csi/csi/csi_driver/include       \
                   csi/libs/include                 \
                   csi/libs/mm                      \
                   csi/libs/mm/include              \
                   csi/csi/csi_kernel/include       \
                   csi/csi/csi_kernel/rhino/driver  \
                   hal

GLOBAL_LDFLAGS  += -T platform/board/dh5021a_evb/configs/gcc_eflash.ld
GLOBAL_INCLUDES += csi/csi/csi_driver/dahua/dh5021a/include

$(NAME)_SOURCES += csi/csi/csi_driver/dahua/common/intc/dw_intc.c   \
                   csi/csi/csi_core/core_ck803S.c                   \
                   modules/libc/minilibc_port.c                     \
                   csi/drivers/flash/lcsfc.c                        \
                   csi/csi/csi_driver/dahua/common/intc/interrupt.c \
                   csi/csi/csi_driver/dahua/common/timer/dw_timer.c \
                   csi/csi/csi_driver/dahua/common/usart/dw_usart.c \
                   csi/csi/csi_driver/dahua/dh5021a/startup.S       \
                   csi/csi/csi_driver/dahua/dh5021a/knlpatch.c      \
                   csi/csi/csi_driver/dahua/dh5021a/system.c        \
                   csi/csi/csi_driver/dahua/dh5021a/isr.c           \
                   csi/csi/csi_driver/dahua/dh5021a/lib.c           \
                   csi/csi/csi_driver/dahua/dh5021a/devices.c       \
                   csi/csi/csi_driver/dahua/dh5021a/pinmux.c        \
                   csi/csi/csi_driver/dahua/dh5021a/ck_sys_freq.c   \
                   csi/csi/csi_driver/dahua/dh5021a/novic_irq_tbl.c

#ifeq ($(LWIP),1)
$(NAME)_SOURCES += csi/drivers/eth/csi_eth_enc28j60.c
#endif

