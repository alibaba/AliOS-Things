NAME := libsst

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.1
$(NAME)_SUMMARY := Secure Storage for AliOS Things.
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/isst

SST_DBG := N
SST_MIGRATION := N
DATA_MIGRATION := N
SST_USE_ID2 := Y

ifeq ($(SST_DBG), Y)
$(NAME)_DEFINES += CONFIG_SST_DBG
endif

ifeq ($(SST_MIGRATION), Y)
$(NAME)_DEFINES += CONFIG_SST_MIGRATION=1
endif

ifeq ($(DATA_MIGRATION), Y)
$(NAME)_DEFINES += CONFIG_DATA_MIGRATION=1
endif

ifeq ($(SST_USE_ID2), Y)
$(NAME)_DEFINES += CONFIG_SST_USE_ID2=1
endif

GLOBAL_INCLUDES  += ../include/isst

$(NAME)_INCLUDES += src \
		    src/wrapper/inc \
		    src/osa/inc

$(NAME)_INCLUDES += ../irot/include

$(NAME)_SOURCES += src/osa/aos/sst_osa.c \
		   src/wrapper/km/sst_wrapper.c \
		   src/sst.c

$(NAME)_COMPONENTS := irot alicrypto kv ls_hal

