NAME := id2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 2.0.3
$(NAME)_SUMMARY := ID2 client SDK
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/id2

################################################################################
#id2 config

ifeq ($(CONFIG_LS_KM_SE), y)
$(NAME)_DEFINES     += CONFIG_ID2_KEY_TYPE=ID2_KEY_TYPE_3DES
else ifeq ($(CONFIG_LS_KM_TEE), y)
$(NAME)_DEFINES     += CONFIG_ID2_KEY_TYPE=ID2_KEY_TYPE_AES
else
$(NAME)_DEFINES     += CONFIG_ID2_KEY_TYPE=ID2_KEY_TYPE_AES

ifeq ($(CONFIG_LS_ID2_OTP), y)
$(NAME)_DEFINES     += CONFIG_ID2_OTP
endif
endif

ifeq ($(CONFIG_LS_DEBUG), y)
$(NAME)_DEFINES     += CONFIG_ID2_DEBUG
endif

################################################################################

GLOBAL_INCLUDES   += ../include/id2

$(NAME)_INCLUDES  += ./include
$(NAME)_INCLUDES  += ../include/osa
$(NAME)_INCLUDES  += ../include/irot
$(NAME)_INCLUDES  += ../include/irot/se

$(NAME)_SOURCES   := \
       ./src/core/id2_client.c  \
       ./src/core/otp.c         \
       ./src/plat/id2_plat.c    \
       ./src/utils/id2_util.c

$(NAME)_COMPONENTS := ls_osa
$(NAME)_COMPONENTS += alicrypto
$(NAME)_COMPONENTS += irot

