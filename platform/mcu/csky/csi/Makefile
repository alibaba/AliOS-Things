##
 # Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License");
 # you may not use this file except in compliance with the License.
 # You may obtain a copy of the License at
 #
 #   http://www.apache.org/licenses/LICENSE-2.0
 #
 # Unless required by applicable law or agreed to in writing, software
 # distributed under the License is distributed on an "AS IS" BASIS,
 # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 # See the License for the specific language governing permissions and
 # limitations under the License.
##
###############################################################################
# @file     makefile
# @brief    the makefile for the whole project
# @version  V1.0
# @date     02. June 2017
###############################################################################
ABSROOTDIR = $(shell pwd)
ROOTDIR = .

TARGETS_ROOT_PATH ?=
CONFIG_FILE ?=
CONFIG_INC_PATH ?=

ifneq ($(CONFIG_FILE),)
CONFIG_FILE_CHECK:= $(shell ls $(CONFIG_FILE) 2>/dev/null)
endif

ifeq ($(CONFIG_FILE_CHECK),)
$(info "CONFIG_FILE not set")
else

include $(CONFIG_FILE)

ifeq ($(CONFIG_CPU_CK610), y)
CC      = $(TOOL_PATH)csky-elf-gcc
AS      = $(TOOL_PATH)csky-elf-as
AR      = $(TOOL_PATH)csky-elf-ar
LD      = $(TOOL_PATH)csky-elf-ld
DUMP    = $(TOOL_PATH)csky-elf-objdump
OBJCOPY = $(TOOL_PATH)csky-elf-objcopy
else
CC      = $(TOOL_PATH)csky-abiv2-elf-gcc
AS      = $(TOOL_PATH)csky-abiv2-elf-as
AR      = $(TOOL_PATH)csky-abiv2-elf-ar
LD      = $(TOOL_PATH)csky-abiv2-elf-ld
DUMP    = $(TOOL_PATH)csky-abiv2-elf-objdump
OBJCOPY = $(TOOL_PATH)csky-abiv2-elf-objcopy
endif

CONFIG_CHIP_VENDOR_NAME := $(patsubst "%",%,$(strip $(CONFIG_CHIP_VENDOR_STR)))
CONFIG_CHIP_NAME  := $(patsubst "%",%,$(strip $(CONFIG_CHIP_NAME_STR)))
CONFIG_BOARD_NAME := $(patsubst "%",%,$(strip $(CONFIG_BOARD_NAME_STR)))

CONFIG_DEBUG_LEVEL := $(patsubst "%", %, $(strip $(CONFIG_DEBUG_LEVEL)))
CONFIG_OPTIMIZE_LEVEL := $(patsubst "%", %, $(strip $(CONFIG_OPTIMIZE_LEVEL)))

ifeq ($(CONFIG_BIG_ENDIAN),y)
ENDIAN_MODE = -mbig-endian
else
ENDIAN_MODE = -mlittle-endian
endif

OUTDIR = out
OBJDIR = $(OUTDIR)/obj
CSIDRIVERDIR = $(ROOTDIR)/csi_driver
COREDIR = $(ROOTDIR)/csi_core
OFFCHIPDRIVERDIR = $(ROOTDIR)/drivers
DRIVERDIR = $(ROOTDIR)/csi_driver/$(CONFIG_CHIP_VENDOR_NAME)/common
LIBSDIR = $(ROOTDIR)/libs
CHIPDIR = $(ROOTDIR)/csi_driver/$(CONFIG_CHIP_VENDOR_NAME)/$(CONFIG_CHIP_NAME)
KERNELDIR = $(ROOTDIR)/csi_kernel
INCDIR =
CSRC =
LDDIR = $(ROOTDIR)/csi_driver/$(CONFIG_CHIP_VENDOR_NAME)/$(CONFIG_CHIP_NAME)
NAME = $(CONFIG_BOARD_NAME)
PACKNAME = libcsi.a

export CC AS AR LD DUMP OBJCOPY CFLAGS LDFLAGS ASFLAGS INCLUDEDIRS ROOTDIR TARGERDIR DRIVERDIR COREDIR TESTDIR KERNELDIR OFFCHIPDRIVERDIR

$(shell [ -d ${OUTDIR} ] || mkdir -p ${OUTDIR} && mkdir -p ${OBJDIR})

include $(CHIPDIR)/csi.mk
include $(OFFCHIPDRIVERDIR)/csi.mk
include $(LIBSDIR)/csi.mk
ifneq ($(CONFIG_KERNEL_NONE), y)
include $(KERNELDIR)/csi.mk
endif

INCLUDEDIRS = -I$(ROOTDIR)/include -I$(COREDIR)/include -I$(CSIDRIVERDIR)/include -I$(TESTDIR)/include -I$(KERNELDIR)/include -I$(LIBSDIR)/include
INCLUDEDIRS += $(INCDIR)

ifneq ($(TARGETS_ROOT_PATH),)
INCLUDEDIRS += -I$(TARGETS_ROOT_PATH)/include
endif

ifneq ($(CONFIG_INC_PATH),)
INCLUDEDIRS += -I$(CONFIG_INC_PATH)
endif

ifneq ($(BOARD_INCDIR),)
INCLUDEDIRS += -I$(BOARD_INCDIR)
endif

ifeq ($(CONFIG_CPU_CK801), y)
CPU += -mcpu=ck801
endif

ifeq ($(CONFIG_CPU_CK802), y)
CPU += -mcpu=ck802
endif

ifeq ($(CONFIG_CPU_CK803), y)
CPU += -mcpu=ck803sf
endif

ifeq ($(CONFIG_CPU_CK610), y)
ifeq ($(CONFIG_HAS_DSP), y)
CPU += -mcpu=ck610e
endif
endif

CFLAGS += $(CPU) -c -Wa,-melrw

ifeq ($(CONFIG_HARD_FLOAT), y)
CFLAGS += -mhard-float
endif
CFLAGS += $(CONFIG_DEBUG_LEVEL) $(CONFIG_OPTIMIZE_LEVEL) $(CONFIG_ENDIAN_MODE)
CFLAGS += -Wall
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
ifeq ($(CONFIG_HAVE_VIC), y)
CFLAGS += -mistack
endif
CFLAGS += $(INCLUDEDIRS)

ifeq ($(CONFIG_BENCHMARK_COREMARK), y)
CFLAGS += -DCOMPILER_FLAGS=\"-Os\"
endif

LDFLAGS = -EL $(CPU)
ASFLAGS = $(CFLAGS)
ARFLAGS = r


LDLIBS	=  -lm -lc -lgcc  -Wl,--gc-section

ifeq ($(V),1)
Q =
else
Q = @
endif

export Q

.PHONY: all

all: lib
lib: lib_sub

S_SRC = $(wildcard $(SSRC))
C_SRC = $(wildcard $(CSRC))
CPP_SRC      = $(wildcard $(CPPSRC))
CORE_C_SRC   = $(wildcard $(CORE_CSRC))
DRIVER_S_SRC = $(wildcard $(DRIVER_SSRC))
DRIVER_C_SRC = $(wildcard $(DRIVER_CSRC))
KERNEL_S_SRC = $(wildcard $(KERNEL_SSRC))
KERNEL_C_SRC = $(wildcard $(KERNEL_CSRC))
LIB_S_SRC    = $(wildcard $(LIB_SSRC))
LIB_C_SRC    = $(wildcard $(LIB_CSRC))
CMSIS_S_SRC  = $(wildcard $(CMSIS_SSRC))
CMSIS_C_SRC  = $(wildcard $(CMSIS_CSRC))
MBEDOS_S_SRC = $(wildcard $(MBEDOS_SSRC))
MBEDOS_C_SRC = $(wildcard $(MBEDOS_CSRC))
MBEDOS_CPP_SRC = $(wildcard $(MBEDOS_CPPSRC))

DOBJECTS        = $(C_SRC:%.c=%.o) $(S_SRC:%.S=%.o) $(CPP_SRC:%.cpp=%.o)
CORE_DOBJECTS   = $(CORE_C_SRC:%.c=%.o)
DRIVER_DOBJECTS = $(DRIVER_C_SRC:%.c=%.o) $(DRIVER_S_SRC:%.S=%.o)
KERNEL_DOBJECTS = $(KERNEL_C_SRC:%.c=%.o) $(KERNEL_S_SRC:%.S=%.o)
LIB_DOBJECTS    = $(LIB_C_SRC:%.c=%.o) $(LIB_S_SRC:%.S=%.o)
CMSIS_DOBJECTS  = $(CMSIS_C_SRC:%.c=%.o) $(CMSIS_S_SRC:%.S=%.o)
MBEDOS_DOBJECTS = $(MBEDOS_C_SRC:%.c=%.o) $(MBEDOS_S_SRC:%.S=%.o) $(MBEDOS_CPP_SRC:%.cpp=%.o)

L_DEPS := $(DOBJECTS:%o=%d) $(CORE_DOBJECTS:%o=%d) $(DRIVER_DOBJECTS:%o=%d) $(KERNEL_DOBJECTS:%o=%d) $(LIB_DOBJECTS:%o=%d) \
          $(CMSIS_DOBJECTS:%o=%d) $(MBEDOS_DOBJECTS:%o=%d)

lib_sub: $(CORE_DOBJECTS) $(DRIVER_DOBJECTS) $(KERNEL_DOBJECTS) $(LIB_DOBJECTS) $(CMSIS_DOBJECTS) $(MBEDOS_DOBJECTS)
	$(Q)$(AR) $(ARFLAGS) $(OUTDIR)/$(PACKNAME) $(CORE_DOBJECTS) $(DRIVER_DOBJECTS) $(KERNEL_DOBJECTS) $(LIB_DOBJECTS)
	$(Q)$(AR) $(ARFLAGS) $(OUTDIR)/libcmsis.a $(CMSIS_DOBJECTS)
	$(Q)$(AR) $(ARFLAGS) $(OUTDIR)/libmbedos.a $(MBEDOS_DOBJECTS)

main_objs: $(DOBJECTS)

%.o:%.c
	@echo CC $<
	$(Q)$(CC) -MP -MMD $(CFLAGS)  -o $@ $<

%.o:%.S
	@echo AS $<
	$(Q)$(CC) -MP -MMD $(ASFLAGS) -o $@ $<

%.o:%.cpp
	@echo CPP $<
	$(Q)$(CPP) -MP -MMD $(CFLAGS)  -o $@ $<

sinclude $(L_DEPS)

$(NAME).elf: lib_sub main_objs $(LDDIR)/gcc_csky.ld
	@echo LINK $@
	$(Q)$(CC)  -T $(LDDIR)/gcc_csky.ld -o $(OUTDIR)/$(NAME).elf $(CPPLDFLAG_BEGIN) $(LDFLAGS) \
		$(DOBJECTS) $(EOBJECTS) $(CORE_DOBJECTS) $(DRIVER_DOBJECTS) $(KERNEL_DOBJECTS) $(LIB_DOBJECTS) \
		$(CMSIS_DOBJECTS) $(MBEDOS_DOBJECTS) \
		$(SUBDIR_FILES) $(LDLIBS) $(CPPLDFLAG_END) -nostartfiles
	@-cp $(DOBJECTS) $(OBJDIR)
	@echo DUMP $@ to $(NAME).asm
	$(Q)$(DUMP) -S $(OUTDIR)/$(NAME).elf > $(OUTDIR)/$(NAME).asm
	@echo OBJCOPY $@ to $(NAME).bin
	$(Q)$(OBJCOPY) -O binary $(OUTDIR)/$(NAME).elf $(OUTDIR)/$(NAME).bin
	$(Q)$(OBJCOPY) -O srec $(OUTDIR)/$(NAME).elf $(OUTDIR)/$(NAME).hex

.PHONY : debug-make

DEBUG_VARS = 	SUB_DIRS \
		SUBDIR_FILES\
		INCLUDEDIRS \
		CSRC \
		SSRC \
		EOBJECTS \
		DOBJECTS \
		ASFLAGS \
		CFLAGS
#:
#: debug-make -- Print a list of Makefile variables

debug-make:
	@$(foreach var, $(DEBUG_VARS), echo $(var)=$($(var)) ; )

test_core:
	sh ./products/unit_test/core_test/testsuite/core_test.sh

endif

###############
clean:
	rm -rf $(OUTDIR)
#	rm -f  $(DOBJECTS)
#	rm -f  $(L_DEPS)
	find . -name "*.d" | xargs rm -f
	find . -name "*.o" | xargs rm -f
	rm out -fr
#	rm -f include/csi_config.h

tee_os:
	@make -C $(ROOTDIR)/csi_driver/$(CONFIG_CHIP_VENDOR_STR)/common/tee \
	TEEOS_LIB_PATH=$(ABSROOTDIR)/$(CHIPDIR)/tee/ \
	TEEOS_LIB=libtee_os_$(CONFIG_CHIP_NAME_STR).a \
	TARGETS_ROOT_PATH=$(TARGETS_ROOT_PATH)
