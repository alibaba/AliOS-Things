NAME := libiot_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.3.0
$(NAME)_SUMMARY :=

$(NAME)_SOURCES := 
$(NAME)_COMPONENTS := 

$(NAME)_INCLUDES := ../../../src/ref-impl/tls \
../../../src/ref-impl/tls/include \
../../../src/ref-impl/tls/include/mbedtls \
../../../src/ref-impl/tls/include/mbedtls/uOTA_required \
../../../src/ref-impl/tls/library \
../../../src/ref-impl/tls/library/uOTA_required \
 ../../../src/infra \
../../../src/infra/log \
../../../src/infra/utils \
../../../src/infra/utils/misc \
../../../src/infra/utils/digest \
../../../src/infra/system \
 ../../../src/security/pro \
../../../src/security/pro/crypto \
../../../src/security/pro/id2 \
../../../src/security/pro/km \
../../../src/security/pro/itls \
 
