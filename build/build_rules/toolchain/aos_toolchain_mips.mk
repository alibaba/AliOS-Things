ifneq ($(filter MIPS% , $(HOST_ARCH)),)

TOOLCHAIN_PATH ?=
ifneq ($(filter $(HOST_MCU_FAMILY), mcu_rda8955), )
TOOLCHAIN_PREFIX := mips-elf-
endif

TOOLCHAIN_PATH := /opt/CSDTK/bin/


CC      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
CXX     := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)g++
AS      := $(CC)
AR      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ar
LD      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ld
CPP     := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)cpp
OPTIONS_IN_FILE_OPTION    := @

ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS   = 
ADD_COMPILER_SPECIFIC_STANDARD_CXXFLAGS = 
ADD_COMPILER_SPECIFIC_STANDARD_ADMFLAGS = $(1)
COMPILER_SPECIFIC_OPTIMIZED_CFLAGS    := -Os
COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS  := -Og
COMPILER_SPECIFIC_PEDANTIC_CFLAGS  := $(COMPILER_SPECIFIC_STANDARD_CFLAGS)
COMPILER_SPECIFIC_ARFLAGS_CREATE   := -rcs
COMPILER_SPECIFIC_ARFLAGS_ADD      := -rcs
COMPILER_SPECIFIC_ARFLAGS_VERBOSE  := -v

# debug: no optimize, full exception inspect and log enabled
COMPILER_SPECIFIC_DEBUG_CFLAGS     := -DDEBUG -ggdb $(COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_DEBUG_CXXFLAGS   := -DDEBUG -ggdb $(COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_DEBUG_ASFLAGS    := -DDEBUG=1
COMPILER_SPECIFIC_DEBUG_LDFLAGS    := -Wl,--gc-sections -Wl,--cref

# inspect: optimize, full exception inspect and log enabled
COMPILER_SPECIFIC_INSPECT_CFLAGS   := -DDEBUG -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_INSPECT_CXXFLAGS := -DDEBUG -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_INSPECT_ASFLAGS  := -DDEBUG=1
COMPILER_SPECIFIC_INSPECT_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref -nostdlib

# release: optimize, minimal exception inspect and less log
COMPILER_SPECIFIC_RELEASE_CFLAGS   := -DNDEBUG $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_CXXFLAGS := -DNDEBUG $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_ASFLAGS  :=
COMPILER_SPECIFIC_RELEASE_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref -nostdlib

# silence: optimize, and log, cli, exception inspect disabled
COMPILER_SPECIFIC_SILENCE_CFLAGS   := -DNDEBUG -DSILENCE $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_SILENCE_CXXFLAGS := -DNDEBUG -DSILENCE $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_SILENCE_ASFLAGS  :=
COMPILER_SPECIFIC_SILENCE_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref -nostdlib

COMPILER_SPECIFIC_DEPS_FLAG        := -MD
COMPILER_SPECIFIC_COMP_ONLY_FLAG   := -c
COMPILER_SPECIFIC_LINK_MAP         = -Wl,-Map,$(1)
COMPILER_SPECIFIC_LINK_FILES       = --whole-archive -Wl,--start-group $(1) 
COMPILER_SPECIFIC_LINK_SCRIPT_DEFINE_OPTION =
COMPILER_SPECIFIC_LINK_SCRIPT      =
LINKER                             := $(CC)
LINK_SCRIPT_SUFFIX                 := .ld
TOOLCHAIN_NAME := GCC

ENDIAN_CFLAGS_LITTLE   := -mlittle-endian
ENDIAN_CXXFLAGS_LITTLE := -mlittle-endian
ENDIAN_ASMFLAGS_LITTLE :=
ENDIAN_LDFLAGS_LITTLE  := -mlittle-endian
CLIB_LDFLAGS_NANO      := --specs=nano.specs
CLIB_LDFLAGS_NANO_FLOAT:= --specs=nano.specs -u _printf_float

# Chip specific flags for GCC

CPU_CFLAGS     :=
CPU_CXXFLAGS   := 
CPU_ASMFLAGS   := 
CPU_LDFLAGS    :=
CLIB_LDFLAGS_NANO       += 
CLIB_LDFLAGS_NANO_FLOAT += 

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_TO_CSV = $(PYTHON) $(MAPFILE_PARSER) $(1) > $(2)

MAPFILE_PARSER            :=$(SCRIPTS_PATH)/map_parse_gcc.py

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_DISPLAY_SUMMARY = $(PYTHON) $(MAPFILE_PARSER) $(1)

#KILL_OPENOCD_SCRIPT := $(SCRIPTS_PATH)/kill_openocd.py

#KILL_OPENOCD = $(PYTHON) $(KILL_OPENOCD_SCRIPT)

OBJDUMP := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objdump
OBJCOPY := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objcopy
STRIP   := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)strip
NM      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)nm

STRIP_OUTPUT_PREFIX := -o
OBJCOPY_HEX_FLAGS := --input-target=elf32-littlemips --output-target=srec
OBJCOPY_BIN_FLAGS       := -O binary 
OBJCOPY_OUTPUT_PREFIX   :=
#OBJCOPY_HEX_FLAGS   := -O ihex 

LINK_OUTPUT_SUFFIX  :=.elf
BIN_OUTPUT_SUFFIX :=.bin
HEX_OUTPUT_SUFFIX :=.srec


endif
