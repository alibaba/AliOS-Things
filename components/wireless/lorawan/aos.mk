NAME := lorawan_stack

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := lorawan stack

$(NAME)_COMPONENTS-$(LORAWAN_STACK_CONFIG_4_4_0) += lorawan_4_4_0
$(NAME)_COMPONENTS-$(LORAWAN_STACK_CONFIG_4_4_2) += lorawan_4_4_2
