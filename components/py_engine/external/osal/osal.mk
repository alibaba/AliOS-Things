OSAL_DIR = $(EXTERNAL_DIR)/osal

#ifneq ($(PLATFORM_STM32),1)
SRC_MOD += $(wildcard $(OSAL_DIR)/stm32/*.c)
#endif

INC += -I$(OSAL_DIR)/include