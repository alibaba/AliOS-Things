NAME := irq

# component information
$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := irq component

# component internal header file folder
GLOBAL_INCLUDES += ../include

ifeq ($(HOST_ARCH),Cortex-M33)
ifneq ($(AOS_KERNEL_MODE),y)
$(NAME)_PREBUILT_LIBRARY += irq_m_u.a
else
$(NAME)_PREBUILT_LIBRARY += irq_m_k.a
endif
endif

ifeq ($(HOST_ARCH),Cortex-A9)
ifneq ($(AOS_KERNEL_MODE),y)
$(NAME)_PREBUILT_LIBRARY += irq_a_u.a
else
$(NAME)_PREBUILT_LIBRARY += irq_a_k.a
endif
endif
