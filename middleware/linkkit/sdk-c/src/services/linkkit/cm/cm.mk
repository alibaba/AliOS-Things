NAME := libiot_cm

$(NAME)_SOURCES := ./iotx_cm_log.c \
./iotx_cm_common.c \
./iotx_cm_cloud_conn.c \
./iotx_cm_conn_http.c \
./iotx_cm_conn_mqtt.c \
./iotx_local_conn_alcs.c \
./iotx_cm_api.c \
./iotx_cm_local_conn.c \
./iotx_cm_conn_coap.c \
./iotx_cm_connectivity.c \
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \


$(NAME)_DEFINES += CONFIG_CM_SUPPORT_LOCAL_CONN CONFIG_CM_VIA_CLOUD_CONN

$(NAME)_INCLUDES := ../../../../src/infra \
../../../../src/infra/log \
../../../../src/infra/utils \
../../../../src/infra/utils/misc \
../../../../src/infra/utils/digest \
../../../../src/infra/system \
../../../../src/protocol/mqtt \
../../../../src/protocol/mqtt/client \
../../../../src/protocol/mqtt/MQTTPacket \
./example \

