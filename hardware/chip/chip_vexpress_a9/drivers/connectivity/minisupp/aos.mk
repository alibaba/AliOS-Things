NAME := minisupp

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := wpa_supplicant

ifneq ("$(wildcard $(SOURCE_ROOT)/vendor_mtk_protected/connectivity/minisupp)", "")
include ./vendor/vexpress/mcu/drivers/connectivity/minisupp/module.mk
else
GLOBAL_LDFLAGS += -L$(SOURCE_ROOT)/vendor/vexpress/mcu/drivers/lib/connectivity/minisupp  -lmtk_minisupp
endif