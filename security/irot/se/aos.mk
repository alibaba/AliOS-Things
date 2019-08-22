NAME := libkm_se
CHIP_NAME := template

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.0
$(NAME)_SUMMARY := key management for se

$(NAME)_INCLUDES  := ./inc
$(NAME)_INCLUDES  += ./chipset/$(CHIP_NAME)
$(NAME)_INCLUDES  += ./../../include/irot
$(NAME)_INCLUDES  += ./../../include/irot/se

$(NAME)_SOURCES   := src/core/km_to_irot.c
$(NAME)_SOURCES   += src/core/mtk_se_adapter.c
$(NAME)_SOURCES   += src/core/std_se_adapter.c
$(NAME)_SOURCES   += src/log/chip_log.c
$(NAME)_SOURCES   += chipset/$(CHIP_NAME)/se_driver/se_driver.c

$(NAME)_COMPONENTS  := ls_osa

