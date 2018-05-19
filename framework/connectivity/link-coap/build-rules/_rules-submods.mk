SUB_LOG_OPTION := $(if $(Q),,| tee -a $(OUTPUT_DIR)/$${i}/$(COMPILE_LOG))
ALL_LOG_OPTION := $(if $(Q),,| tee -a $(COMPILE_LOG))

sub-mods: toolchain
	$(Q) \
	if [ -f $(STAMP_PRJ_CFG) ]; then true; else \
	    set -o pipefail; \
	    for i in \
	        $$(echo $(IMPORT_DIR)|sed 's:$(TOP_DIR)/*::g')/$(CONFIG_VENDOR)/platform \
	        $(SUBDIRS); do \
	            if [ ! -d $${i} ]; then continue; fi; \
	            $(MAKE) --no-print-directory Q=$(Q) $${i} 2>&1 $(SUB_LOG_OPTION); \
	            RETVAL=$$?; \
	            if [ $${RETVAL} != 0 ]; then exit $${RETVAL}; fi; \
	    done 2>&1 $(ALL_LOG_OPTION); \
	fi

SUB_BUILD_VARS := \
    CFLAGS LDFLAGS \
    PACKAGE_DIR \
    IMPORT_DIR \
    TOP_DIR \
    RULE_DIR \
    CONFIG_VENDOR \
    COMP_LIB \
    $(CROSS_CANDIDATES) \
    $(MAKE_ENV_VARS) \
    INSTALL_DIR \
    INSTALL_LIB_DIR \
    SYSROOT_INC \
    KERNEL_DIR \
    MAKE_ENV_VARS \
    CROSS_PREFIX \
    CROSS_CANDIDATES \
    ALL_SUB_DIRS \

CMDLINE_VARS := \
    HD_MAKEFILE \
    MAKE_SEGMENT \
    OUTPUT_DIR \
    PACKAGE_DIR \
    STAMP_BLD_ENV \
    STAMP_UNPACK \
    TOP_DIR \
    RULE_DIR \

# When SUB_BUILD_VARS like $(CFLAGS) contains special character '$'
# simply echo its value into 'Makefile' will cause '$' lost when GNU make read in again
#
$(STAMP_BLD_ENV): $(TOP_DIR)/makefile $(shell ls $(CONFIG_TPL) 2>/dev/null) \
                  $(wildcard $(RULE_DIR)/*.mk) \
                  $(shell grep "^ *include" $(TOP_DIR)/$(TOP_MAKEFILE)|awk '{ print $$NF }'|sed '/^\$$/d')
	@rm -f $@
	@$(foreach V, \
	    $(sort $(SUB_BUILD_VARS)), \
	        echo "$(V) := $(sort $($(V)))"|sed 's:\$$:$$$$:g' >> $(STAMP_BLD_ENV); \
	)

# note:
#   sed -i "/CONFIG_$${i//\//\\/}.*/d" $(CONFIG_TPL);
# above
#   sed -i "1iCONFIG_$${i} = y" $(CONFIG_TPL)
# was removed since modules will be skipped in some cases

$(STAMP_BLD_VAR): $(foreach d,$(ALL_SUB_DIRS),$(d)/$(MAKE_SEGMENT)) $(STAMP_BLD_ENV) $(wildcard $(RULE_DIR)/*.mk)
	$(TOP_Q) \
( \
	if [ ! -f $(STAMP_BLD_VAR) ]; then echo ""; VERBOSE=1; fi; \
	rm -f $(STAMP_BLD_VAR); \
	for i in $(shell echo "$(ALL_SUB_DIRS)"|tr ' ' '\n'|sort -u); do \
	    if [ "$${VERBOSE}" != "" ]; then \
	        if [ ! -L $${i} ]; then \
	            printf "CONFIGURE .............................. [%s]\n" $${i}; \
	        fi; \
	        sed -i "1iCONFIG_$${i} = y" $(CONFIG_TPL); \
	        [ -f $(STAMP_POST_RULE) ] && sed -i "/target-$${i//\//\\/}.*/d" $(STAMP_POST_RULE) || true; \
	        echo "target-$${i}:; @true" >> $(STAMP_POST_RULE); \
	    fi; \
	    $(foreach V, $(CMDLINE_VARS), $(V)="$($(V))") \
	        bash $(RULE_DIR)/pre-build.sh $${i} makefile-only > /dev/null; \
	    if [ -d $(OUTPUT_DIR)/$${i} ]; then \
	        $(MAKE) -s -C $(OUTPUT_DIR)/$${i} modinfo > /dev/null; \
	        if [ $$? = 0 ]; then \
	            $(MAKE) --no-print-directory -s -C $(OUTPUT_DIR)/$${i} modinfo >> $(STAMP_BLD_VAR); \
	        else \
	            echo ""; \
	            echo "ERROR detected in '$${i}/$(MAKE_SEGMENT)'..."|grep --color '.*'; \
	            echo ""; \
	            rm -f $(STAMP_BLD_VAR) $(STAMP_PRJ_CFG); \
	            exit 13; \
	        fi \
	    fi \
	done; \
	sort -o $(STAMP_BLD_VAR) $(STAMP_BLD_VAR); \
	if [ "$${VERBOSE}" != "" ]; then echo ""; fi; \
)

pre-build: MOD = $(subst target-,,$(filter-out $@,$(MAKECMDGOALS)))
pre-build: $(STAMP_BLD_ENV)
ifeq (0,$(MAKELEVEL))
	$(TOP_Q)rm -vf $(OUTPUT_DIR)/$(MOD)/$(STAMP_UNPACK)
endif
	$(if $(filter 0,$(MAKELEVEL)),,@) \
	$(strip $(foreach V, $(CMDLINE_VARS), $(V)="$($(V))") \
	    PKG_SOURCE="$(PKG_SOURCE_$(MOD))" \
	    PKG_BRANCH="$(PKG_BRANCH_$(MOD))" \
	    PKG_REVISION="$(PKG_REVISION_$(MOD))" \
	    PKG_UPSTREAM="$(PKG_UPSTREAM_$(MOD))" \
	    PKG_SWITCH="$(PKG_SWITCH_$(MOD))" \
	) \
	$(if $(filter 0,$(MAKELEVEL)),VERBOSE_PRE_BLD=1) \
	    bash $(RULE_DIR)/pre-build.sh $(subst target-,,$(filter-out $@,$(MAKECMDGOALS)))

.PHONY: $(ALL_SUB_DIRS)

$(ALL_SUB_DIRS): ALL_LOG_OPT = $(if $(Q),,2>&1|tee -a $(OUTPUT_DIR)/$(COMPILE_LOG))
$(ALL_SUB_DIRS): SUB_LOG_OPT = $(if $(Q),,2>&1|tee -a $(OUTPUT_DIR)/$@/$(COMPILE_LOG))

$(ALL_SUB_DIRS): $(if $(filter 0,$(MAKELEVEL)),toolchain) $(STAMP_BLD_VAR)
	$(TOP_Q)rm -f $(STAMP_PRJ_CFG)
	$(TOP_Q)$(MAKE) --no-print-directory pre-build target-$@
ifeq (0,$(MAKELEVEL))
	$(TOP_Q)$(MAKE) --no-print-directory -C $(OUTPUT_DIR)/$@ clean
endif
	$(TOP_Q) \
	if [ "$$(echo $(PKG_SWITCH_$@))" != "" ]; then \
	    if [ "$$(echo $(DEPENDS_$@))" != "" ]; then \
	        if  [ ! -f $(IMPORT_VDRDIR)/$(PREBUILT_LIBDIR)/lib$(shell basename $@).a ] && \
	            [ ! -f $(IMPORT_VDRDIR)/$(PREBUILT_LIBDIR)/lib$(shell basename $@).so ]; then \
	            for i in $(DEPENDS_$@); do \
	                $(MAKE) --no-print-directory $${i} \
	                    $(if $(Q),,2>&1|tee -a $(OUTPUT_DIR)/$${i}/$(COMPILE_LOG)) \
	                    $(ALL_LOG_OPT); \
	                RETVAL=$$?; \
	                if [ $${RETVAL} != 0 ]; then exit $${RETVAL}; fi; \
	            done \
	        fi \
	    fi && \
	    $(call Build_CompLib, $@) && \
	    $(call Update_Extra_Srcs, $(EXTRA_SRCS_$@),$@) && \
	    $(MAKE) --no-print-directory -C $(OUTPUT_DIR)/$@ all $(SUB_LOG_OPT) $(ALL_LOG_OPT) && \
	    if [ "$$(echo $(ORIGIN_$@))" != "" ]; then \
	        touch $(OUTPUT_DIR)/$@/{$(STAMP_UNPACK),$(STAMP_CONFIG),$(STAMP_BUILD),$(STAMP_INSTALL)}; \
	    fi \
	else \
	    echo -ne "\r$$(printf '%40s' '')\r"; \
	fi

