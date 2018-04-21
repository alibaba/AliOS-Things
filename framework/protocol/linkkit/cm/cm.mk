NAME := cm
$(NAME)_TYPE := framework
GLOBAL_INCLUDES +=  ./ ./inc
$(NAME)_INCLUDES += ../../protocol/linkkit/alcs/

$(NAME)_SOURCES     := src/iotx_cm_api.c src/iotx_cm_common.c src/iotx_cm_log.c src/iotx_cm_ota.c \
                       src/iotx_local_conn_alcs.c src/iotx_cloud_conn_coap.c src/iotx_cloud_conn_http.c src/iotx_cloud_conn_mqtt.c \
					   src/iotx_cm_cloud_conn.c src/iotx_cm_local_conn.c

$(NAME)_COMPONENTS := framework.common 
GLOBAL_DEFINES += MQTT_DIRECT MQTT_COMM_ENABLED CM_SUPPORT_LOCAL_CONN OTA_SIGNAL_CHANNEL=1 CMP_SUPPORT_MEMORY_MAGIC CM_VIA_CLOUD_CONN_MQTT CM_VIA_CLOUD_CONN NDEBUG
