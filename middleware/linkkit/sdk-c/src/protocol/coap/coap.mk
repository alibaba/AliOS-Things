NAME := libiot_coap

$(NAME)_SOURCES := ./CoAPDeserialize.c \
./iotx_coap_api.c \
./CoAPNetwork.c \
./iotx_ca_cert.c \
./CoAPMessage.c \
./CoAPExport.c \
./CoAPSerialize.c \
 
$(NAME)_COMPONENTS := middleware/linkkit/sdk-c/src/infra/utils \
middleware/linkkit/sdk-c/src/infra/log \
middleware/linkkit/sdk-c/src/infra/system \
middleware/linkkit/sdk-c/src/sdk-impl \


$(NAME)_INCLUDES := ../../../src/infra \
../../../src/infra/log \
../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/system \

