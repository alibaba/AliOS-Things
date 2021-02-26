NAME := mpy_odla_ocr_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := MPY ODLA OCR DEMO
$(NAME)_SOURCES := maintask.c appdemo.c

$(NAME)_COMPONENTS += haascv
$(NAME)_COMPONENTS += littlefs
$(NAME)_COMPONENTS += fatfs
$(NAME)_COMPONENTS += cli
$(NAME)_COMPONENTS += uai
$(NAME)_COMPONENTS += ucloud
$(NAME)_COMPONENTS += oss
$(NAME)_COMPONENTS += netmgr
$(NAME)_COMPONENTS += libiot_devmodel
$(NAME)_COMPONENTS += libiot_dynreg
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_AWSS) += libiot_awss

GLOBAL_INCLUDES += ./
$(NAME)_INCLUDES += ../../../components/haascv

GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_MMC
GLOBAL_DEFINES += USE_LWIPSOCK

UCLOUD_AI_FACEBODY_ENABLE ?= y
UCLOUD_AI_OBJECTDET_ENABLE ?= y
UCLOUD_AI_OCR_ENABLE ?= y

CONFIG_UAI_SUPPORT_ODLA ?= 1
CONFIG_UAI_USE_CMSIS_NN ?= y
CONFIG_UAI_SUPPORT_IMG_PREPROC ?= 1
