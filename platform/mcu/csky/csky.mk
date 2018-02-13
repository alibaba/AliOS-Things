HOST_OPENOCD := csky

NAME := csky

$(NAME)_TYPE := kernel

#$(NAME)_COMPONENTS += platform/arch/csky/cskyv2-l
$(NAME)_COMPONENTS += rhino hal protocols.net framework.common cjson cli
$(NAME)_COMPONENTS += platform/mcu/csky/hal_init

GLOBAL_DEFINES += CONFIG_AOS_KV_MULTIPTN_MODE
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN=6
GLOBAL_DEFINES += CONFIG_AOS_KV_SECOND_PTN=7
GLOBAL_DEFINES += CONFIG_AOS_KV_PTN_SIZE=4096
GLOBAL_DEFINES += CONFIG_AOS_KV_BUFFER_SIZE=8192
GLOBAL_DEFINES += CONFIG_AOS_CLI_BOARD
GLOBAL_DEFINES += CONFIG_AOS_CLI
GLOBAL_DEFINES += CONFIG_AOS_FOTA_BREAKPOINT
GLOBAL_DEFINES += WITH_LWIP

GLOBAL_INCLUDES += ../../arch/csky/cskyv2-l

GLOBAL_ASMFLAGS += -mcpu=ck802 -ffunction-sections -fdata-sections
GLOBAL_CFLAGS += -mcpu=ck802 -c -Wa,-melrw

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

$(NAME)_SOURCES := hal/uart.c
$(NAME)_SOURCES += aos/aos.c \
                   ../../arch/csky/cskyv2-l/cpu_impl.c \
                   ../../arch/csky/cskyv2-l/port_s_novic.S \
                   ../../arch/csky/cskyv2-l/port_c.c \
                   ../../arch/csky/cskyv2-l/entry.S \
                   aos/trace_impl.c \
                   modules/libc/minilibc_port.c \
                   csi/csi_kernel/rhino/adapter/csi_rhino.c \
                   csi/csi_kernel/rhino/driver/systick.c \
                   csi/csi_kernel/rhino/driver/hook_impl.c \
                   hal/ringbuffer.c \
                   hal/i2c.c \
                   hal/flash.c \
                   libs/posix/time/clock_gettime.c \
                   hal/eth_port.c

GLOBAL_INCLUDES += csi/csi_core/include \
                   include   \
                   csi/csi_driver/include  \
                   csi/csi_kernel/include   \
                   csi/csi_kernel/rhino/driver   \
                   csi/libs/include   \
                   hal

ifeq ($(HOST_CHIP), zx297100)
GLOBAL_DEFINES += CONFIG_HAVE_ICU
GLOBAL_LDFLAGS += -T board/zx297100_evb/gcc_csky.ld

DRIVERDIR := csi/csi_driver/sanechips/common
CHIPDIR := csi/csi_driver/sanechips/zx297100
GLOBAL_INCLUDES += csi/csi_driver/sanechips/zx297100/include \
                   csi/csi_driver/sanechips/common/include

$(NAME)_SOURCES += $(CHIPDIR)/novic_irq_tbl.c \
                   $(CHIPDIR)/entry.S \
                   $(CHIPDIR)/startup.S \
                   $(CHIPDIR)/system.c \
                   $(CHIPDIR)/isr.c \
                   $(CHIPDIR)/lib.c \
                   $(CHIPDIR)/devices.c \
                   $(CHIPDIR)/pinmux.c \
                   $(CHIPDIR)/trap_c.c \
                   $(DRIVERDIR)/zx29_usart.c \
                   $(DRIVERDIR)/zx29_gpio.c \
                   $(DRIVERDIR)/zx29_intc.c \
                   $(DRIVERDIR)/zx29_efc.c \
                   $(DRIVERDIR)/zx29_clk.c \
                   $(DRIVERDIR)/zx29_iic.c \
                   $(DRIVERDIR)/zx29_spi.c \
                   $(DRIVERDIR)/zx29_timer.c \
                   $(DRIVERDIR)/zx29_wdt.c \
                   $(DRIVERDIR)/zx29_icp.c \
                   $(DRIVERDIR)/zx29_rtc.c \
                   $(DRIVERDIR)/zx29_sleep_pmu.S \
                   $(DRIVERDIR)/zx29_pmu.c \
                   $(DRIVERDIR)/zx29_bmu.c
else
GLOBAL_LDFLAGS += -T board/hobbit1_evb/gcc_csky.ld
GLOBAL_INCLUDES += csi/csi_driver/csky/hobbit1_2/include   \
                   csi/csi_driver/csky/common/include

$(NAME)_SOURCES += csi/csi_driver/csky/common/spi/dw_spi.c \
                   csi/csi_driver/csky/common/usart/dw_usart.c \
                   csi/csi_driver/csky/common/eflash/ck_eflash.c \
                   csi/csi_driver/csky/common/timer/dw_timer.c \
                   csi/csi_driver/csky/common/gpio/dw_gpio.c \
                   csi/csi_driver/csky/common/iic/dw_iic.c \
                   csi/csi_driver/csky/common/rtc/ck_rtc.c \
                   csi/csi_driver/csky/common/wdt/dw_wdt.c \
                   csi/csi_driver/csky/common/pwm/ck_pwm.c \
                   csi/csi_driver/csky/common/dmac/dw_dmac.c \
                   csi/csi_driver/csky/common/adc/ck_adc.c \
                   csi/csi_driver/csky/common/trng/ck_trng.c \
                   csi/csi_driver/csky/common/crc/ck_crc.c \
                   csi/csi_driver/csky/common/aes/ck_aes.c \
                   csi/csi_driver/csky/common/rsa/ck_rsa.c \
                   csi/csi_driver/csky/common/sha/ck_sha_v1.c \
                   csi/csi_driver/csky/hobbit1_2/startup.S \
                   csi/csi_driver/csky/hobbit1_2/vectors.S \
                   csi/csi_driver/csky/hobbit1_2/system.c \
                   csi/csi_driver/csky/hobbit1_2/isr.c \
                   csi/csi_driver/csky/hobbit1_2/lib.c \
                   csi/csi_driver/csky/hobbit1_2/devices.c \
                   csi/csi_driver/csky/hobbit1_2/pinmux.c \
                   csi/csi_driver/csky/hobbit1_2/trap_c.c \
                   csi/csi_driver/csky/hobbit1_2/ck_sys_freq.c \
                   csi/csi_driver/csky/hobbit1_2/novic_irq_tbl.c \
                   csi/drivers/eth/csi_eth_enc28j60.c  \
                   csi/libs/libc/malloc.c
endif

