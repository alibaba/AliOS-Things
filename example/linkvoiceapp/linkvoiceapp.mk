NAME := linkvoiceapp

$(NAME)_SOURCES := linkvoiceapp.c linkvoice_sample.c
#pal_test.c 
#
ifeq ($(xr871),1)
$(NAME)_SOURCES +=  xplayer.c pcm_record_xr871.c xplayer_init.c oled.c
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/base/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/Cdx2.0Plugin/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/CdxEncPlugin/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/external/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/muxer/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/os_glue/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/parser/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/playback/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/record/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/stream/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/xplayer/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/xrecoder/include
$(NAME)_INCLUDES += ../../platform/mcu/xr871/src/cedarx/common/iniparser/
$(NAME)_DEFINES := MCU_XR871
endif



$(NAME)_COMPONENTS := opus yloop cli linkvoice framework.common  netmgr cjson connectivity/websockets

LWIP:=0
ifeq ($(LWIP),1)
$(NAME)_COMPONENTS  += protocols.net
no_with_lwip := 0
endif

sds:=1
ifeq ($(sds),1)
GLOBAL_DEFINES += CONFIG_SDS MBEDTLS_SSL_MAX_CONTENT_LEN=6144 ALINK_PRODUCT_ATALK 
endif