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
      $> make helloworld@mk108

    Build, Download and Run using the default USB-JTAG programming interface
      $> make helloworld@mk108 download run

    Build for Release
      $> make helloworld@mk108@release

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
else ifeq (framework,$(BINS))
BINSTYPE_UPPER :=FRAMEWORK
BINSTYPE_LOWER :=framework
UNDERLINE :=_
RADIXPOINT :=.
else ifeq (kernel,$(BINS))
BINSTYPE_UPPER :=KERNEL
BINSTYPE_LOWER :=kernel
UNDERLINE :=_
RADIXPOINT :=.
else
$(error BINS Error, Valid BINS: [BINS=app BINS=kernel BINS=framework])
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

.PHONY: $(BUILD_STRING) main_app bootloader clean Help download total run download_bootloader .gdbinit

Help: 
	$(TOOLCHAIN_HOOK_TARGETS)
	$(error $(USAGE_TEXT))

clean:
	$(QUIET)$(ECHO) Cleaning...
	$(QUIET)$(CLEAN_COMMAND)
	$(QUIET)$(RM) -rf .gdbinit
	$(QUIET)$(ECHO) Done

ifneq ($(BUILD_STRING),)
-include $(OUTPUT_DIR)/config.mk
# Now we know the target architecture - include all toolchain makefiles and check one of them can handle the architecture
 include $(MAKEFILES_PATH)/aos_toolchain_gcc.mk

$(OUTPUT_DIR)/config.mk: $(MAKEFILES_PATH)/aos_target_config.mk $(MAKEFILES_PATH)/aos_host_cmd.mk $(MAKEFILES_PATH)/aos_toolchain_gcc.mk $(AOS_SDK_MAKEFILES)
	$(QUIET)$(ECHO) $(if $(AOS_SDK_MAKEFILES),Applying changes made to: $?,Making config file for first time)
	$(QUIET)$(MAKE) -r $(SILENT) -f $(MAKEFILES_PATH)/aos_target_config.mk $(CLEANED_BUILD_STRING)
endif


JOBS ?=4
ifeq (,$(SUB_BUILD))
JOBSNO := -j$(JOBS)
endif

PASSDOWN_TARGETS := $(strip $(filter-out $(MAKEFILE_TARGETS) $(BUILD_STRING),$(MAKECMDGOALS))) #download total
$(PASSDOWN_TARGETS):
	@:

$(BUILD_STRING): main_app $(if $(SUB_BUILD),,.gdbinit)

main_app: $(OUTPUT_DIR)/config.mk $(YOS_SDK_PRE_APP_BUILDS) $(MAKEFILES_PATH)/aos_target_build.mk
	$(QUIET)$(ECHO) Build AOS Now
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/binary)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/modules)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/libraries)
	$(QUIET)$(call MKDIR, $(OUTPUT_DIR)/resources)
	$(QUIET)$(MAKE) -r $(JOBSNO) $(SILENT) -f $(MAKEFILES_PATH)/aos_target_build.mk $(CLEANED_BUILD_STRING) $(PASSDOWN_TARGETS)
	$(QUIET)$(ECHO) Build complete

ifeq ($(SUB_BUILD),)
.gdbinit: $(OUTPUT_DIR)/config.mk $(MAKEFILES_PATH)/aos_host_cmd.mk main_app
	$(QUIET)$(ECHO) Making $@
	$(QUIET)$(ECHO) set remotetimeout 20 > $@
	$(QUIET)$(ECHO) $(GDB_KILL_OPENOCD) >> $@
	$(QUIET)$(ECHO) $(GDBINIT_STRING) >> $@
endif
