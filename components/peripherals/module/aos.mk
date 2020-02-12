NAME := external_module

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := general external module

$(NAME)_COMPONENTS-$(AOS_SAL_DEVICE) += device_sal
$(NAME)_COMPONENTS-$(AOS_MAL_DEVICE) += device_mal
