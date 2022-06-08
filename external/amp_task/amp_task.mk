OSAL_DIR = $(EXTERNAL_DIR)/amp_task

#ifneq ($(PLATFORM_STM32),1)
SRC_MOD += $(wildcard $(OSAL_DIR)/*.c)
#endif

INC += -I$(OSAL_DIR)