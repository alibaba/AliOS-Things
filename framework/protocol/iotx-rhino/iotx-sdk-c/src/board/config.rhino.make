include $(TOP_DIR)/src/board/aos_board_conf.mk
CONFIG_ENV_CFLAGS   += \
    -g3 --coverage \
    -DBUILD_AOS \
    -DCONFIG_HTTP_AUTH_TIMEOUT=500 \
    -DCONFIG_MID_HTTP_TIMEOUT=500 \
    -DCONFIG_GUIDER_AUTH_TIMEOUT=500 \
    -DIOTX_NET_INIT_WITH_PK_EXT \
    -DCM_VIA_CLOUD_CONN \
    -DTEST_HTTP_DAILY   \
    -DCM_VIA_CLOUD_CONN_MQTT \
    -DCM_SUPPORT_MULTI_THREAD_VIA_HAL

   
CONFIG_src/thirdparty/tls :=
#CONFIG_ENV_CFLAGS   += -Werror
CONFIG_samples      :=
CONFIG_src/tools/linkkit_tsl_convert :=

LDFLAGS             += -lpthread

CFLAGS              += -DTEST_ON_DAILY -I$(TOP_DIR)/../../../include 
