NAME := ota

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 3.0.0
$(NAME)_SUMMARY := An over-the-air update is the wireless delivery of new software. 

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

GLOBAL_INCLUDES += include hal
$(NAME)_INCLUDES += include hal ota_agent/verify ota_agent/mcu tools/xz/include

ifneq (,$(filter breeze,$(COMPONENTS)))
GLOBAL_DEFINES += OTA_CONFIG_BLE
$(NAME)_SOURCES := ota_agent/download/ota_download_ble.c   \
                   ota_agent/transport/ota_transport_ble.c

else ifneq (,$(filter link_sdk,$(COMPONENTS)))
$(NAME)_SOURCES := ota_agent/download/ota_download_http.c   \
                   ota_agent/transport/ota_transport_mqtt.c \
                   ota_agent/download/ota_download_file2fs_http.c

$(NAME)_COMPONENTS += http
else ifneq (,$(filter libiot_coap,$(COMPONENTS)))
$(NAME)_SOURCES := ota_agent/download/ota_download_coap.c   \
                   ota_agent/transport/ota_transport_coap.c
endif

ifeq ($(OTA_CONFIG_SLAVE),y)
$(NAME)_SOURCES := ota_agent/mcu/ota_slaver_upgrade.c
else
$(NAME)_SOURCES += ota_service.c                      \
                   ota_agent/verify/ota_verify_hash.c \
                   ota_agent/verify/ota_verify_rsa.c  \
                   ota_agent/mcu/ota_mcu_ymodem.c     \
                   ota_agent/mcu/ota_mcu_can.c        \
                   ota_agent/mcu/ota_mcu_can_slave.c  \
                   ota_agent/mcu/ota_mcu_ubus.c       \
                   ota_agent/mcu/ota_download_uart.c

$(NAME)_SOURCES += hal/ota_hal_trans.c            \
                   hal/ota_hal_digest.c           \
                   hal/ota_hal_fs_plat.c          \
                   hal/ota_hal_fs_ctrl.c
endif

$(NAME)_SOURCES += hal/ota_hal_os.c               \
                   hal/ota_hal_param.c            \
                   hal/ota_hal_plat.c             \
                   hal/ota_hal_ctrl.c

$(NAME)_COMPONENTS += genie_cjson

ifeq ($(USE_ITLS),y)
   $(NAME)_COMPONENTS += itls
   ifeq ($(COMPILER),armcc)
   GLOBAL_CFLAGS += -DOTA_CONFIG_ITLS -DCONFIG_HTTP_SECURE_ITLS=1 -DOTA_SIGNAL_CHANNEL=1 -DCONFIG_HTTP_SECURE=1
   else
   GLOBAL_DEFINES += OTA_CONFIG_ITLS CONFIG_HTTP_SECURE_ITLS=1 OTA_SIGNAL_CHANNEL=1 CONFIG_HTTP_SECURE=1
   endif
else
   ifeq ($(OTA_CONFIG_SLAVE),y)
   GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=3
   else
      $(NAME)_COMPONENTS += mbedtls
      ifeq ($(COMPILER),armcc)
         ifeq ($(HTTPS_DL),1)
         GLOBAL_CFLAGS += -DOTA_CONFIG_SECURE_DL_MODE
         endif
         ifneq (,$(filter mcu_esp8266,$(HOST_MCU_FAMILY)))
         GLOBAL_CFLAGS += -DOTA_SIGNAL_CHANNEL=1 -DCONFIG_HTTP_SECURE=1 -DOTA_CONFIG_SECURE_DL_MODE
         else
         GLOBAL_CFLAGS += -DCONFIG_HTTP_SECURE=1
         endif
      else
         ifeq ($(HTTPS_DL),1)
         GLOBAL_DEFINES += OTA_CONFIG_SECURE_DL_MODE
         endif
         ifneq (,$(filter mcu_esp8266,$(HOST_MCU_FAMILY)))
         GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1 CONFIG_HTTP_SECURE=1 OTA_CONFIG_SECURE_DL_MODE
         else
         GLOBAL_DEFINES += OTA_SIGNAL_CHANNEL=1 CONFIG_HTTP_SECURE=1
         endif
      endif
   endif
endif
