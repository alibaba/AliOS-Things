NAME := netdev

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.3
$(NAME)_SUMMARY := network device.

$(NAME)_SOURCES-y := src/netdev.c
$(NAME)_SOURCES-y += src/netdev_ipaddr.c
$(NAME)_SOURCES-y += src/netdev_cli.c

GLOBAL_INCLUDES += include
