-include $(TOP_DIR)/aos_board_conf.mk

CONFIG_ENV_CFLAGS   += \
    -ffunction-sections -fdata-sections \
    -Os -DBUILD_AOS \
    -I$(TOP_DIR)/../../../../../include \
    -I$(TOP_DIR)/../../../../activation \

CONFIG_ENV_CFLAGS   += \
    -DWITH_MQTT_ZIP_TOPIC=1 \
    -DWITH_MQTT_SUB_SHORTCUT=1 \
    -DWITH_MQTT_DYN_BUF=1

CONFIG_ENV_CFLAGS               += -Werror
CONFIG_src/ref-impl/tls         :=
CONFIG_src/ref-impl/hal         :=
CONFIG_examples                 :=
CONFIG_src/services/uOTA        :=
CONFIG_tests                    :=
CONFIG_src/tools/linkkit_tsl_convert :=
