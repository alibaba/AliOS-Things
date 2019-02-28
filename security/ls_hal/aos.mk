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

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), LINUXHOST)
$(NAME)_INCLUDES += ./crypto/demo/inc
$(NAME)_DEFINES  += CONFIG_HAL_DEMO

$(NAME)_SOURCES  += ./crypto/demo/aes.c
$(NAME)_SOURCES  += ./crypto/demo/md5.c
$(NAME)_SOURCES  += ./crypto/demo/sha1.c
$(NAME)_SOURCES  += ./crypto/demo/sha256.c

$(NAME)_SOURCES  += ./crypto/ls_hal_aes.c
$(NAME)_SOURCES  += ./crypto/ls_hal_hash.c
$(NAME)_SOURCES  += ./crypto/ls_hal_rand.c
endif

