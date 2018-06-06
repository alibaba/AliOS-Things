ifeq ($(HOST_MCU_NAME), nrf52840)
 include $(SOURCE_ROOT)platform/mcu/$(PLATFORM_MCU_BOARD)/nrf52840.mk 
else ifeq ($(HOST_MCU_NAME), nrf52832)
 include $(SOURCE_ROOT)platform/mcu/$(PLATFORM_MCU_BOARD)/nrf52832.mk 
#ifeq ($(HOST_MCU_NAME),nrf52810)
#include nrf52810.mk
endif
