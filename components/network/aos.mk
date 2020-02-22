NAME := network

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := network

$(NAME)_COMPONENTS-$(CONFIG_AOS_LWIP) += lwip
