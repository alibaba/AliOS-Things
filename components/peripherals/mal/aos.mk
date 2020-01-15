NAME := device_mal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := general component name for mal devices

ifeq (y,$(AOS_COMP_DEVICE_MAL_SIM800))
$(NAME)_COMPONENTS += device_mal_sim800
# add more mal hal impl here
else
$(info Warning: no MAL device selected, use default sim800!)
$(NAME)_COMPONENTS += device_mal_sim800
endif
