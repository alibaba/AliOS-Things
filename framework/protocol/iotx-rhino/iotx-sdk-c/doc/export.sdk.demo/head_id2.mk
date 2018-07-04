.PHONY: all

CC          := gcc

CFLAGS      := -I../include
CFLAGS      += -I../include/iot-sdk
CFLAGS      += -I../include/mbedtls
CFLAGS      += -DMQTT_ID2_AUTH

LDFLAGS     := -L../lib
LDFLAGS     += -liot_sdk -liot_platform
LDFLAGS     += -ltfs_online -liot-tfs
LDFLAGS     += -Bstatic -liot_tls
LDFLAGS     += -liot_sdk -liot_platform
LDFLAGS     += -lgcov

