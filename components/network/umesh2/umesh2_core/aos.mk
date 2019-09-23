NAME := umesh2_core

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := uMesh provides the ability to form local area mesh network.

$(NAME)_DEFINES += UTILS_AES_BUILDIN


$(NAME)_DEFINES += USE_ACTION_FRAME

GLOBAL_DEFINES += CONFIG_AOS_MESH

ifeq ($(COMPILER),armcc)
    $(error "umesh2 not support armcc yet")
else ifeq ($(COMPILER),iar)
    $(error "umesh2 not support iar yet")
else
    ifeq ($(HOST_ARCH),Cortex-M3)
        $(error "umesh2 not support Cortex-M3 yet")
    else ifeq ($(HOST_ARCH),Cortex-M4)
        $(NAME)_PREBUILT_LIBRARY := lib/gcc/$(HOST_ARCH)/umesh2_core.a
    else
        $(error "unsupport platform")
    endif
endif
               
$(NAME)_DEFINES += LWIP_IPV6=1

$(NAME)_COMPONENTS-$(UMESH_USE_YLOOP) := yloop
