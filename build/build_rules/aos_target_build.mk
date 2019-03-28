include $(MAKEFILES_PATH)/aos_host_cmd.mk
include $(CONFIG_FILE)
include $(TOOLCHAIN_MAKEFILE)

.PHONY: display_map_summary build_done

##################################
# Filenames
##################################

# out/helloworld@xx/binary/helloworld@xx.elf
LINK_OUTPUT_FILE          :=$(OUTPUT_DIR)/binary/$(CLEANED_BUILD_STRING)$(RADIXPOINT)$(MBINSTYPE_LOWER)$(LINK_OUTPUT_SUFFIX)
# out/helloworld@xx/binary/helloworld@xx.stripped.elf
STRIPPED_LINK_OUTPUT_FILE :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.stripped$(LINK_OUTPUT_SUFFIX))
# out/helloworld@xx/binary/helloworld@xx.bin
BIN_OUTPUT_FILE           :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=$(BIN_OUTPUT_SUFFIX))
# out/helloworld@xx/binary/helloworld@xx.bin
HEX_OUTPUT_FILE           :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=$(HEX_OUTPUT_SUFFIX))
# out/helloworld@xx/binary/helloworld@xx.map
MAP_OUTPUT_FILE           :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.map)
# out/helloworld@xx/binary/helloworld@xx_map.csv
MAP_CSV_OUTPUT_FILE       :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=_map.csv)

BIN_OUTPUT_FILE_TMP       :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.tmptmp.bin)

ifeq ($(PING_PONG_OTA),1)
LINK_OUTPUT_FILE_XIP2     :=$(LINK_OUTPUT_FILE:$(LINK_OUTPUT_SUFFIX)=.xip2$(LINK_OUTPUT_SUFFIX))
STRIPPED_LINK_OUTPUT_FILE_XIP2 :=$(LINK_OUTPUT_FILE_XIP2:$(LINK_OUTPUT_SUFFIX)=.stripped$(LINK_OUTPUT_SUFFIX))
BIN_OUTPUT_FILE_XIP2            :=$(LINK_OUTPUT_FILE_XIP2:$(LINK_OUTPUT_SUFFIX)=$(BIN_OUTPUT_SUFFIX))
HEX_OUTPUT_FILE_XIP2            :=$(LINK_OUTPUT_FILE_XIP2:$(LINK_OUTPUT_SUFFIX)=$(HEX_OUTPUT_SUFFIX))
MAP_OUTPUT_FILE_XIP2            :=$(LINK_OUTPUT_FILE_XIP2:$(LINK_OUTPUT_SUFFIX)=.map)
MAP_CSV_OUTPUT_FILE_XIP2        :=$(LINK_OUTPUT_FILE_XIP2:$(LINK_OUTPUT_SUFFIX)=_map.csv)
endif

OPENOCD_LOG_FILE          ?= $(OUTPUT_DIR)/openocd_log.txt

LIBS_DIR                  := $(OUTPUT_DIR)/libraries
LINK_OPTS_FILE            := $(OUTPUT_DIR)/binary/link$(UNDERLINE)$(MBINSTYPE_LOWER).opts

LINT_OPTS_FILE            := $(OUTPUT_DIR)/binary/lint$(UNDERLINE)$(MBINSTYPE_LOWER).opts

LDS_FILE_DIR              := $(OUTPUT_DIR)/ld

ifeq (,$(SUB_BUILD))
ifneq (,$(EXTRA_TARGET_MAKEFILES))
$(foreach makefile_name,$(EXTRA_TARGET_MAKEFILES),$(eval include $(makefile_name)))
endif
endif

##################################
# Macros
##################################

###############################################################################
# MACRO: GET_BARE_LOCATION
# Returns a the location of the given component relative to source-tree-root
# rather than from the cwd
# $(1) is component
GET_BARE_LOCATION =$(patsubst $(call ESCAPE_BACKSLASHES,$(SOURCE_ROOT))%,%,$(strip $(subst :,/,$($(1)_LOCATION))))

###############################################################################
# MACRO: BUILD_C_RULE
# Creates a target for building C language files (*.c)
# $(1) is component, $(2) is the source file
define BUILD_C_RULE
ifeq ($(COMPILER),)
-include $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2:.c=.d)
endif
$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2:.c=.o): $(strip $($(1)_LOCATION))$(2) $(CONFIG_FILE) $$(dir $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2)).d $(RESOURCES_DEPENDENCY) $(LIBS_DIR)/$(1).c_opts $(PROCESS_PRECOMPILED_FILES) | $(EXTRA_PRE_BUILD_TARGETS)
	$$(if $($(1)_START_PRINT),,$(eval $(1)_START_PRINT:=1) $(QUIET)$(ECHO) Compiling $(1) )
	$(QUIET)$(CC) $($(1)_C_OPTS) -D__FILENAME__='"$$(notdir $$<)"' $(call COMPILER_SPECIFIC_DEPS_FILE,$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2:.c=.d)) -o $$@ $$< $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
endef

###############################################################################
# MACRO: CHECK_HEADER_RULE
# Compiles a C language header file to ensure it is stand alone complete
# $(1) is component, $(2) is the source header file
define CHECK_HEADER_RULE
$(eval $(1)_CHECK_HEADER_LIST+=$(OUTPUT_DIR)/Modules/$(strip $($(1)_LOCATION))$(2:.h=.chk) )
.PHONY: $(OUTPUT_DIR)/Modules/$(strip $($(1)_LOCATION))$(2:.h=.chk)
$(OUTPUT_DIR)/Modules/$(strip $($(1)_LOCATION))$(2:.h=.chk): $(strip $($(1)_LOCATION))$(2) $(CONFIG_FILE) $$(dir $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2)).d
	$(QUIET)$(ECHO) Checking header  $(2)
	$(QUIET)$(CC) -c $(AOS_SDK_CFLAGS) $(filter-out -pedantic -Werror, $($(1)_CFLAGS) $(C_BUILD_OPTIONS) ) $($(1)_INCLUDES) $($(1)_DEFINES) $(AOS_SDK_INCLUDES) $(AOS_SDK_DEFINES) -o $$@ $$<
endef

###############################################################################
# MACRO: BUILD_CPP_RULE
# Creates a target for building C++ language files (*.cpp)
# $(1) is component name, $(2) is the source file
define BUILD_CPP_RULE
-include $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(patsubst %.cc,%.d,$(2:.cpp=.d))
$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(patsubst %.cc,%.o,$(2:.cpp=.o)): $(strip $($(1)_LOCATION))$(2) $(CONFIG_FILE) $$(dir $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(2)).d $(RESOURCES_DEPENDENCY) $(LIBS_DIR)/$(1).cpp_opts | $(EXTRA_PRE_BUILD_TARGETS)
	$$(if $($(1)_START_PRINT),,$(eval $(1)_START_PRINT:=1) $(ECHO) Compiling $(1))
	$(QUIET)$(CXX) $($(1)_CPP_OPTS) -o $$@ $$< $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
endef

###############################################################################
# MACRO: BUILD_S_RULE
# Creates a target for building Assembly language files (*.s & *.S)
# $(1) is component name, $(2) is the source file
define BUILD_S_RULE
$(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(strip $(patsubst %.S,%.o, $(2:.s=.o) )): $(strip $($(1)_LOCATION))$(2) $($(1)_PRE_BUILD_TARGETS) $(CONFIG_FILE) $$(dir $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))$(strip $(patsubst %.S, %.o, $(2)))).d $(RESOURCES_DEPENDENCY) $(LIBS_DIR)/$(1).as_opts $(PROCESS_PRECOMPILED_FILES) | $(EXTRA_PRE_BUILD_TARGETS)
	$$(if $($(1)_START_PRINT),,$(eval $(1)_START_PRINT:=1) $(ECHO) Compiling $(1))
	$(QUIET)$(AS) $($(1)_S_OPTS) -o $$@ $$< $(COMPILER_SPECIFIC_STDOUT_REDIRECT)
endef

###############################################################################
# MACRO: BUILD_COMPONENT_RULES
# Creates targets for building an entire component
# Target for the component static library is created in this macro
# Targets for source files are created by calling the macros defined above
# $(1) is component name
define BUILD_COMPONENT_RULES

$(eval LINK_LIBS +=$(if $(strip $($(1)_SOURCES)),$(LIBS_DIR)/$(1).a))
$(eval LINK_LIBS +=$(if $($(1)_SELF_BUIlD_COMP_targets),$(LIBS_DIR)/$(notdir $($(1)_SELF_BUIlD_COMP_targets) )))

ifneq ($($(1)_PRE_BUILD_TARGETS),)
include $($(1)_MAKEFILE)
endif

# Make a list of the object files that will be used to build the static library
$(eval $(1)_LIB_OBJS := $(addprefix $(strip $(OUTPUT_DIR)/Modules/$(call GET_BARE_LOCATION,$(1))),  $(filter %.o, $($(1)_SOURCES:.cc=.o) $($(1)_SOURCES:.cpp=.o) $($(1)_SOURCES:.c=.o) $($(1)_SOURCES:.s=.o) $($(1)_SOURCES:.S=.o)))  $(patsubst %.c,%.o,$(call RESOURCE_FILENAME, $($(1)_RESOURCES))))


$(LIBS_DIR)/$(1).c_opts: $($(1)_PRE_BUILD_TARGETS) $(CONFIG_FILE) | $(LIBS_DIR)
	$(eval $(1)_C_OPTS:=$(subst $(COMMA),$$(COMMA), $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(COMPILER_UNI_CFLAGS) $($(1)_CFLAGS) $($(1)_INCLUDES) $($(1)_DEFINES) $(AOS_SDK_INCLUDES) $(AOS_SDK_DEFINES)))
	$(eval C_OPTS_FILE := $($(1)_C_OPTS) )
	$$(call WRITE_FILE_CREATE, $$@, $(C_OPTS_FILE))
	$$(file >$$@, $(C_OPTS_FILE) )

$(LIBS_DIR)/$(1).cpp_opts: $($(1)_PRE_BUILD_TARGETS) $(CONFIG_FILE) | $(LIBS_DIR)
	$(eval $(1)_CPP_OPTS:=$(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $($(1)_CXXFLAGS)  $($(1)_INCLUDES) $($(1)_DEFINES) $(AOS_SDK_INCLUDES) $(AOS_SDK_DEFINES))
	$$(file >$$@, $($(1)_CPP_OPTS) )

$(LIBS_DIR)/$(1).as_opts: $(CONFIG_FILE) | $(LIBS_DIR)
	$(eval $(1)_S_OPTS:=$(CPU_ASMFLAGS) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_UNI_SFLAGS) $($(1)_ASMFLAGS) $($(1)_INCLUDES) $(AOS_SDK_INCLUDES))
	$(eval S_OPTS_FILE := $($(1)_S_OPTS) )
	$$(file >$$@, $(S_OPTS_FILE) )

$(LIBS_DIR)/$(1).ar_opts: $(CONFIG_FILE) | $(LIBS_DIR)
	$(QUIET)$$(call WRITE_FILE_CREATE, $$@ ,$($(1)_LIB_OBJS))


# Allow checking of completeness of headers
$(foreach src, $(if $(findstring 1,$(CHECK_HEADERS)), $(filter %.h, $($(1)_CHECK_HEADERS)), ),$(eval $(call CHECK_HEADER_RULE,$(1),$(src))))

# Target for build-from-source
$(LIBS_DIR)/$(1).a: $$($(1)_LIB_OBJS) $($(1)_CHECK_HEADER_LIST) $(LIBS_DIR)/$(1).ar_opts
	$(ECHO) Making $$@
	$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_CREATE) $$@ $(OPTIONS_IN_FILE_OPTION_PREFIX)$(OPTIONS_IN_FILE_OPTION)$(LIBS_DIR)/$(1).ar_opts$(OPTIONS_IN_FILE_OPTION_SUFFIX)
ifeq ($(1),sensor)
ifeq ($(COMPILER), armcc)
	$(QUIET)$(AR) --zs $(OUTPUT_DIR)/libraries/sensor.a > $(OUTPUT_DIR)/Modules/drivers/sensor/sensor.sym
else ifeq ($(COMPILER), rvct)
	$(QUIET)$(AR) --zs $(OUTPUT_DIR)/libraries/sensor.a > $(OUTPUT_DIR)/Modules/drivers/sensor/sensor.sym
else ifeq ($(COMPILER), iar)
	$(QUIET)$(AR) --symbols $(OUTPUT_DIR)/libraries/sensor.a > $(OUTPUT_DIR)/Modules/drivers/sensor/sensor.sym
else
	$(QUIET)$(OBJDUMP) -t -w $(OUTPUT_DIR)/libraries/sensor.a > $(OUTPUT_DIR)/Modules/drivers/sensor/sensor.sym
endif
	@python $(SCRIPTS_PATH)/gen_sensor_cb.py tool_$(COMPILER) $(OUTPUT_DIR)/Modules/drivers/sensor/sensor.sym $(OUTPUT_DIR)/Modules/drivers/sensor/sensor_config.c

ifeq ($(IDE),iar)
	@python $(SCRIPTS_PATH)/add_sensor_config_mk.py $(SCRIPTS_PATH)/config_mk.py $(OUTPUT_DIR)/Modules/drivers/sensor/sensor_config.c
	@python build/scripts/iar.py $(CLEANED_BUILD_STRING)
else ifeq ($(IDE),keil)
	@python $(SCRIPTS_PATH)/add_sensor_config_mk.py $(SCRIPTS_PATH)/config_mk.py $(OUTPUT_DIR)/Modules/drivers/sensor/sensor_config.c
	@python build/scripts/keil.py $(CLEANED_BUILD_STRING)
endif

	$(CC) $(sensor_C_OPTS) $(OUTPUT_DIR)/Modules/drivers/sensor/sensor_config.c -o $(OUTPUT_DIR)/Modules/drivers/sensor/sensor_config.o
	$(QUIET)rm -rf $(OUTPUT_DIR)/libraries/sensor.a
	$(QUIET)$(AR) $(AOS_SDK_ARFLAGS) $(COMPILER_SPECIFIC_ARFLAGS_CREATE) $$@ $(OUTPUT_DIR)/Modules/drivers/sensor/sensor_config.o $(OPTIONS_IN_FILE_OPTION_PREFIX)$(OPTIONS_IN_FILE_OPTION)$(OUTPUT_DIR)/libraries/$(1).ar_opts$(OPTIONS_IN_FILE_OPTION_SUFFIX)
endif
ifeq ($(COMPILER),)
	$(QUIET)$(STRIP) -g -o $(OUTPUT_DIR)/libraries/$(1).stripped.a $(OUTPUT_DIR)/libraries/$(1).a
endif
# Create targets to built the component's source files into object files
$(foreach src, $(filter %.c, $($(1)_SOURCES)),$(eval $(call BUILD_C_RULE,$(1),$(src))))
$(foreach src, $(filter %.cpp, $($(1)_SOURCES)) $(filter %.cc, $($(1)_SOURCES)),$(eval $(call BUILD_CPP_RULE,$(1),$(src))))
$(foreach src, $(filter %.s %.S, $($(1)_SOURCES)),$(eval $(call BUILD_S_RULE,$(1),$(src))))

$(eval $(1)_LINT_FLAGS +=  $(filter -D% -I%, $($(1)_CFLAGS) $($(1)_INCLUDES) $($(1)_DEFINES) $(AOS_SDK_INCLUDES) $(AOS_SDK_DEFINES) ) )
$(eval LINT_FLAGS +=  $($(1)_LINT_FLAGS) )
$(eval LINT_FILES +=  $(addprefix $(strip $($(1)_LOCATION)), $(filter %.c, $($(1)_SOURCES))) )
endef

###############################################################################
# MACRO: PROCESS_LDS_FILE
# $(1) is ld file
define PROCESS_LDS_FILE
$(LDS_FILE_DIR)/$(notdir $(1:.ld.S=.ld)): $(LDS_FILE_DIR)
	$(ECHO) Making $$@
	$(QUIET)$(CPP) -P $(AOS_SDK_CFLAGS) $(AOS_SDK_INCLUDES) $(AOS_SDK_DEFINES) $(1) -o $$@

$(eval LDS_FILES += $(LDS_FILE_DIR)/$(notdir $(1:.ld.S=.ld)))
endef

##################################
# Processing
##################################

# $(info Components: $(COMPONENTS))
# Create targets for components
ifeq (app, $(MBINS))
IMAGE_MAGIC:=0xabababab
$(foreach comp,$(COMPONENTS),$(eval $(if $($(comp)_MBINS_TYPE), $(if $(filter app share, $($(comp)_MBINS_TYPE)), $(call BUILD_COMPONENT_RULES,$(comp))), $(call BUILD_COMPONENT_RULES,$(comp)))))
else ifeq (kernel, $(MBINS))
IMAGE_MAGIC:=0xcdcdcdcd
$(foreach comp,$(COMPONENTS),$(eval $(if $(filter kernel share, $($(comp)_MBINS_TYPE)), $(call BUILD_COMPONENT_RULES,$(comp)))))
else ifeq (,$(MBINS))
IMAGE_MAGIC:=0xefefefef
$(foreach comp,$(COMPONENTS),$(eval $(call BUILD_COMPONENT_RULES,$(comp))))
endif

# handle lds file, lds -> ld
$(foreach ldsfile,$(AOS_SDK_LDS_FILES),$(eval $(call PROCESS_LDS_FILE,$(ldsfile))))
$(foreach ldsfile,$(AOS_SDK_LDS_INCLUDES),$(eval $(call PROCESS_LDS_FILE,$(ldsfile))))
$(foreach ldsfile,$(AOS_SDK_LDS_FILES),$(eval AOS_SDK_LDFLAGS += -T $(notdir $(ldsfile:.ld.S=.ld))))
$(if $(AOS_SDK_LDS_FILES),$(eval AOS_SDK_LDFLAGS += -L $(LDS_FILE_DIR)))

# Add pre-built libraries
LINK_LIBS += $(AOS_SDK_PREBUILT_LIBRARIES)

##################################
# Build rules
##################################

$(LIBS_DIR):
	$(QUIET)$(call MKDIR, $@)

$(LDS_FILE_DIR):
	$(QUIET)$(call MKDIR, $@)

# Directory dependency - causes mkdir to be called once for each directory.
%/.d:
	$(QUIET)$(call MKDIR, $(dir $@))
	$(QUIET)$(TOUCH) $(@)

%/.i:
	$(QUIET)$(call MKDIR, $(dir $@))

LINK_OPTS := $(AOS_SDK_LINK_SCRIPT_CMD) $(call COMPILER_SPECIFIC_LINK_MAP,$(MAP_OUTPUT_FILE))  $(call COMPILER_SPECIFIC_LINK_FILES, $(AOS_SDK_LINK_FILES) $(filter %.a,$^) $(LINK_LIBS)) $(AOS_SDK_LDFLAGS)
ifeq ($(PING_PONG_OTA),1)
LINK_OPTS_XIP2 := $(AOS_SDK_LINK_SCRIPT_CMD) $(call COMPILER_SPECIFIC_LINK_MAP,$(MAP_OUTPUT_FILE_XIP2))  $(call COMPILER_SPECIFIC_LINK_FILES, $(AOS_SDK_LINK_FILES) $(filter %.a,$^) $(LINK_LIBS)) $(AOS_SDK_LDFLAGS)
endif

# FIXME GCC Whole archive not ready in all platform
$(LINK_OPTS_FILE): $(OUTPUT_DIR)/config.mk $(LDS_FILES)
ifeq ($(COMPILER),armcc)
	$(QUIET)$(call WRITE_FILE_CREATE, $@ ,$(LINK_OPTS))
else
	$(QUIET)$(call WRITE_FILE_CREATE, $@ ,$(LINK_OPTS) )
endif

$(LINT_OPTS_FILE): $(LINK_LIBS)
	$(QUIET)$(call WRITE_FILE_CREATE, $@ , )
	$(QUIET)$(foreach opt,$(sort $(subst \",",$(LINT_FLAGS))) $(sort $(LINT_FILES)),$(call WRITE_FILE_APPEND, $@ ,$(opt)))

ifeq ($(PING_PONG_OTA),1)
$(LINK_OUTPUT_FILE): $(LINK_LIBS) $(AOS_SDK_LINK_SCRIPT) $(LINK_OPTS_FILE) $(LINT_DEPENDENCY) | $(EXTRA_PRE_LINK_TARGETS)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(LINKER) $(LINK_OPTS) $(COMPILER_SPECIFIC_STDOUT_REDIRECT) -T $(AOS_SDK_IMG1_XIP1_LD_FILE) -o $@
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(call COMPILER_SPECIFIC_MAPFILE_TO_CSV,$(MAP_OUTPUT_FILE),$(MAP_CSV_OUTPUT_FILE))

$(LINK_OUTPUT_FILE_XIP2): $(LINK_LIBS) $(AOS_SDK_LINK_SCRIPT) $(LINK_OPTS_FILE) $(LINT_DEPENDENCY) | $(EXTRA_PRE_LINK_TARGETS)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(LINKER) $(LINK_OPTS_XIP2) $(COMPILER_SPECIFIC_STDOUT_REDIRECT) -T $(AOS_SDK_IMG2_XIP2_LD_FILE) -o $@
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(call COMPILER_SPECIFIC_MAPFILE_TO_CSV,$(MAP_OUTPUT_FILE_XIP2),$(MAP_CSV_OUTPUT_FILE_XIP2))
else
$(LINK_OUTPUT_FILE): $(LINK_LIBS) $(AOS_SDK_LINK_SCRIPT) $(LINK_OPTS_FILE) $(LINT_DEPENDENCY) | $(EXTRA_PRE_LINK_TARGETS)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(LINKER) $(LINK_OPTS) $(COMPILER_SPECIFIC_STDOUT_REDIRECT) -o $@
	$(QUIET)$(ECHO_BLANK_LINE)
	$(QUIET)$(call COMPILER_SPECIFIC_MAPFILE_TO_CSV,$(MAP_OUTPUT_FILE),$(MAP_CSV_OUTPUT_FILE))
endif

# Stripped elf file target - Strips the full elf file and outputs to a new .stripped.elf file
$(STRIPPED_LINK_OUTPUT_FILE): $(LINK_OUTPUT_FILE)
ifeq ($(COMPILER),iar)
	$(QUIET)$(STRIP) $(STRIPFLAGS) $< $(STRIP_OUTPUT_PREFIX)$@
else
	$(QUIET)$(STRIP) $(STRIP_OUTPUT_PREFIX)$@ $(STRIPFLAGS) $<
endif

# Bin file target - uses objcopy to convert the stripped elf into a binary file
$(BIN_OUTPUT_FILE_TMP): $(STRIPPED_LINK_OUTPUT_FILE)
	$(ECHO) Making $(notdir $(BIN_OUTPUT_FILE))
	$(QUIET)$(RM) $(BIN_OUTPUT_FILE)
	$(QUIET)$(OBJCOPY) $(OBJCOPY_BIN_FLAGS) $< $(OBJCOPY_OUTPUT_PREFIX)$(BIN_OUTPUT_FILE)

ifeq ($(PING_PONG_OTA),1)
$(STRIPPED_LINK_OUTPUT_FILE_XIP2): $(LINK_OUTPUT_FILE_XIP2)
	$(QUIET)$(STRIP) $(STRIP_OUTPUT_PREFIX)$@ $(STRIPFLAGS) $<

$(BIN_OUTPUT_FILE_XIP2): $(STRIPPED_LINK_OUTPUT_FILE_XIP2)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(OBJCOPY) $(OBJCOPY_BIN_FLAGS) $< $(OBJCOPY_OUTPUT_PREFIX)$@
endif

$(HEX_OUTPUT_FILE): $(STRIPPED_LINK_OUTPUT_FILE)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(OBJCOPY) $(OBJCOPY_HEX_FLAGS) $< $(OBJCOPY_OUTPUT_PREFIX)$@

ifeq ($(PING_PONG_OTA),1)
$(HEX_OUTPUT_FILE_XIP2): $(STRIPPED_LINK_OUTPUT_FILE_XIP2)
	$(QUIET)$(ECHO) Making $(notdir $@)
	$(QUIET)$(OBJCOPY) $(OBJCOPY_HEX_FLAGS) $< $(OBJCOPY_OUTPUT_PREFIX)$@
endif

# Linker output target - This links all component & resource libraries and objects into an output executable
# CXX is used for compatibility with C++
#$(AOS_SDK_CONVERTER_OUTPUT_FILE): $(LINK_OUTPUT_FILE)
#	$(QUIET)$(ECHO) Making $(notdir $@)
#	$(QUIET)$(CONVERTER) "--ihex" "--verbose" $(LINK_OUTPUT_FILE) $@

#$(AOS_SDK_FINAL_OUTPUT_FILE): $(AOS_SDK_CONVERTER_OUTPUT_FILE)
#	$(QUIET)$(ECHO) Making $(PYTHON_FULL_NAME) $(AOS_SDK_CHIP_SPECIFIC_SCRIPT) -i $(AOS_SDK_CONVERTER_OUTPUT_FILE) -o $(AOS_SDK_FINAL_OUTPUT_FILE)
#	$(QUIET)$(PYTHON_FULL_NAME) $(AOS_SDK_CHIP_SPECIFIC_SCRIPT) -i $(AOS_SDK_CONVERTER_OUTPUT_FILE) -o $(AOS_SDK_FINAL_OUTPUT_FILE)


ifeq ($(PING_PONG_OTA),1)
$(LINK_OUTPUT_FILE_XIP2): $(LINK_OUTPUT_FILE)
display_map_summary_XIP2: $(LINK_OUTPUT_FILE_XIP2) $(AOS_SDK_CONVERTER_OUTPUT_FILE) $(AOS_SDK_FINAL_OUTPUT_FILE)
	$(QUIET) $(call COMPILER_SPECIFIC_MAPFILE_DISPLAY_SUMMARY,$(MAP_OUTPUT_FILE_XIP2))
endif

display_map_summary: $(LINK_OUTPUT_FILE) $(AOS_SDK_CONVERTER_OUTPUT_FILE) $(AOS_SDK_FINAL_OUTPUT_FILE)
	$(QUIET) $(call COMPILER_SPECIFIC_MAPFILE_DISPLAY_SUMMARY,$(MAP_OUTPUT_FILE))

# Main Target - Ensures the required parts get built
# $(info Prebuild targets:$(EXTRA_PRE_BUILD_TARGETS))
# $(info $(BIN_OUTPUT_FILE))
ifeq ($(PING_PONG_OTA),1)
$(BIN_OUTPUT_FILE_XIP2): $(BIN_OUTPUT_FILE_TMP)
build_done: $(EXTRA_PRE_BUILD_TARGETS) $(BIN_OUTPUT_FILE_TMP) $(HEX_OUTPUT_FILE) display_map_summary
build_done: $(EXTRA_PRE_BUILD_TARGETS) $(BIN_OUTPUT_FILE_XIP2) $(HEX_OUTPUT_FILE_XIP2) display_map_summary_XIP2
else
build_done: $(EXTRA_PRE_BUILD_TARGETS) $(BIN_OUTPUT_FILE_TMP) $(HEX_OUTPUT_FILE) display_map_summary
endif

$(EXTRA_POST_BUILD_TARGETS): build_done

$(BUILD_STRING): $(if $(EXTRA_POST_BUILD_TARGETS),$(EXTRA_POST_BUILD_TARGETS),build_done)
	$(PYTHON) $(SCRIPTS_PATH)/ota_gen_md5_bin.py $(BIN_OUTPUT_FILE)

%.compile: $(LINK_LIBS)
	$(QUIET)$(ECHO) Build libraries complete
