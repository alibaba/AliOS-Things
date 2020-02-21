NAME := network

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := network

ifeq ($(CONFIG_AOS_LWIP), y)
$(NAME)_COMPONETNS += lwip
endif
