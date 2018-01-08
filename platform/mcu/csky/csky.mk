
NAME := csky

HOST_OPENOCD := csky

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
GLOBAL_INCLUDES += csi_core/include \
                   csi_core/ck802   \
                   include   \
                   include/sys/   \
                   csi_core/include/csi-gcc   \
                   csi_driver/csky/hobbit1_2/include   \
                   csi_driver/include  \
                   csi_core/ck802   \
                   csi_driver/csky/common/usart \
                   csi_kernel/include   \
                   hal   \
                   csi_kernel/driver

CPU += -mcpu=ck802

GLOBAL_CFLAGS += $(CPU) -c -Wa,-melrw

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

GLOBAL_LDFLAGS += -T platform/mcu/csky/csi_driver/csky/hobbit1_2/gcc_csky.ld
GLOBAL_LDFLAGS += -mcpu=ck802 

$(NAME)_SOURCES := hal/uart.c                     
$(NAME)_SOURCES += csi_core/csi_core_dummy.c \
                   csi_core/ck802/core_ck802.c \
                   aos/aos.c \
                   aos/trace_impl.c \
                   csi_driver/csky/common/usart/dw_usart.c \
                   ../../arch/csky/cskyv2-l/cpu_impl.c \
                   ../../arch/csky/cskyv2-l/port_s_novic.S \
                   ../../arch/csky/cskyv2-l/port_c.c \
                   ../../arch/csky/cskyv2-l/entry.S \
                   ../../arch/csky/cskyv2-l/trap_c.c \
                   csi_driver/csky/hobbit1_2/ck_sys_freq.c \
                   csi_driver/csky/hobbit1_2/devices.c \
                   csi_driver/csky/hobbit1_2/isr.c \
                   csi_driver/csky/hobbit1_2/novic_irq_tbl.c \
                   csi_driver/csky/hobbit1_2/startup.S \
                   csi_driver/csky/hobbit1_2/system.c \
                   csi_driver/csky/hobbit1_2/lib.c \
                   csi_driver/csky/hobbit1_2/pinmux.c \
                   csi_kernel/driver/systick.c \
                   hal/ringbuffer.c \
                   hal/i2c.c

                   #../../arch/csky/cskyv2-l/csky_sched.c \
