NAME := das

$(NAME)_VERSION := 1.1.0
$(NAME)_SUMMARY := Link Security SOC Device Attestation SDK for AliOS Things
#if component's header files under another directory, add RPM_INCLUDE_DIR to indicate where the header file folder is located
RPM_INCLUDE_DIR := ../include/das

DEBUG := Y

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), LINUXHOST)
CONFIG_DAS_BOARD := linux
CONFIG_DAS_PLATFORM_ALT := N
CONFIG_DAS_PLATFORM_SHA1_ALT := N
endif

ifeq ($(CONFIG_SYSINFO_DEVICE_NAME), MK3080)
CONFIG_DAS_BOARD := mk3080
CONFIG_DAS_PLATFORM_ALT := N
CONFIG_DAS_PLATFORM_SHA1_ALT := Y
endif

GLOBAL_INCLUDES  += ../include/das

$(NAME)_INCLUDES += src/board/$(CONFIG_DAS_BOARD)

$(NAME)_DEFINES  += DAS_PLATFORM_AOS

ifeq ($(DEBUG), Y)
$(NAME)_DEFINES += DAS_DEBUG
endif

ifeq ($(CONFIG_DAS_PLATFORM_ALT), Y)
$(NAME)_DEFINES += DAS_PLATFORM_ALT
endif
ifeq ($(CONFIG_DAS_PLATFORM_SHA1_ALT), Y)
$(NAME)_DEFINES += DAS_PLATFORM_SHA1_ALT
endif

$(NAME)_SOURCES += src/core/das_core.c
$(NAME)_SOURCES += src/core/utils.c
$(NAME)_SOURCES += src/core/sha1.c
$(NAME)_SOURCES += src/core/proto/lsoc.pb.c
$(NAME)_SOURCES += src/core/proto/pb_encode.c
$(NAME)_SOURCES += src/core/proto/pb_decode.c
$(NAME)_SOURCES += src/core/proto/pb_common.c

$(NAME)_SOURCES += src/service/service.c
$(NAME)_SOURCES += src/service/service_rom.c
$(NAME)_SOURCES += src/service/service_sys.c

$(NAME)_SOURCES += src/board/$(CONFIG_DAS_BOARD)/das_board.c
ifeq ($(CONFIG_DAS_PLATFORM_SHA1_ALT), Y)
$(NAME)_SOURCES += src/board/$(CONFIG_DAS_BOARD)/das_sha1.c
endif

$(NAME)_COMPONENTS := alicrypto

