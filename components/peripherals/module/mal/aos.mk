NAME := device_mal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := general component name for mal devices

$(NAME)_COMPONENTS-y += mal
$(NAME)_COMPONENTS-$(AOS_MAL_GPRS_SIM800) += device_mal_sim800
