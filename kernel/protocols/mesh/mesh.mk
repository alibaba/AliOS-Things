NAME := mesh

$(NAME)_TYPE := kernel
GLOBAL_INCLUDES += include

$(NAME)_CFLAGS += -Wall -Werror
GLOBAL_DEFINES += CONFIG_AOS_MESH

PLATFORM := linuxhost
ifeq ($(HOST_ARCH), linux)
PLATFORM := linuxhost
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmesh.a
else ifeq ($(HOST_ARCH), ARM968E-S)
PLATFORM := mk3060
$(NAME)_PREBUILT_LIBRARY := lib/$(PLATFORM)/libmesh.a
else
$(error "not find correct platform!")
endif
