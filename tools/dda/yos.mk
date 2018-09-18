L_PATH := $(call cur-dir)

include $(DEFINE_LOCAL)

L_MODULE := libdda

#L_EXCLUDE_TARGET := true

L_INCS := id2kernel/rhino/arch/$(CONFIG_CHIP_ARCH)
L_INCS += id2kernel/uradar/mesh/include
L_INCS += id2kernel/sys/include

L_CFLAGS := -Werror

L_SRCS += \
    eloop.c \
    agent.c \
    msg.c \
    hal.c

ifeq ($(CONFIG_AOS_DDM), y)
L_SRCS += config_parser.c
L_SRCS += master.c
L_SRCS += dgraph.c
endif

L_INCS += services/framework/include

ifeq ($(CONFIG_AOS_DDM_DG), y)
L_CFLAGS += -DCONFIG_DDA_DGRAPH_ENABLE
endif

include $(BUILD_MODULE)

