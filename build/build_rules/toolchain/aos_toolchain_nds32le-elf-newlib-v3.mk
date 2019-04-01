ifneq ($(filter $(HOST_ARCH), ANDES_N10),)

TOOLCHAIN_PATH ?=
TOOLCHAIN_PREFIX  := nds32le-elf-
TOOLCHAIN_DEFAULT_FOLDER := nds32le-elf-newlib-v3
ifneq (,$(wildcard $(COMPILER_ROOT)/$(TOOLCHAIN_DEFAULT_FOLDER)/bin))
TOOLCHAIN_PATH    := $(COMPILER_ROOT)/$(TOOLCHAIN_DEFAULT_FOLDER)/bin/
endif

BINS ?=


ifneq (,$(filter $(HOST_OS),Linux32 Linux64))
################
# Linux 32/64-bit settings
################

ifeq (,$(TOOLCHAIN_PATH))
SYSTEM_GCC_PATH = $(shell which $(TOOLCHAIN_PREFIX)gcc)
ifneq (,$(findstring $(TOOLCHAIN_PREFIX)gcc,$(SYSTEM_GCC_PATH)))
TOOLCHAIN_PATH :=
else
$(error can not find compiler toolchain, please download toolchain and unzip to $(COMPILER_ROOT)/$(TOOLCHAIN_DEFAULT_FOLDER) folder)
endif
endif

else # Linux32/64
$(error incorrect 'make' used ($(MAKE)) - please use:  (Windows) .\make.exe <target_string>    (OS X, Linux) ./make <target_string>)
endif


# Notes on C++ options:
# The next two CXXFLAGS reduce the size of C++ code by removing unneeded
# features. For example, these flags reduced the size of a console app build
# (with C++ iperf) from 604716kB of flash to 577580kB of flash and 46756kB of
# RAM to 46680kB of RAM.
#
# -fno-rtti
# Disable generation of information about every class with virtual functions for
# use by the C++ runtime type identification features (dynamic_cast and typeid).
# Disabling RTTI eliminates several KB of support code from the C++ runtime
# library (assuming that you don't link with code that uses RTTI).
#
# -fno-exceptions
# Stop generating extra code needed to propagate exceptions, which can produce
# significant data size overhead. Disabling exception handling eliminates
# several KB of support code from the C++ runtime library (assuming that you
# don't link external code that uses exception handling).

CC      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc$(EXECUTABLE_SUFFIX)"
CXX     := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)g++$(EXECUTABLE_SUFFIX)"
AS      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc$(EXECUTABLE_SUFFIX)"
AR      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ar$(EXECUTABLE_SUFFIX)"
LD      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ld$(EXECUTABLE_SUFFIX)"
CPP     := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)cpp$(EXECUTABLE_SUFFIX)"
OBJDUMP := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objdump$(EXECUTABLE_SUFFIX)"
OBJCOPY := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objcopy$(EXECUTABLE_SUFFIX)"
STRIP   := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)strip$(EXECUTABLE_SUFFIX)"
NM      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)nm$(EXECUTABLE_SUFFIX)"
READELF := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)readelf$(EXECUTABLE_SUFFIX)"

ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS   = $(1) -fno-common $(if $(filter yes,$(MXCHIP_INTERNAL) $(TESTER)),-Werror)
ADD_COMPILER_SPECIFIC_STANDARD_CXXFLAGS = $(1) -fno-common -fno-rtti -fno-exceptions  $(if $(filter yes,$(MXCHIP_INTERNAL) $(TESTER)),-Werror)
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
COMPILER_SPECIFIC_DEBUG_ASFLAGS    := --defsym DEBUG=1 -ggdb
COMPILER_SPECIFIC_DEBUG_LDFLAGS    := -Wl,--gc-sections -Wl,--cref

# inspect: optimize, full exception inspect and log enabled
COMPILER_SPECIFIC_INSPECT_CFLAGS   := -DDEBUG -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_INSPECT_CXXFLAGS := -DDEBUG -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_INSPECT_ASFLAGS  := --defsym DEBUG=1 -ggdb
COMPILER_SPECIFIC_INSPECT_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref

# release_log: optimize, minimal exception inspect and less log
COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS   := -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_LOG_CXXFLAGS := -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_LOG_ASFLAGS  := -ggdb
COMPILER_SPECIFIC_RELEASE_LOG_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref

# release: optimize, and log, cli, exception inspect disabled
COMPILER_SPECIFIC_RELEASE_CFLAGS   := -DNDEBUG -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_CXXFLAGS := -DNDEBUG -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_ASFLAGS  := -ggdb
COMPILER_SPECIFIC_RELEASE_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref

COMPILER_SPECIFIC_DEPS_FLAG        := -MD
COMPILER_SPECIFIC_COMP_ONLY_FLAG   := -c
COMPILER_SPECIFIC_LINK_MAP         =  -Wl,-Map,$(1)
COMPILER_SPECIFIC_LINK_FILES       =  -Wl,--whole-archive -Wl,--start-group $(1) -Wl,--end-group -Wl,-no-whole-archive
COMPILER_SPECIFIC_LINK_SCRIPT_DEFINE_OPTION = -Wl$(COMMA)-T
COMPILER_SPECIFIC_LINK_SCRIPT      =  $(addprefix -Wl$(COMMA)-T ,$(1))
LINKER                             := $(CC) --static -Wl,-static -Wl,--warn-common
LINK_SCRIPT_SUFFIX                 := .ld
TOOLCHAIN_NAME := GCC
OPTIONS_IN_FILE_OPTION    := @

#CPU_ASMFLAGS   := $(CPU_CFLAGS)

COMPILER_UNI_SFLAGS := $(CFLAGS) -DMH4

# Chip specific flags for GCC

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_TO_CSV = $(PYTHON) $(MAPFILE_PARSER) $(1) > $(2)

MAPFILE_PARSER :=$(SCRIPTS_PATH)/map_parse_gcc.py

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_DISPLAY_SUMMARY = $(PYTHON) $(MAPFILE_PARSER) $(1)

#KILL_OPENOCD_SCRIPT := $(SCRIPTS_PATH)/kill_openocd.py

#KILL_OPENOCD = $(PYTHON) $(KILL_OPENOCD_SCRIPT)

LINK_OUTPUT_SUFFIX :=.elf
BIN_OUTPUT_SUFFIX  :=.bin
HEX_OUTPUT_SUFFIX  :=.hex

STRIP_OUTPUT_PREFIX := -o
STRIPFLAGS :=

OBJCOPY_BIN_FLAGS   := -O binary

endif
