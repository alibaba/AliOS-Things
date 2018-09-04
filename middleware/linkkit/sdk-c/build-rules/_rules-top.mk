.PHONY: doc detect config reconfig toolchain sub-mods final-out env cmake one help

all: detect config toolchain sub-mods final-out
	$(TOP_Q) \
	if [ -f $(STAMP_PRJ_CFG) ]; then \
	    $(RECURSIVE_MAKE) toolchain; \
	    rm -f $(STAMP_PRJ_CFG); \
	fi
	@rm -rf $(STAMP_DIR)

RESET_ENV_VARS := \
    CROSS_PREFIX \
    CFLAGS \
    HOST \
    LDFLAGS \

help:
	@echo -e "\033[1;37m[$(RULE_DIR)/docs]\e[0m"
	@echo ""
	@cat $(RULE_DIR)/docs/Help.md
	@echo ""

doc:
	$(TOP_Q)rm -rf $(DOXYGEN_DIR)/html; mkdir -p $(DOXYGEN_DIR)
	$(TOP_Q) \
	$(SED) \
	    -e 's:^PROJECT_NAME.*:PROJECT_NAME = $(PRJ_NAME):g;' \
	    -e 's:^PROJECT_NUMBER.*:PROJECT_NUMBER = $(PRJ_VERSION):g;' \
	    -e 's:^OUTPUT_DIRECTORY.*:OUTPUT_DIRECTORY = $(DOXYGEN_DIR):g;' \
	build-rules/misc/Doxyfile.tpl > $(OUTPUT_DIR)/.doxygen.cfg
	$(TOP_Q)doxygen $(OUTPUT_DIR)/.doxygen.cfg

detect:
	@if [ -d .git ]; then \
	    mkdir -p .git/hooks; \
	    for i in $(RULE_DIR)/hooks/*; do \
	        cp -f $$i .git/hooks && chmod a+x .git/hooks/$$(basename $$i); \
	    done; \
	fi

prune:
	@echo "$(TOP_DIR).pkgs directory removed!"|grep --color ".*"
	@rm -rf $(TOP_DIR).pkgs
	@$(MAKE) --no-print-directory distclean

unzip: config $(STAMP_BLD_VAR)
	@echo "Components: "
	@echo ""
	@for i in $(ALL_SUB_DIRS); do \
	    $(MAKE) --no-print-directory pre-build target-$${i} ; \
	    echo -ne "\r. $${i}"; \
	    echo -e "                                          "; \
	done
	@echo ""

cmake:
	@$(MAKE) -s distclean
	@$(MAKE) -s DEFAULT_BLD=$(RULE_DIR)/misc/config.generic.cmake config
	@$(foreach V,$(filter-out CFLAGS,$(INFO_ENV_VARS)),$(V)="$($(V))") \
	    SEP_LIBS="$$(grep -m 1 '^COMP_LIB_FILES' $(STAMP_BLD_ENV) | cut -d' ' -f3-)" \
	    CFLAGS=$(CFLAGS) \
	    bash $(RULE_DIR)/scripts/gen_top_cmake.sh $(TOP_DIR)/CMakeLists.txt
	@for D in $(ALL_SUB_DIRS); do \
	    echo "+ $${D}"; \
	    $(MAKE) --no-print-directory -C $(OUTPUT_DIR)/$${D} cmake; \
	done
	@echo ""

one: COMP_LIB_OBJS = $(foreach V,$(COMP_LIB_COMPONENTS),$(foreach U,$(LIB_OBJS_$(V)),$(V)/$(U)))
one:
	@$(foreach V,$(filter-out CFLAGS,$(INFO_ENV_VARS)),$(V)="$($(V))") \
	    ALL_LIBS="$(strip $(foreach V,$(SUBDIRS),$(LIBA_TARGET_$(V))))" \
	    ALL_PROG="$(strip $(foreach V,$(SUBDIRS),$(TARGET_$(V))))" \
	    COMP_LIB_OBJS="$(foreach V,$(COMP_LIB_OBJS),$(OUTPUT_DIR)/$(V))" \
	    CFLAGS=$(CFLAGS) \
	    bash $(RULE_DIR)/scripts/gen_one_makefile.sh $(STAMP_ONE_MK)

config:

	@mkdir -p $(OUTPUT_DIR) $(STAMP_DIR) $(INSTALL_DIR)
	@mkdir -p $(SYSROOT_BIN) $(SYSROOT_INC) $(SYSROOT_LIB)

	$(TOP_Q) \
	if [ -f $(STAMP_BLD_VAR) ]; then \
	    if [ "$$($(SED) '/[-_/a-zA-Z0-9]* = *..*/d' $(STAMP_BLD_VAR)|wc -l|$(SED) 's:^  *::g')" != "0" ]; then \
	        rm -vf $(STAMP_BLD_VAR); \
	    fi \
	fi

	$(TOP_Q)+( \
	if [ -f $(CONFIG_TPL) ]; then \
	    if [ "$(filter comp-lib,$(MAKECMDGOALS))" = "" ]; then \
	        printf "BUILDING WITH EXISTING CONFIGURATION:\n\n"; \
	        command grep -m 1 "VENDOR *:" $(CONFIG_TPL)|cut -c 3-; \
	        command grep -m 1 "MODEL *:" $(CONFIG_TPL)|cut -c 3-; \
	        echo ""; \
	    fi \
	else \
	    if [ "$(DEFAULT_BLD)" != "" ] && [ -f $(DEFAULT_BLD) ] && \
	       ([ "$(DEFAULT_BLD)" = "$(RULE_DIR)/misc/config.generic.default" ] \
	            || [ "$(MAKECMDGOALS)" = "" ] || [ "$(MAKECMDGOALS)" = "config" ]); then \
	        printf "# Automatically Generated Section End\n\n" >> $(CONFIG_TPL); \
	        printf "# %-10s %s\n" "VENDOR :" $$(basename $(DEFAULT_BLD)|cut -d. -f2) >> $(CONFIG_TPL); \
	        printf "# %-10s %s\n" "MODEL  :" $$(basename $(DEFAULT_BLD)|cut -d. -f3) >> $(CONFIG_TPL); \
	        cat $(DEFAULT_BLD) >> $(CONFIG_TPL); \
	    else \
	        printf "SELECT A CONFIGURATION:\n\n"; \
	        LIST=$$(for i in $(CONFIG_DIR)/config.*.*; do basename $${i}; done|sort); \
	        select V in $${LIST}; do \
	            echo ""; \
	            printf "# Automatically Generated Section End\n\n" >> $(CONFIG_TPL); \
	            printf "# %-10s %s\n" "VENDOR :" $$(echo $${V}|cut -d. -f2) >> $(CONFIG_TPL); \
	            printf "# %-10s %s\n" "MODEL  :" $$(echo $${V}|cut -d. -f3) >> $(CONFIG_TPL); \
	            cp -f -P $(IMPORT_DIR)/$$(echo $${V}|cut -d. -f2)/$(PREBUILT_LIBDIR)/*.so* $(SYSROOT_LIB) 2>/dev/null; \
	            cat $(CONFIG_DIR)/$${V} >> $(CONFIG_TPL); \
	            break; \
	        done; \
	    fi && \
	    printf "SELECTED CONFIGURATION:\n\n" && \
	    command grep -m 1 "VENDOR *:" $(CONFIG_TPL)|cut -c 3- && \
	    command grep -m 1 "MODEL *:" $(CONFIG_TPL)|cut -c 3- && \
	    echo ""; \
	    if [ "$(MAKECMDGOALS)" = "config" ]; then true; else \
	        if [ "$(DEFAULT_BLD)" = "" ]; then \
	            touch $(STAMP_PRJ_CFG); \
	        fi; \
	    fi; \
	    for i in $(RESET_ENV_VARS); do unset $${i}; done; \
	    $(MAKE) --no-print-directory -f $(TOP_MAKEFILE) $(STAMP_BLD_VAR) unzip; \
	fi)

	@$(MAKE) --no-print-directory one

DL_TOOLCHAIN_VARS = \
    TOOLCHAIN_DLDIR \
    OUTPUT_DIR \

toolchain:
	@$(foreach V,$(DL_TOOLCHAIN_VARS),$(V)=$($(V))) \
	    CC=$(shell basename $(CC)) \
	    AR=$(shell basename $(AR)) \
	    RELPATH=$$( $(call Relative_TcPath,$(shell basename $(CC))) ) \
	    GITPATH=$$( $(call Gitrepo_TcPath,$(shell basename $(CC))) ) \
	    bash $(RULE_DIR)/scripts/gen_cross_toolchain.sh

# toolchain: VSP_TARBALL = $(shell $(SHELL_DBG) basename $(CONFIG_TOOLCHAIN_RPATH))
# toolchain: config
# ifneq ($(CONFIG_TOOLCHAIN_NAME),)
# ifeq (,$(CONFIG_TOOLCHAIN_RPATH))
# 	@echo "Error! CONFIG_TOOLCHAIN_NAME defined, but CONFIG_TOOLCHAIN_RPATH undefined!" && exit 1
# else
# 	$(TOP_Q) \
# ( \
# 	if [ -e $(OUTPUT_DIR)/$(CONFIG_TOOLCHAIN_NAME) ]; then \
# 	    exit 0; \
# 	fi; \
# \
# 	if [ ! -d /tmp/$(CONFIG_TOOLCHAIN_NAME) -a -f /tmp/$(VSP_TARBALL) ]; then \
# 	    echo "De-compressing Cached ToolChain ..." && \
# 	    tar xf /tmp/$(VSP_TARBALL) -C /tmp; \
# 	fi; \
# 	if [ -d /tmp/$(CONFIG_TOOLCHAIN_NAME) ]; then \
# 	    echo "Using Cached ToolChain ..." && \
# 	    ln -sf /tmp/$(CONFIG_TOOLCHAIN_NAME) $(OUTPUT_DIR)/$(CONFIG_TOOLCHAIN_NAME); \
# 	    exit 0; \
# 	fi; \
# \
# 	echo "Downloading ToolChain ..." && \
# 	wget -O $(OUTPUT_DIR)/$(VSP_TARBALL) $(CONFIG_VSP_WEBSITE)/$(CONFIG_TOOLCHAIN_RPATH) && \
# 	echo "De-compressing ToolChain ..." && \
# 	tar xf $(OUTPUT_DIR)/$(VSP_TARBALL) -C $(OUTPUT_DIR); \
# 	cp -f $(OUTPUT_DIR)/$(VSP_TARBALL) /tmp; \
# 	rm -rf /tmp/$(CONFIG_TOOLCHAIN_NAME); \
# 	tar xf /tmp/$(VSP_TARBALL) -C /tmp; \
# )
# endif
# endif

reconfig: distclean
	$(TOP_Q)+( \
	if [ -d $(CONFIG_DIR) ]; then \
	    $(RECURSIVE_MAKE) config DEFAULT_BLD=not-exist-actually; \
	else \
	    $(RECURSIVE_MAKE) config; \
	fi)
	$(TOP_Q)rm -f $(STAMP_PRJ_CFG)


#	for i in $(SUBDIRS) $(COMP_LIB_COMPONENTS); do \
#	    if [ -d $(OUTPUT_DIR)/$${i} ]; then \
#	        $(MAKE) --no-print-directory -C $(OUTPUT_DIR)/$${i} clean; \
#	    fi \
#	done

clean:
	$(TOP_Q) \

	$(TOP_Q) \
	rm -rf \
	        $(LIBOBJ_TMPDIR) \
	        $(COMPILE_LOG) \
	        $(DIST_DIR)/* \
	        $(STAMP_DIR) \
	        $(SYSROOT_INC)/* $(SYSROOT_LIB)/* $(SYSROOT_LIB)/../bin/* \
	        $(shell $(SHELL_DBG) find $(OUTPUT_DIR) -name "$(COMPILE_LOG)" \
	                             -or -name "$(WARNING_LOG)" \
	                             -or -name "$(STAMP_BUILD)" \
	                             -or -name "$(STAMP_INSTALL)" \
	                             -or -name "$(STAMP_POSTINS)" \
	                             -or -name "*.so" \
	                             -or -name "*.a" \
	                             -or -name "*.o" \
	                             -or -name "*.d" \
	                             -or -name "*.gc*" \
	        2>/dev/null)

distclean:
	$(TOP_Q) \
	rm -rf \
	    $(CONFIG_TPL) $(COMPILE_LOG) \
	    $(STAMP_PRJ_CFG) $(STAMP_BLD_ENV) $(STAMP_BLD_VAR) $(STAMP_POST_RULE) \
	    $(DIST_DIR) $(STAMP_DIR) \

	$(TOP_Q) \
	if [ -d $(OUTPUT_DIR) ]; then \
	    cd $(OUTPUT_DIR); \
	    if [ "$(CONFIG_TOOLCHAIN_NAME)" = "" ]; then \
	        rm -rf *; \
	    else \
	        rm -rf $$(ls -I $(CONFIG_TOOLCHAIN_NAME)); \
	    fi \
	fi

