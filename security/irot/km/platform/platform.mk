NAME := libplat_gen

GLOBAL_INCLUDES    += include

ifeq ($(HOST_ARCH), linux)
HOST_NAME := $(HOST_ARCH)
else
HOST_NAME := $(shell echo $(CONFIG_SYSINFO_DEVICE_NAME)|tr A-Z a-z)
endif

$(NAME)_SOURCES    := $(HOST_NAME)/plat_gen.c

$(NAME)_COMPONENTS := alicrypto
