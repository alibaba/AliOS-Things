NAME := device_sal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := general component name for sal devices

$(NAME)_COMPONENTS-y += sal
$(NAME)_COMPONENTS-$(AOS_SAL_GPRS_SIM800) := device_sal_sim800
$(NAME)_COMPONENTS-$(AOS_SAL_LTE_M02H) := device_sal_m02h
$(NAME)_COMPONENTS-$(AOS_SAL_NBIOT_M5310A) := device_sal_m5310a
$(NAME)_COMPONENTS-$(AOS_SAL_WIFI_BK7231) := device_sal_bk7231
$(NAME)_COMPONENTS-$(AOS_SAL_WIFI_MK3060) := device_sal_mk3060
