TARGET = vectored_interrupts
CFLAGS += -O2 -fno-builtin-printf -DVECT_IRQ

BSP_BASE = ../../bsp

ASM_SRCS += $(ENV_DIR)/ventry.S
C_SRCS += vectored_interrupts.c
C_SRCS += $(BSP_BASE)/drivers/plic/plic_driver.c

include $(BSP_BASE)/env/common.mk
