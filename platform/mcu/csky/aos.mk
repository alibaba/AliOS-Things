HOST_OPENOCD := csky

NAME := mcu_csky

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu csky

$(NAME)_COMPONENTS += arch_cskyv2-l
$(NAME)_COMPONENTS += rhino cli

GLOBAL_DEFINES += CONFIG_AOS_UOTA_BREAKPOINT

GLOBAL_ASMFLAGS += -mcpu=ck802 -ffunction-sections -fdata-sections
GLOBAL_CFLAGS   += -mcpu=ck802 -c -Wa,-melrw

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

GLOBAL_LDFLAGS += -mcpu=ck802

$(NAME)_PREBUILT_LIBRARY += ./csi/csi_driver/csky/common/tee/ck802/libcsiteeca.a
$(NAME)_PREBUILT_LIBRARY += ./csi/csi_driver/csky/common/tee/ck802/libtee_client.a
$(NAME)_INCLUDE := csi/csi_driver/include

$(NAME)_SOURCES := hal/uart.c
$(NAME)_SOURCES += aos/aos.c                                \
                   modules/libc/minilibc_port.c             \
                   csi/csi_kernel/rhino/adapter/csi_rhino.c \
                   csi/csi_kernel/rhino/driver/systick.c    \
                   csi/csi_kernel/rhino/driver/hook_impl.c  \
                   hal/ringbuffer.c                         \
                   hal/i2c.c                                \
                   hal/flash.c                              \
                   libs/posix/time/clock_gettime.c

ifneq (y,$(strip $(BSP_SUPPORT_EXTERNAL_MODULE)))
$(NAME)_SOURCES += hal/wifi_port.c
$(NAME)_SOURCES += cli/uart_config.c
endif

ifeq (y, $(CONFIG_AOS_LWIP))
$(NAME)_SOURCES += hal/eth_port.c
endif

GLOBAL_INCLUDES += csi/csi_core/include        \
                   include                     \
                   csi/csi_driver/include      \
                   csi/csi_kernel/include      \
                   csi/csi_kernel/rhino/driver \
                   csi/libs/include            \
                   hal

ifeq ($(HOST_CHIP), zx297100)
GLOBAL_DEFINES += CONFIG_HAVE_ICU
GLOBAL_LDFLAGS += -T platform/board/board_legacy/zx297100_evb/gcc_csky.ld

DRIVERDIR       := csi/csi_driver/sanechips/common
CHIPDIR         := csi/csi_driver/sanechips/zx297100
GLOBAL_INCLUDES += csi/csi_driver/sanechips/zx297100/include \
                   csi/csi_driver/sanechips/common/include

$(NAME)_SOURCES += $(CHIPDIR)/novic_irq_tbl.c    \
                   $(CHIPDIR)/entry.S            \
                   $(CHIPDIR)/startup.S          \
                   $(CHIPDIR)/system.c           \
                   $(CHIPDIR)/isr.c              \
                   $(CHIPDIR)/lib.c              \
                   $(CHIPDIR)/devices.c          \
                   $(CHIPDIR)/pinmux.c           \
                   $(CHIPDIR)/trap_c.c           \
                   $(DRIVERDIR)/zx29_usart.c     \
                   $(DRIVERDIR)/zx29_gpio.c      \
                   $(DRIVERDIR)/zx29_intc.c      \
                   $(DRIVERDIR)/zx29_efc.c       \
                   $(DRIVERDIR)/zx29_clk.c       \
                   $(DRIVERDIR)/zx29_iic.c       \
                   $(DRIVERDIR)/zx29_spi.c       \
                   $(DRIVERDIR)/zx29_timer.c     \
                   $(DRIVERDIR)/zx29_wdt.c       \
                   $(DRIVERDIR)/zx29_icp.c       \
                   $(DRIVERDIR)/zx29_rtc.c       \
                   $(DRIVERDIR)/zx29_sleep_pmu.S \
                   $(DRIVERDIR)/zx29_pmu.c       \
                   $(DRIVERDIR)/zx29_bmu.c
else
GLOBAL_LDFLAGS += -T platform/board/board_legacy/cb2201/gcc_csky.ld
GLOBAL_INCLUDES += csi/csi_driver/csky/ch2201/include \
                   csi/csi_driver/csky/common/include

$(NAME)_SOURCES += csi/csi_driver/csky/common/dw_spi.c        \
                   csi/csi_driver/csky/common/dw_usart.c      \
                   csi/csi_driver/csky/common/ck_eflash.c     \
                   csi/csi_driver/csky/common/dw_timer.c      \
                   csi/csi_driver/csky/common/dw_gpio.c       \
                   csi/csi_driver/csky/common/dw_iic.c        \
                   csi/csi_driver/csky/common/ck_rtc.c        \
                   csi/csi_driver/csky/common/dw_wdt.c        \
                   csi/csi_driver/csky/common/ck_pwm.c        \
                   csi/csi_driver/csky/common/dw_dmac.c       \
                   csi/csi_driver/csky/common/ck_adc.c        \
                   csi/csi_driver/csky/common/ck_trng.c       \
                   csi/csi_driver/csky/common/ck_i2s.c        \
                   csi/csi_driver/csky/common/ck_crc_v1.c     \
                   csi/csi_driver/csky/common/ck_aes.c        \
                   csi/csi_driver/csky/common/ck_rsa.c        \
                   csi/csi_driver/csky/common/ck_sha_v1.c     \
                   csi/csi_driver/csky/ch2201/startup.S       \
                   csi/csi_driver/csky/ch2201/vectors.S       \
                   csi/csi_driver/csky/ch2201/system.c        \
                   csi/csi_driver/csky/ch2201/isr.c           \
                   csi/csi_driver/csky/ch2201/lib.c           \
                   csi/csi_driver/csky/ch2201/devices.c       \
                   csi/csi_driver/csky/ch2201/pinmux.c        \
                   csi/csi_driver/csky/ch2201/trap_c.c        \
                   csi/csi_driver/csky/ch2201/ck_sys_freq.c   \
                   csi/csi_driver/csky/ch2201/novic_irq_tbl.c \
                   csi/libs/libc/malloc.c

$(NAME)_SOURCES += csi/drivers/eth/csi_eth_enc28j60.c
endif

include ./platform/mcu/csky/hal_init/hal_init.mk

