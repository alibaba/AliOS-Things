ifneq ($(GEN_SDK_LIB_NET),1)
cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

NET_LIB_NAME := libnet
ifneq ($(SDK),1)
ifneq ($(WIFI_RF_TEST),1)
src_obj := core/ drv/ utils/ net80211/  security/
endif
src_obj += net_os/ wpa_supplicant_l/

ifeq ($(NET_LWIP_SUPPORT),1)
ifeq ($(NET_LWIP_ALIOS_LIB),1)
#alios things need this file, pack into lib
src_obj += lwip/src/netif/ethernetif.o
else
src_obj += lwip/
endif
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
src_obj += mbedtls_client/
endif

ifeq ($(NET_NGHTTP2_SUPPORT),1)
src_obj += nghttp2_client/ 
endif

ifeq ($(NET_FTP_CLIENT_SUPPORT),1)
src_obj += ftp_client/
endif

$(NET_LIB_NAME)-y := $(src_obj)
endif

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
obj-y := $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)
obj-y += $(NET_LIB_NAME).a

ifeq ($(NET_LIBRWS_SUPPORT),1)
obj-y += librws/ 
endif

ifeq ($(NET_NGHTTP2_SUPPORT),1)
obj-y += nghttp2/
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
obj-y += mbedtls/
endif

ifeq ($(NET_LIBCURL_SUPPORT),1)
obj-y += libcurl/
endif

ifeq ($(NET_TINYCBOR_SUPPORT),1)
src_obj += tinycbor/
endif

ifeq ($(NET_MQTT_SUPPORT),1)
obj-y += mqtt/
endif

ifeq ($(NET_COAP_SUPPORT),1)
obj-y += coap/
endif

ifeq ($(NET_JANSSON_SUPPORT),1)
obj-y += jansson/
endif

ifeq ($(NET_CJSON_SUPPORT),1)
obj-y += cJSON/
endif

ifeq ($(NET_JSONC_SUPPORT),1)
obj-y += json-c/
endif

ifeq ($(NET_FTPD_SUPPORT),1)
obj-y += ftpd/ 
endif

ifeq ($(NET_LWIP_SUPPORT)_$(NET_LWIP_ALIOS_LIB),1_1)
obj-y += lwip/
endif


else


cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

LIB_NAME := libnet
ifneq ($(SDK),1)
ifneq ($(WIFI_RF_TEST),1)
src_obj := core/ drv/ utils/ net80211/  security/
endif
src_obj += net_os/ wpa_supplicant_l/

ifeq ($(NET_LWIP_SUPPORT),1)
ifeq ($(NET_LWIP_ALIOS_LIB),1)
#alios things need this file, pack into lib
src_obj += lwip/src/netif/ethernetif.o
else
src_obj += lwip/
endif
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
src_obj += mbedtls_client/
endif

ifeq ($(NET_NGHTTP2_SUPPORT),1)
src_obj += nghttp2_client/
endif

obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.c))
obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.S))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))
src_obj += $(obj_c:.c=.o) $(obj_s:.S=.o) $(obj_cpp:.cpp=.o)

ifeq ($(NET_LIBRWS_SUPPORT),1)
src_obj += librws/
endif

ifeq ($(NET_NGHTTP2_SUPPORT),1)
src_obj += nghttp2/
endif

ifeq ($(NET_MBEDTLS_SUPPORT),1)
src_obj += mbedtls/
endif

ifeq ($(NET_LIBCURL_SUPPORT),1)
src_obj += libcurl/
endif

ifeq ($(NET_TINYCBOR_SUPPORT),1)
src_obj += tinycbor/
endif

ifeq ($(NET_MQTT_SUPPORT),1)
src_obj += mqtt/
endif

ifeq ($(NET_COAP_SUPPORT),1)
src_obj += coap/
endif

ifeq ($(NET_JANSSON_SUPPORT),1)
src_obj += jansson/
endif

ifeq ($(NET_CJSON_SUPPORT),1)
src_obj += cJSON/
endif

ifeq ($(NET_JSONC_SUPPORT),1)
src_obj += json-c/
endif

ifeq ($(NET_FTPD_SUPPORT),1)
src_obj += ftpd/
endif

# src_obj += json-c_wrap/
# only used for BEST2001 METAL_ID_0

ifeq ($(NET_FTP_CLIENT_SUPPORT),1)
src_obj += ftp_client/
endif

$(LIB_NAME)-y := $(src_obj)
endif

obj-y :=

ifeq ($(NET_LWIP_SUPPORT)_$(NET_LWIP_ALIOS_LIB),1_1)
LIB_NAME_LWIP := libnetlwip
ifneq ($(SDK),1)
src_obj := lwip/
$(LIB_NAME_LWIP)-y := $(src_obj)
endif
obj-y += $(LIB_NAME_LWIP).a
endif

obj-y += $(LIB_NAME).a
endif

ccflags-y := -Iplatform/hal -Iplatform/cmsis -Iplatform/cmsis/inc -Inet/include/  -Inet/include/byteorder  -Inet/include/os  -Inet/include/unaligned
ccflags-y += -Inet/net80211 -Inet/core -Inet/utils -Inet/net_os -Inet/drv/cw1260 -Inet/drv/drv_os -Inet/security -Inet/wpa_supplicant_l
ccflags-y += -Iplatform/drivers/wifi/
ccflags-y += -Iservices/wifi_app/dhcpd/
ifeq ($(APP_ALEXA_SUPPORT),1)
ccflags-y += -Inet/mbedtls/library/ \
			 -Inet/nghttp2/library/ \
			 -Inet/nghttp2/library/includes/
endif

ccflags-y += -Inet/libcurl/library/

subdir-ccflags-y +=\
			-Inet/\
			-Inet/include/os/\
			-Inet/lwip/src/include/ \
			-Inet/lwip/src/include/compat/posix/ \
			-Inet/coap/libcoap/include/coap2/ \
			-Inet/coap/port/include/ \
			-Iservices/bt_profiles_enhanced/inc/\
			-Iservices/nvrecord/\
			-Iservices/sys_time/ \
			-Iservices/wifi_app/airkiss/ \
			-Iplatform/drivers/wifi/$(CHIP) \
			-Iutils/heap/ \
			-Iapps/alios/genie/include
ifeq ($(APP_ALEXA_SUPPORT),1)
subdir-ccflags-y += -Inet/mbedtls/library/ \
			 -Inet/nghttp2/library/ \
			 -Inet/nghttp2/library/includes/
endif
subdir-ccflags-y += -Inet/libcurl/library/

ifeq ($(ALIOS_APP_SUPPORT),1)
ccflags-y += -Iapps/alios/genie/include/
endif

ifneq ($(SKB_SRAM), )
subdir-ccflags-y += -DSKB_SRAM=$(SKB_SRAM)
endif

subdir-ccflags-y += -DCW1260_DETECTION_LOGIC
subdir-ccflags-y += -DP2P_MULTIVIF
#subdir-ccflags-y += -DCUSTOM_FEATURE
subdir-ccflags-y += -DMCAST_FWDING
subdir-ccflags-y += -DCW1200_SUSPEND_RESUME_FILTER_ENABLE
subdir-ccflags-y += -DAP_AGGREGATE_FW_FIX
subdir-ccflags-y += -DAP_HT_CAP_UPDATE
subdir-ccflags-y += -DCONFIG_CW1200_USE_STE_EXTENSIONS
#subdir-ccflags-y += -DCONFIG_CW1200_5GHZ_SUPPORT

ifeq ($(ALSA_RESAMPLE_USER_HOOK_EN),1)
ccflags-y += -Iservices/wifi_app/alsa_adapter/
ccflags-y += -DALSA_RESAMPLE_USER_HOOK_EN
endif

