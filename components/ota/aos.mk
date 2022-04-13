NAME := ota

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.1
$(NAME)_SUMMARY := An over-the-air update is the wireless delivery of new software. 

#default gcc
ifeq ($(COMPILER),)
$(NAME)_CFLAGS      += -Wall -Werror
else ifeq ($(COMPILER),gcc)
$(NAME)_CFLAGS      += -Wall -Werror
endif

$(NAME)_COMPONENTS += ota_agent

GLOBAL_INCLUDES += include hal
$(NAME)_INCLUDES += include hal

ifeq ($(OTA_CONFIG_SLAVE),y)

else
$(NAME)_SOURCES += hal/ota_hal_trans.c            \
                   hal/ota_hal_digest.c           \
                   hal/subdev/ota_hal_subdev_plat.c \
                   hal/subdev/ota_hal_subdev_fsctrl.c
endif

$(NAME)_SOURCES += hal/ota_hal_os.c               \
                   hal/ota_hal_param.c            \
                   hal/ota_hal_plat.c             \
                   hal/ota_hal_ctrl.c

$(NAME)_COMPONENTS += cjson

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
