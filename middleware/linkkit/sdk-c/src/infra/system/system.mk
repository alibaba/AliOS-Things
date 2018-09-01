NAME := libiot_system

$(NAME)_SOURCES := ./guider.c \
./device.c \
./ca.c \
./report.c \
 
$(NAME)_COMPONENTS := 

$(NAME)_INCLUDES := ../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/log \
../../../src/security/pro \
../../../src/security/pro/crypto \
../../../src/security/pro/id2 \
../../../src/security/pro/km \
../../../src/security/pro/itls \

