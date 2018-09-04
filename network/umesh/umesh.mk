NAME := mesh

$(NAME)_MBINS_TYPE := kernel
GLOBAL_INCLUDES += include
GLOBAL_DEFINES += CONFIG_AOS_MESH

ifeq ($(COMPILER),armcc)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/keil/umesh.a
else ifeq ($(COMPILER),iar)
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/iar/umesh.a
else
$(NAME)_PREBUILT_LIBRARY := lib/$(HOST_ARCH)/gcc/umesh.a
endif
