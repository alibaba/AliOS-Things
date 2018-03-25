default: Help

export AOS_SDK_VERSION_MAJOR    :=  3
export AOS_SDK_VERSION_MINOR    :=  2
export AOS_SDK_VERSION_REVISION :=  3

export SOURCE_ROOT ?= ./

export MAKEFILES_PATH := $(SOURCE_ROOT)/build
export SCRIPTS_PATH := $(SOURCE_ROOT)/build/scripts

export BINSTYPE_UPPER
export BINSTYPE_LOWER
export UNDERLINE
export RADIXPOINT

MAKEFILE_TARGETS := clean  # targets used by makefile 

BINS ?=

#define BUILD_STRING, AOS toolchain commands on different hosts
include $(MAKEFILES_PATH)/aos_host_cmd.mk

define USAGE_TEXT
Aborting due to invalid targets

Usage: make <target> [download] [run | debug] [JTAG=xxx] [total] [VERBOSE=1] [BINS=app/framework/kernel]
       make run

  <target>
    One each of the following mandatory [and optional] components separated by '@'
      * Application (apps in example)
      * Board ($(filter-out common  include README.txt,$(notdir $(wildcard board/*))))
      * [debug | release] Building for debug or release configurations

  [download]
    Download firmware image to target platform

  [run]
    Reset and run an application on the target hardware

  [total]
    Build all targets related to this application and board

  [JTAG=xxx]
    JTAG interface configuration file from the tools/OpenOCD dirctory
    Default option is jlink_swd

  [VERBOSE=1]
    Shows the commands as they are being executed

  [BINS=app/framework/kernel]
    Build bins: app or framework or kernel

  Notes
    * Component names are case sensitive
    * 'rhino' and 'debug' are reserved component names
    * Component names MUST NOT include space or '@' characters
    * Building for debug is assumed unless '@release' is appended to the target

  Example Usage
    Build for Debug
      $> make helloworld@mk3060

    Build, Download and Run using the default USB-JTAG programming interface
      $> make helloworld@mk3060 download run

    Build for Release
      $> make helloworld@mk3060@release

    Reset and run an application on the target hardware
      $> make run

    Clean output directory
      $> make clean
endef

ifeq (,$(BINS))
BINSTYPE_UPPER :=
BINSTYPE_LOWER :=
UNDERLINE :=
RADIXPOINT :=
else ifeq (app,$(BINS))
BINSTYPE_UPPER :=APP
BINSTYPE_LOWER :=app
UNDERLINE :=_
RADIXPOINT :=.
CREATE_SYSCALLFILE :=$(MAKEFILES_PATH)/scripts/gen_syscalls.py
PARSE_RESOURSE_TO_SYSCALL_FILE = $(PYTHON) $(CREATE_SYSCALLFILE) $(1) $(2)
else ifeq (framework,$(BINS))
BINSTYPE_UPPER :=FRAMEWORK
BINSTYPE_LOWER :=framework
UNDERLINE :=_
RADIXPOINT :=.
CREATE_SYSCALLFILE :=$(MAKEFILES_PATH)/scripts/gen_syscalls.py
PARSE_RESOURSE_TO_SYSCALL_FILE = $(PYTHON) $(CREATE_SYSCALLFILE) $(1) $(2)
else ifeq (kernel,$(BINS))
BINSTYPE_UPPER :=KERNEL
BINSTYPE_LOWER :=kernel
UNDERLINE :=_
RADIXPOINT :=.
CREATE_SYSCALLFILE :=$(MAKEFILES_PATH)/scripts/gen_syscalls.py
PARSE_RESOURSE_TO_SYSCALL_FILE = $(PYTHON) $(CREATE_SYSCALLFILE) $(1) $(2)
else
$(error ***** BINS Error, Valid BINS: [BINS=app BINS=kernel BINS=framework] ***** ***)
endif

############################
# Extra options:
#                CHECK_HEADERS=1 : builds header files to test for their completeness
############################

OPENOCD_LOG_FILE ?= $(BUILD_DIR)/openocd.log
DOWNLOAD_LOG := >> $(OPENOCD_LOG_FILE)

BOOTLOADER_LOG_FILE ?= $(BUILD_DIR)/bootloader.log
export HOST_OS
export VERBOSE
export SUB_BUILD
export OPENOCD_LOG_FILE

COMPONENT_DEPENDENCY_SCRIPT := $(MAKEFILES_PATH)/scripts/component_dependencies.py
COMPONENT_DEPENDENCY = $(PYTHON) $(COMPONENT_DEPENDENCY_SCRIPT)
export COMPONENT_DEPENDENCY

.PHONY: $(BUILD_STRING) main_app bootloader clean Help download total run download_bootloader .gdbinit .gdbinit$(BINSTYPE_LOWER)

Help:
	$(TOOLCHAIN_HOOK_TARGETS)
	$(error $(USAGE_TEXT))

clean:
	$(QUIET)$(ECHO) Cleaning...
	$(QUIET)$(CLEAN_COMMAND)
	$(QUIET)$(RM) -rf .gdbinit .gdbinitkernel .gdbinitframework .gdbinitapp
	$(QUIET)rm -rf build/scripts/config_mk.py
ifneq (,$(wildcard build/scripts/*.pyc))
	$(QUIET)rm -rf build/scripts/*.pyc
endif
	$(QUIET)$(ECHO) Done

ifneq ($(BUILD_STRING),)
-include $(OUTPUT_DIR)/config.mk

ifeq ($(IDE),iar)
COMPILER := iar
CLEANED_BUILD_STRING += COMPILER=iar
else ifeq ($(IDE),keil)
COMPILER := armcc
CLEANED_BUILD_STRING += COMPILER=armcc
endif

# Now we know the target architecture - include all toolchain makefiles and check one of them can handle the architecture
ifeq ($(COMPILER),armcc)
 include $(MAKEFILES_PATH)/aos_toolchain_armcc.mk
else ifeq ($(COMPILER),iar)
 include $(MAKEFILES_PATH)/aos_toolchain_iar.mk
else
 include $(MAKEFILES_PATH)/aos_toolchain_gcc.mk
endif

AUTO_COMPONENT = $(AUTO_COMPONENT_DIR)/auto_component.mk
ifneq ($(test), )
TEST_COMPONENT_COLLECTION = $(AUTO_COMPONENT_DIR)/test_collection.$(test)
else
TEST_COMPONENT_COLLECTION = $(AUTO_COMPONENT_DIR)/test_collection.default
endif

$(AUTO_COMPONENT): $(TEST_COMPONENT_COLLECTION)
	$(QUIET)$(PYTHON) $(MAKEFILES_PATH)/scripts/auto_component.py $(AUTO_COMPONENT_DIR)

$(TEST_COMPONENT_COLLECTION):
	$(QUIET)$(PYTHON) $(MAKEFILES_PATH)/scripts/gen_test_collection.py $(AUTO_COMPONENT_DIR) $(TEST_COMPONENT_COLLECTION)

ifeq ($(COMPILER),armcc)
 TOOLCHAIN_MK = aos_toolchain_armcc.mk
else ifeq ($(COMPILER),iar)
 TOOLCHAIN_MK = aos_toolchain_iar.mk
else
 TOOLCHAIN_MK = aos_toolchain_gcc.mk
endif

$(OUTPUT_DIR)/config.mk: $(MAKEFILES_PATH)/aos_target_config.mk $(MAKEFILES_PATH)/aos_host_cmd.mk $(MAKEFILES_PATH)/$(TOOLCHAIN_MK) $(AOS_SDK_MAKEFILES) $(AUTO_COMPONENT)
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call MKDIR, $(OUTPUT_DIR)/precompile))
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call MKDIR, $(OUTPUT_DIR)/syscall/syscall_kapi))
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call MKDIR, $(OUTPUT_DIR)/syscall/syscall_fapi))
	$(QUIET)$(if $(PARSE_RESOURSE_TO_SYSCALL_FILE), $(call PARSE_RESOURSE_TO_SYSCALL_FILE, $(OUTPUT_DIR), pre-create))
	$(QUIET)$(ECHO) $(if $(AOS_SDK_MAKEFILES),Applying changes made to: $?,Making config file for first time)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_target_config.mk $(CLEANED_BUILD_STRING)
endif

define BINS_EXIT
$(error ***** This Platform Not Support Multi-Binsi! [SUPPORT_BINS:=$(SUPPORT_BINS)] Not Use Option:[BINs=*] ***** ***)
endef

ifneq ($(BINS),)
ifneq ($(SUPPORT_BINS),yes)
BINS_ERROR := error
endif
endif

JOBS ?=4
ifeq (,$(SUB_BUILD))
JOBSNO := -j$(JOBS)
endif

PASSDOWN_TARGETS := $(strip $(filter-out $(MAKEFILE_TARGETS) $(BUILD_STRING),$(MAKECMDGOALS))) #download total
$(PASSDOWN_TARGETS):
	@:

$(BUILD_STRING): main_app $(if $(SUB_BUILD),,.gdbinit .gdbinit$(BINSTYPE_LOWER))

main_app: $(OUTPUT_DIR)/config.mk $(YOS_SDK_PRE_APP_BUILDS) $(MAKEFILES_PATH)/aos_target_build.mk
	$(if $(BINS_ERROR), $(call BINS_EXIT))
	$(QUIET)$(ECHO) Build AOS Now
	$(QUIET)$(ECHO) TOOLCHAIN_PATH=$(TOOLCHAIN_PATH)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/binary)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/modules)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/libraries)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/resources)
	$(QUIET)$(PYTHON) $(MAKEFILES_PATH)/scripts/gen_auto_code.py $(OUTPUT_DIR)/config.mk $(AUTO_COMPONENT_DIR)
	$(QUIET)$(MAKE) -r $(JOBSNO) $(SILENT) -f $(MAKEFILES_PATH)/aos_target_build.mk $(CLEANED_BUILD_STRING) $(PASSDOWN_TARGETS)
	$(QUIET)$(ECHO) Build complete

ifeq ($(SUB_BUILD),)
.gdbinit: $(OUTPUT_DIR)/config.mk $(MAKEFILES_PATH)/aos_host_cmd.mk main_app
	$(QUIET)$(ECHO) Making $@
ifeq ($(HOST_OS), Win32)
	$(QUIET)$(ECHO) #GDB_PATH=$(GDB_COMMAND) > $@
else
	$(QUIET)$(ECHO) '#GDB_PATH=$(GDB_COMMAND)' > $@
endif
	$(QUIET)$(ECHO) set remotetimeout 20 >> $@
	$(QUIET)$(ECHO) $(GDB_KILL_OPENOCD) >> $@
	$(QUIET)$(ECHO) $(GDBINIT_STRING) >> $@

ifneq ($(BINS),)
.gdbinit$(BINSTYPE_LOWER): $(OUTPUT_DIR)/config.mk $(MAKEFILES_PATH)/aos_host_cmd.mk main_app
	$(QUIET)$(ECHO) Making $@
	$(QUIET)$(ECHO) $(SUBGDBINIT_STRING) > $@
endif
endif
