#
# Export project for 3rd IDEs
#

CONFIG_FILE := $(OUTPUT_DIR)/config.mk
CONFIG_PY_FILE := $(SCRIPTS_PATH)/config_mk.py

IDE_IAR_FLAG :=
IDE_KEIL_FLAG :=

ifeq ($(IDE),iar)
IDE_IAR_FLAG := 1
else ifeq ($(IDE),keil)
IDE_KEIL_FLAG := 1
endif

###############################################################################
# MACRO: WRITE_COMPOENT_PY
# $(1): Component name
define WRITE_COMPOENT_PY

$(eval comp := $(1))
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,{'name':'$(comp)'$(COMMA) )
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'src':[ )
$(eval SOURCES_FULLPATH := $(addprefix $($(comp)_LOCATION), $($(comp)_SOURCES)))
$(foreach src,$(SOURCES_FULLPATH), $(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'$(src)'$(COMMA)))
$(eval LIB_FULLPATH := $(addprefix $($(comp)_LOCATION), $($(comp)_PREBUILT_LIBRARY)))
$(foreach complib,$(LIB_FULLPATH), $(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'$(complib)'$(COMMA)))
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,]$(COMMA))
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'include':[ )
$(eval INCLUDE_FULLPATH := $(addprefix $($(comp)_LOCATION),$($(comp)_INCLUDES)) )
$(eval INCLUDE_FULLPATH += $(subst -I.,.,$(call unique,$(AOS_SDK_INCLUDES))) )
$(foreach inc,$(INCLUDE_FULLPATH), $(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'$(inc)'$(COMMA)))
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,]$(COMMA))
$(call PROCESS_COMPONENT_OPTS, $(comp))
$(if $(IDE_IAR_FLAG),$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'c_opts_iar': '$(C_OPTS_IAR)'$(COMMA)),)
$(if $(IDE_IAR_FLAG),$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'as_opts_iar': '$(AS_OPTS_IAR)'$(COMMA)),)
$(if $(IDE_KEIL_FLAG),$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'c_opts_keil': '$(C_OPTS_KEIL)'$(COMMA)),)
$(if $(IDE_KEIL_FLAG),$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,'as_opts_keil': '$(AS_OPTS_KEIL)'$(COMMA)),)
$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,}$(COMMA))

endef

###############################################################################
# MACRO: PROCESS_COMPONENT_OPTS
# $(1): Component name
# TODO:
#   Get all the options from config.mk instead of aos_target_config.mk runtime
define PROCESS_COMPONENT_OPTS

$(eval AS_OPTS_IAR :=)
$(eval AS_OPTS_KEIL :=)

$(eval C_OPTS := $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_SPECIFIC_DEPS_FLAG) $(COMPILER_UNI_CFLAGS) $(AOS_SDK_CFLAGS) $($(1)_CFLAGS_ALL) $(EXTRA_CFLAGS) $(addprefix -I$($(1)_LOCATION),$($(1)_INCLUDES)) $(addprefix -D,$($(1)_DEFINES)) $(strip $(call unique,$(AOS_SDK_INCLUDES))) $(strip $(call unique,$(addprefix -D,$(AOS_SDK_DEFINES)))))
$(eval C_OPTS_IAR := $(subst =\",="\",$(C_OPTS) ) )
$(eval C_OPTS_IAR := $(subst \" ,\"" ,$(C_OPTS_IAR) ) )
$(eval C_OPTS_IAR := $(filter-out -I% --cpu=% --endian% --dlib_config%,$(C_OPTS_IAR)) )
$(eval C_OPTS_KEIL := $(subst -I.,-I../../../../.,$(C_OPTS)) )

$(if $(strip $(findstring arch_,$(1)) $(findstring board_,$(1))),
$(eval AS_OPTS = $(CPU_ASMFLAGS) $(COMPILER_SPECIFIC_COMP_ONLY_FLAG) $(COMPILER_UNI_SFLAGS) $(AOS_SDK_ASMFLAGS) $($(1)_ASMFLAGS) $(addprefix -I$($(1)_LOCATION),$($(1)_INCLUDES)) $(strip $(call unique,$(AOS_SDK_INCLUDES)))) \
$(eval AS_OPTS_IAR := $(filter-out --cpu Cortex-M4, $(AS_OPTS) ) ) \
$(eval AS_OPTS_KEIL := $(subst -I.,-I../../../../., $(AS_OPTS) ) ),)

endef

# Generate IAR project
ifeq ($(IDE),iar)
PROJ_GEN_DIR := projects/IAR/$(CLEANED_BUILD_STRING)
PROJECT_GEN := $(PROJ_GEN_DIR)/iar_project/$(CLEANED_BUILD_STRING).ewp
$(MAKECMDGOALS): $(PROJECT_GEN)
$(PROJECT_GEN): $(SCRIPTS_PATH)/iar.py $(MAKEFILES_PATH)/aos_target_config.mk $(CONFIG_FILE)
	$(QUIET)echo Making $(IDE) Project
	$(QUIET)$(call WRITE_FILE_CREATE, $(CONFIG_PY_FILE) ,Projects = [)
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_COMPOENT_PY, $(comp) ))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,])
	$(QUIET)$(call MKDIR, $(PROJ_GEN_DIR)/iar_project)
	$(QUIET)cp -f  build/scripts/template.ewd $(PROJ_GEN_DIR)/iar_project/$(CLEANED_BUILD_STRING).ewd
	python build/scripts/iar.py $(CLEANED_BUILD_STRING)
	$(QUIET)echo ----------- iar project has generated in $(PROJ_GEN_DIR)/iar_project -----------
endif

# Generate Keil project
ifeq ($(IDE),keil)
PROJ_GEN_DIR := projects/Keil/$(CLEANED_BUILD_STRING)
PROJECT_GEN := $(PROJ_GEN_DIR)/keil_project/$(CLEANED_BUILD_STRING).uvprojx
$(MAKECMDGOALS): $(PROJECT_GEN)
$(PROJECT_GEN): $(SCRIPTS_PATH)/keil.py $(MAKEFILES_PATH)/aos_target_config.mk $(CONFIG_FILE)
	$(QUIET)echo Making $(IDE) Project
	$(QUIET)$(call WRITE_FILE_CREATE, $(CONFIG_PY_FILE) ,Projects = [)
	$(QUIET)$(foreach comp,$(PROCESSED_COMPONENTS), $(call WRITE_COMPOENT_PY, $(comp) ))
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,])
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,keil_device = "$(strip $(foreach comp,$(PROCESSED_COMPONENTS),$(if $($(comp)_KEIL_DEVICE),$($(comp)_KEIL_DEVICE),)))")
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,keil_vendor = "$(strip $(foreach comp,$(PROCESSED_COMPONENTS),$(if $($(comp)_KEIL_VENDOR),$($(comp)_KEIL_VENDOR),)))")
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,global_cflags = "$(strip $(AOS_SDK_CFLAGS))")
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,global_ldflags = "$(strip $(AOS_SDK_LDFLAGS))")
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,global_includes = "$(strip $(AOS_SDK_INCLUDES))")
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,global_defines = "$(strip $(AOS_SDK_DEFINES))")
	$(QUIET)$(call WRITE_FILE_APPEND, $(CONFIG_PY_FILE) ,host_arch = "$(strip $(HOST_ARCH))")
	$(QUIET)$(call MKDIR, $(PROJ_GEN_DIR)/keil_project)
	python build/scripts/keil.py $(CLEANED_BUILD_STRING)
	$(QUIET)echo ----------- keil project has generated in $(PROJ_GEN_DIR)/keil_project -----------
endif
