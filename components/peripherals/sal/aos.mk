NAME := device_sal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := general component name for sal devices


ifeq (y,$(AOS_COMP_DEVICE_SAL_MK3060))
$(NAME)_COMPONENTS += device_sal_mk3060
else ifeq (y,$(AOS_COMP_DEVICE_SAL_ESP8266))
$(NAME)_COMPONENTS += device_sal_esp8266
else ifeq (y,$(AOS_COMP_DEVICE_SAL_GT202))
$(NAME)_COMPONENTS += device_sal_gt202
else ifeq (y,$(AOS_COMP_DEVICE_SAL_BK7231))
$(NAME)_COMPONENTS += device_sal_bk7231
else ifeq (y,$(AOS_COMP_DEVICE_SAL_SIM800))
$(NAME)_COMPONENTS += device_sal_sim800
else ifeq (y,$(AOS_COMP_DEVICE_SAL_M02H))
$(NAME)_COMPONENTS += device_sal_m02h
else ifeq (y,$(AOS_COMP_DEVICE_SAL_M5310A))
$(NAME)_COMPONENTS += device_sal_m5310a
else
$(info Warning: no SAL device selected, use default mk3060!)
$(NAME)_COMPONENTS += device_sal_mk3060
endif
