NAME := ls_hal

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := link security SDK hardware abstract layer.
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/hal

GLOBAL_INCLUDES  += ../include/hal
GLOBAL_INCLUDES  += ./crypto

$(NAME)_INCLUDES := ./km/inc

$(NAME)_SOURCES  := ./sst/ls_hal_sst.c

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), LINUXHOST)
$(NAME)_SOURCES  += ./km/linux/ls_hal_km.c
endif
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3060)
$(NAME)_SOURCES  += ./km/mk3060/ls_hal_km.c
endif
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3080)
$(NAME)_SOURCES  += ./km/mk3080/ls_hal_km.c
endif
ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), UNO-91H)
$(NAME)_SOURCES  += ./km/uno-91h/ls_hal_km.c
endif

$(NAME)_INCLUDES += ./crypto/demo/inc

$(NAME)_SOURCES  += ./crypto/demo/src/ls_hal_aes.c
$(NAME)_SOURCES  += ./crypto/demo/src/ls_hal_hash.c
$(NAME)_SOURCES  += ./crypto/demo/src/ls_hal_rand.c
$(NAME)_SOURCES  += ./crypto/demo/src/ls_hal_rsa.c
$(NAME)_SOURCES  += ./crypto/demo/src/aes.c
$(NAME)_SOURCES  += ./crypto/demo/src/md5.c
$(NAME)_SOURCES  += ./crypto/demo/src/sha1.c
$(NAME)_SOURCES  += ./crypto/demo/src/sha256.c
$(NAME)_SOURCES  += ./crypto/demo/src/bignum.c
$(NAME)_SOURCES  += ./crypto/demo/src/rsa.c

