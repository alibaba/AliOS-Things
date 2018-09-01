TARGET := dhrystone

ASM_SRCS := 
C_SRCS := dhry_stubs.c dhry_printf.c
HEADERS := dhry.h

DHRY_SRCS := dhry_1.c dhry_2.c
DHRY_CFLAGS := -O3 -DTIME -fno-inline -fno-builtin-printf -Wno-implicit -mcmodel=medany -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI)

XLEN ?= 32
CFLAGS := -O3 -fno-common  -mcmodel=medany -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mexplicit-relocs -DNOENUM -falign-functions=4
LDFLAGS := -Wl,--wrap=scanf -Wl,--wrap=printf  -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=medany

DHRY_OBJS := $(patsubst %.c,%.o,$(DHRY_SRCS))
LINK_OBJS := $(DHRY_OBJS)

BSP_BASE = ../../bsp
include $(BSP_BASE)/env/common.mk

$(DHRY_OBJS): %.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(DHRY_CFLAGS) -c -o $@ $<
