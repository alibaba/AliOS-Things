export SOURCE_ROOT ?= ./
export AOS_OS_PATH := $(SOURCE_ROOT)

LIB_NAME := $(notdir $(LIB_DIR))
LIB_OUT_DIR := $(dir $(LIB_DIR))

ALWAYS_OPTIMISE := 1

BYPASS_LIBRARY_POISON_CHECK=1

include $(LIB_DIR)/$(LIB_NAME)_src.mk
include $(AOS_OS_PATH)/build/aos_toolchain_gcc.mk

SOURCES := $(addprefix $(LIB_DIR)/,$($(NAME)_SOURCES))
LIBRARY_OUTPUT_DIR := $(LIB_OUT_DIR)

GLOBAL_INCLUDES += $(AOS_OS_PATH)include

ifeq ($(HOST_ARCH),ARM968E-S)
GLOBAL_INCLUDES += $(AOS_OS_PATH)platform/arch/arm/armv5
override CPU_CFLAGS := -mcpu=arm968e-s -march=armv5te -mthumb -mthumb-interwork -mlittle-endian
OTHER_CFLAGS := -std=gnu11
else #Linux

ifeq ($(HOST_ARCH), linux)
GLOBAL_INCLUDES += $(AOS_OS_PATH)platform/arch/linux
OTHER_CFLAGS += -m32  -std=gnu99
override ENDIAN_CFLAGS_LITTLE := 

else #Cortex-M3
ifeq ($(HOST_ARCH), Cortex-M3) 
GLOBAL_INCLUDES += $(AOS_OS_PATH)platform/arch/arm/armv7m/gcc/m4

else #Cortex-M4
ifeq ($(HOST_ARCH), Cortex-M4)
GLOBAL_INCLUDES += $(AOS_OS_PATH)platform/arch/arm/armv7m/gcc/m4

else #Cortex-M4F
ifeq ($(HOST_ARCH), Cortex-M4F)
GLOBAL_INCLUDES += $(AOS_OS_PATH)platform/arch/arm/armv7m/gcc/m4

endif #Cortex-M4F
endif #Cortex-M4
endif #Cortex-M3
endif #Linux
endif #ARM968E-S

# Standard library defines
CFLAGS += -c -w -MD -ggdb $(CPU_CFLAGS) $(ENDIAN_CFLAGS_LITTLE) -Wall -fsigned-char -ffunction-sections -Werror -fdata-sections -fno-common $(OTHER_CFLAGS)

CFLAGS += $(addprefix -I,$(GLOBAL_INCLUDES)) $(addprefix -D,$(GLOBAL_DEFINES)) $(addprefix -I$(LIB_DIR)/,$($(NAME)_INCLUDES)) $(addprefix -D,$($(NAME)_DEFINES)) $($(NAME)_CFLAGS)

include $(AOS_OS_PATH)/build/aos_library_build.mk
