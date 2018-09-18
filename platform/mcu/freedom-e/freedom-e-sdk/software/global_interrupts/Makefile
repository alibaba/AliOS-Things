TARGET = global_interrupts
CFLAGS += -O2 -fno-builtin-printf -DUSE_LOCAL_ISR

BSP_BASE = ../../bsp

C_SRCS += global_interrupts.c
C_SRCS += $(BSP_BASE)/drivers/plic/plic_driver.c

include $(BSP_BASE)/env/common.mk
