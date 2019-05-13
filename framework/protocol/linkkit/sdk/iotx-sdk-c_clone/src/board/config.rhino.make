-include $(TOP_DIR)/aos_board_conf.mk

CONFIG_ENV_CFLAGS   += \
    -g3 --coverage \
    -DBUILD_AOS \
    -DIOTX_NET_INIT_WITH_PK_EXT \
    -DCM_VIA_CLOUD_CONN \
    -DCM_VIA_CLOUD_CONN_MQTT \
    -DCM_SUPPORT_MULTI_THREAD_VIA_HAL \
    -I$(TOP_DIR)/../../../../../include \
    -I$(TOP_DIR)/../../../../activation \
    -DCM_SUPPORT_LOCAL_CONN \
    -DCONFIG_DM_SUPPORT_LOCAL_CONN
   
CONFIG_ENV_CFLAGS   += \
    -DCONFIG_MQTT_TX_MAXLEN=640 \
    -DCONFIG_MQTT_RX_MAXLEN=1200 \

#CONFIG_ENV_CFLAGS   += -Werror
CONFIG_src/ref-impl/tls         :=
CONFIG_src/ref-impl/hal         :=
CONFIG_examples                 :=
CONFIG_src/services/ota         :=
CONFIG_tests                    :=
CONFIG_src/tools/linkkit_tsl_convert :=
