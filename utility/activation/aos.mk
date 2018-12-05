NAME := activation
$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 0.0.1.0
$(NAME)_SUMMARY := for device activation
GLOBAL_INCLUDES := .


ifeq ($(HOST_ARCH),ARM968E-S)
$(NAME)_PREBUILT_LIBRARY := ./ARM968E-S/activation.a
endif

ifeq ($(HOST_ARCH),Cortex-M4)
ifeq ($(COMPILER),armcc)
$(NAME)_PREBUILT_LIBRARY := ./Cortex-M4/KEIL/activation.a
else ifeq ($(COMPILER),iar)
$(NAME)_PREBUILT_LIBRARY := ./Cortex-M4/IAR/activation.a
else ifeq ($(ENABLE_VFP),1)
$(NAME)_PREBUILT_LIBRARY := ./Cortex-M4/VFP/activation.a
else
$(NAME)_PREBUILT_LIBRARY := ./Cortex-M4/activation.a
endif
endif

ifeq ($(HOST_ARCH),Cortex-A5)
ifeq ($(COMPILER),armcc)

else ifeq ($(COMPILER),iar)

else
$(NAME)_PREBUILT_LIBRARY := ./Cortex-A5/GCC/activation.a
endif
endif

ifeq ($(HOST_ARCH),linux)
$(NAME)_PREBUILT_LIBRARY := ./linux/activation.a
endif

ifeq ($(HOST_ARCH),xtensa)
ifeq ($(HOST_MCU_FAMILY),esp32)
$(NAME)_PREBUILT_LIBRARY := ./xtensa/esp32/activation.a
else ifeq ($(HOST_MCU_FAMILY),esp8266)
$(NAME)_PREBUILT_LIBRARY := ./xtensa/esp8266/activation.a
endif
endif

ifeq ($(HOST_ARCH),rockchiplinux)
$(NAME)_PREBUILT_LIBRARY := ./rockchiplinux/activation.a
endif

ifeq ($(HOST_ARCH),armhflinux)
$(NAME)_PREBUILT_LIBRARY := ./armhflinux/activation.a
endif
