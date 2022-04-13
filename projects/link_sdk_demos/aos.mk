NAME := link_sdk_demos

$(NAME)_TYPE := app

$(NAME)_MBINS_TYPE := app

$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY :=
$(NAME)_MBINS_APP_LD_FILE := app1

ifeq ($(ENABLE_USPACE),1)

ifeq ($(APP_FORMAT),bin)
$(NAME)_SOURCES := src/entry_bin.c
else ifeq ($(APP_FORMAT),elf)
$(NAME)_SOURCES := src/entry_elf.c
else
$(error "Unknown app format")
endif

$(NAME)_COMPONENTS += uspace_lib
$(NAME)_COMPONENTS += usyscall
$(NAME)_COMPONENTS += mbedtls
$(NAME)_COMPONENTS += link_sdk

$(NAME)_SOURCES += src/app.c

$(NAME)_SOURCES-$(LINK_SDK_DEMO_MQTT_BASIC)     += src/demos/mqtt_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_MQTT_BROADCAST) += src/demos/mqtt_broadcast_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_MQTT_RRPC)      += src/demos/mqtt_rrpc_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_DATA_MODEL)     += src/demos/data_model_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_SUBDEV)         += src/demos/subdev_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_BOOTSTRAP)      += src/demos/bootstrap_posix_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_COTA)           += src/demos/cota_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_HTTP)           += src/demos/http_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_NTP)            += src/demos/ntp_posix_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_SHADOW)         += src/demos/shadow_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_LOGPOST)        += src/demos/logpost_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_DIAG)           += src/demos/diag_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_DEVINFO)        += src/demos/devinfo_posix_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_DYNREG)         += src/demos/dynreg_basic_demo.c
$(NAME)_SOURCES-$(LINK_SDK_DEMO_DYNREG_MQTT)    += src/demos/dynregmq_basic_demo.c

$(NAME)_INCLUDES +=  include

$(NAME)_CFLAGS +=  -Werror -Wno-stringop-truncation

endif

