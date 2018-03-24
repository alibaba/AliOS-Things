TOOLS_ROOT ?= $(SOURCE_ROOT)build
COMPILER_ROOT ?=$(TOOLS_ROOT)/compiler


OPENOCD_PATH      := $(TOOLS_ROOT)/OpenOCD/
OPENOCD_CFG_PATH  := $(MAKEFILES_PATH)/OpenOCD/
PATH :=

JTAG         ?= jlink_swd

BUILD_DIR    ?= out

DATE := date

ifeq ($(HOST_OS),Win32)
################
# Windows settings
################
COMMON_TOOLS_PATH := $(TOOLS_ROOT)/cmd/win32/
export SHELL       = cmd.exe
EXECUTABLE_SUFFIX  := .exe
OPENOCD_FULL_NAME := $(OPENOCD_PATH)Win32/openocd.exe
DATE := $(COMMON_TOOLS_PATH)$(DATE)

# Python
ifneq ($(wildcard C:\Python34\python.exe),)
PYTHON_FULL_NAME := C:\Python34\python.exe
endif
ifneq ($(wildcard C:\Python27\python.exe),)
PYTHON_FULL_NAME := C:\Python27\python.exe
endif

SLASH_QUOTE_START :=\"
SLASH_QUOTE_END :=\"

ESC_QUOTE:="
ESC_SPACE:=$(SPACE)
CAT               := type
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo$(EXECUTABLE_SUFFIX)"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo$(EXECUTABLE_SUFFIX)" -n
ECHO              := echo
QUOTES_FOR_ECHO   :=
CMD_TRUNC         := "$(COMMON_TOOLS_PATH)trunc$(EXECUTABLE_SUFFIX)"
PERL              := "$(COMMON_TOOLS_PATH)perl$(EXECUTABLE_SUFFIX)"
PYTHON            := "$(COMMON_TOOLS_PATH)Python27/python$(EXECUTABLE_SUFFIX)"
LINT_EXE          := "$(TOOLS_ROOT)/splint/splint/bin/splint$(EXECUTABLE_SUFFIX)"
PERL_ESC_DOLLAR   :=$$
CLEAN_COMMAND     := if exist "$(BUILD_DIR)" $(call CONV_SLASHES,$(COMMON_TOOLS_PATH))rmdir /s /q "$(BUILD_DIR)"
MKDIR              = if not exist $(subst /,\,$1) mkdir $(subst /,\,$1)
RMDIR              = if exist $(subst /,\,$1) rmdir /s /q $(subst /,\,$1)
CPDIR              = xcopy /s /q /i $(subst /,\,$1) $(subst /,\,$2)
CONV_SLASHES       = $(subst /,\,$1)
DIR                = $(dir $(subst /,\,$1))
TOUCH              = $(ECHO) >
CYGWIN :=
DEV_NULL          := nul
TRUE_CMD          := call
FALSE_CMD         := fail > nul 2>&1

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) $(1)>>$(2)

endef

WRITE_FILE_CREATE =$(file >$(1),$(2))
WRITE_FILE_APPEND =$(file >>$(1),$(2))

else  # Win32
ifeq ($(HOST_OS),Linux32)
################
# Linux 32-bit settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/cmd/linux32/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)Linux32/openocd"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
# dash command shell has built in echo command
ECHO              := echo
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := $(shell which perl)
PYTHON            := $(shell which python)
PERL_ESC_DOLLAR   :=\$$
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf $(BUILD_DIR)
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir" -p $1
RMDIR              = "$(COMMON_TOOLS_PATH)rm" -rf $1
CPDIR              = "$(COMMON_TOOLS_PATH)cp" -rf $1 $2
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' >> $(1);

else # Linux32
ifeq ($(HOST_OS),Linux64)
################
# Linux 64-bit settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/cmd/linux64/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)Linux64/openocd"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
# dash command shell has built in echo command
ECHO              := echo
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := $(shell which perl)
PYTHON            := $(shell which python)
PERL_ESC_DOLLAR   :=\$$
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf $(BUILD_DIR)
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir" -p $1
RMDIR              = "$(COMMON_TOOLS_PATH)rm" -rf $1
CPDIR              = "$(COMMON_TOOLS_PATH)cp" -rf $1 $2
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(subst ',\047,$(subst \,\\,$(2)))' >> $(1);
# # Check the file writing is working correctly
# # should result in: $'""\"\"\\"\\\"\\
# TEST_DATA := $$'""\"\"\\"\\\"\\
# $(info TEST_DATA=$(TEST_DATA))
# $(info $(call WRITE_FILE_CREATE,test.txt,$(TEST_DATA)))
# $(info done=$(shell $(call WRITE_FILE_CREATE,test.txt,$(TEST_DATA))))

else # Linux64
ifeq ($(HOST_OS),OSX)
################
# OSX settings
################

COMMON_TOOLS_PATH := $(TOOLS_ROOT)/cmd/osx/
export SHELL       = $(COMMON_TOOLS_PATH)dash
EXECUTABLE_SUFFIX  :=
OPENOCD_FULL_NAME := "$(OPENOCD_PATH)OSX/openocd"
SLASH_QUOTE_START :=\"
SLASH_QUOTE_END   :=\"
ESC_QUOTE         :=\"
ESC_SPACE         :=\$(SPACE)
CAT               := "$(COMMON_TOOLS_PATH)cat"
ECHO_BLANK_LINE   := "$(COMMON_TOOLS_PATH)echo"
ECHO_NO_NEWLINE   := "$(COMMON_TOOLS_PATH)echo" -n
ECHO              := "$(COMMON_TOOLS_PATH)echo"
QUOTES_FOR_ECHO   :="
CMD_TRUNC         := $(ECHO)
PERL              := $(shell which perl)
PYTHON            := $(shell which python)
PERL_ESC_DOLLAR   :=\$$
CLEAN_COMMAND     := "$(COMMON_TOOLS_PATH)rm" -rf $(BUILD_DIR)
MKDIR              = "$(COMMON_TOOLS_PATH)mkdir" -p $1
RMDIR              = "$(COMMON_TOOLS_PATH)rm" -rf $1
CPDIR              = "$(COMMON_TOOLS_PATH)cp" -rf $1 $2
CONV_SLASHES       = $1
TOUCH              = $(ECHO) >
DEV_NULL          := /dev/null
TRUE_CMD          := true
FALSE_CMD         := false

# $(1) is the content, $(2) is the file to print to.
define PRINT
@$(ECHO) '$(1)'>>$(2)

endef

WRITE_FILE_CREATE =$(ECHO) '$(2)' > $(1);
WRITE_FILE_APPEND =$(ECHO) '$(2)' >> $(1);

else # OSX

$(error incorrect 'make' used ($(MAKE)) - please use:  (Windows) .\make.exe <target_string>    (OS X, Linux) ./make <target_string>)

endif # OSX
endif # Linux64
endif # Linux32
endif # Win32


# Set shortcuts to the compiler and other tools
RM      := "$(COMMON_TOOLS_PATH)rm$(EXECUTABLE_SUFFIX)" -f
CP      := "$(COMMON_TOOLS_PATH)cp$(EXECUTABLE_SUFFIX)" -f
MAKE    := "$(COMMON_TOOLS_PATH)make$(EXECUTABLE_SUFFIX)"
BIN2C   := "$(COMMON_TOOLS_PATH)bin2c$(EXECUTABLE_SUFFIX)"
CURRENT_TIME = $(shell $(DATE) +%Y%m%d.%H%M)


SHOULD_I_WAIT_FOR_DOWNLOAD := $(filter download, $(MAKECMDGOALS))
BUILD_STRING ?= $(strip $(filter-out $(MAKEFILE_TARGETS) download run total, $(MAKECMDGOALS)))
BUILD_STRING_TO_DIR = $(subst .,/,$(1))
DIR_TO_BUILD_STRING = $(subst /,.,$(1))
CLEANED_BUILD_STRING := $(BUILD_STRING)

OUTPUT_DIR   := $(BUILD_DIR)/$(CLEANED_BUILD_STRING)$(BINS)
AUTO_COMPONENT_DIR := $(OUTPUT_DIR)/auto_component

# Newline Macro
define newline


endef


# Use VERBOSE=1 to get full output
ifneq ($(VERBOSE),1)
QUIET:=@
SILENT:=-s
QUIET_SHELL =$(shell $(1))
QUIET_SHELL_IN_MACRO =$$(shell $(1))
else
QUIET:=
SILENT:=
QUIET_SHELL =$(shell $(1)$(info $(1)))
QUIET_SHELL_IN_MACRO =$$(shell $(1)$$(info $(1)))
endif



COMMA :=,

SPACE :=
SPACE +=

# $(1) is a string to be escaped
ESCAPE_BACKSLASHES =$(subst \,\\,$(1))

#########
# Expand wildcard platform names.
# Consider all platforms in platforms/* and platform/*/*
define EXPAND_WILDCARD_PLATFORMS
$(eval POSSIBLE_PLATFORMS :=) \
$(eval EXPANDED_PLATFORMS :=) \
$(foreach ENTRY, $(1), \
$(eval WILDCARD_PLATFORM := $(call BUILD_STRING_TO_DIR, $(ENTRY))) \
$(eval POSSIBLE_PLATFORMS += $(subst board/,,$(wildcard board/$(WILDCARD_PLATFORM)))) \
$(eval POSSIBLE_PLATFORMS += $(subst board/,,$(wildcard board/$(WILDCARD_PLATFORM)/*)))) \
$(eval $(foreach PLATFORM, $(POSSIBLE_PLATFORMS), \
$(eval $(if $(wildcard board/$(PLATFORM)/$(notdir $(PLATFORM)).mk), EXPANDED_PLATFORMS += $(call DIR_TO_BUILD_STRING, $(PLATFORM))))))\
${EXPANDED_PLATFORMS}
endef

##########
# Recurse directories to find valid AOS components.
# $(1) = starting directory
# $(2) = name of variable to which to add components that are found
define RECURSE_DIR_COMPONENT_SEARCH
$(foreach file, $(wildcard $(1)/*), $(if $(wildcard $(file)/*), $(if $(wildcard $(file)/$(notdir $(file)).mk), $(eval $(2) += $(file)),) $(call RECURSE_DIR_COMPONENT_SEARCH,$(file),$(2)),))
endef


##########
# Find all valid components.
# $(1) = name of variable to which to add components that are found
# $(2) = list of component directories
define FIND_VALID_COMPONENTS
$(call RECURSE_DIR_COMPONENT_SEARCH, $(patsubst %/,%,$(SOURCE_ROOT)),$(1)) \
$(eval $(1) := $(subst ./,,$($(strip $(1))))) \
$(foreach compdir, $(2),$(eval $(1) := $(patsubst $(compdir)/%,%,$($(strip $(1)))))) \
$(eval $(1) := $(subst /,.,$($(strip $(1)))))
endef

##########
# Strip duplicate items in list without sorting
# $(1) = List of items to de-duplicate
unique = $(eval seen :=)$(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))${seen}
