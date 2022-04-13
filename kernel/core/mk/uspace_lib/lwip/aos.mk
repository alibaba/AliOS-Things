NAME := uspace_lwip

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := some utilities functions used by lwip in user space.

$(NAME)_SOURCES := def.c ip4_addr.c ip6_addr.c

$(NAME)_COMPONENTS += uspace
