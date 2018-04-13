.PHONY: doc detect config reconfig toolchain sub-mods final-out env help

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
	@echo -e "\e[1;37m[$(RULE_DIR)/docs]\e[0m"
	@echo ""
	@cat $(RULE_DIR)/docs/Help.md
	@echo ""

doc:
	$(TOP_Q)rm -rf html
	$(TOP_Q) \
	sed \
	    's:^PROJECT_NAME.*:PROJECT_NAME = $(PRJ_NAME):g; s:^PROJECT_NUMBER.*:PROJECT_NUMBER = $(PRJ_VERSION):g' \
	build-rules/misc/Doxyfile.tpl > $(OUTPUT_DIR)/.doxygen.cfg
	$(TOP_Q)doxygen $(OUTPUT_DIR)/.doxygen.cfg

detect:
	@if [ -d .git ]; then \
	    mkdir -p .git/hooks; \
	    for i in $(RULE_DIR)/hooks/*; do \
	        cp -f $$i .git/hooks && chmod a+x .git/hooks/$$(basename $$i); \
	    done; \
	fi

unzip: config $(STAMP_BLD_VAR)
	@echo "Components: "
	@echo ""
	@for i in $(ALL_SUB_DIRS); do \
	    $(MAKE) --no-print-directory pre-build target-$${i} > /dev/null; \
	    echo ". $${i}"; \
	done
	@echo ""

#	@for i in $$(grep "^ *include" $(TOP_DIR)/$(TOP_MAKEFILE)|awk '{ print $$NF }'|sed '/^\$$/d'); do \
#	    if [ $$i -nt $(CONFIG_TPL) ]; then \
#	        echo "Re-configure project since '$${i}' updated"|grep --color ".*"; \
#	        $(RECURSIVE_MAKE) reconfig; \
#	    fi; \
#	done
#
#	@if [ ! -d $(OUTPUT_DIR) ]; then \
#	    echo "Re-configure project since '$(OUTPUT_DIR)' non-exist!"|grep --color ".*"; \
#	    $(RECURSIVE_MAKE) reconfig; \
#	fi

config:

	@mkdir -p $(OUTPUT_DIR) $(STAMP_DIR) $(INSTALL_DIR)
	@mkdir -p $(SYSROOT_BIN) $(SYSROOT_INC) $(SYSROOT_LIB)

	$(TOP_Q) \
	if [ -f $(STAMP_BLD_VAR) ]; then \
	    if [ "$$(sed '/[-_/a-zA-Z0-9]* = ..*/d' $(STAMP_BLD_VAR)|wc -l)" != "0" ]; then \
	        rm -f $(STAMP_BLD_VAR); \
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
	       ([ "$(DEFAULT_BLD)" = "$(RULE_DIR)/misc/config.generic.default" ] || [ "$(MAKECMDGOALS)" = "" ]); then \
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

toolchain: VSP_TARBALL = $(shell $(SHELL_DBG) basename $(CONFIG_TOOLCHAIN_RPATH))
toolchain: config
ifneq ($(CONFIG_TOOLCHAIN_NAME),)
ifeq (,$(CONFIG_TOOLCHAIN_RPATH))
	@echo "Error! CONFIG_TOOLCHAIN_NAME defined, but CONFIG_TOOLCHAIN_RPATH undefined!" && exit 1
else
	$(TOP_Q) \
( \
	if [ -e $(OUTPUT_DIR)/$(CONFIG_TOOLCHAIN_NAME) ]; then \
	    exit 0; \
	fi; \
\
	if [ ! -d /tmp/$(CONFIG_TOOLCHAIN_NAME) -a -f /tmp/$(VSP_TARBALL) ]; then \
	    echo "De-compressing Cached ToolChain ..." && \
	    tar xf /tmp/$(VSP_TARBALL) -C /tmp; \
	fi; \
	if [ -d /tmp/$(CONFIG_TOOLCHAIN_NAME) ]; then \
	    echo "Using Cached ToolChain ..." && \
	    ln -sf /tmp/$(CONFIG_TOOLCHAIN_NAME) $(OUTPUT_DIR)/$(CONFIG_TOOLCHAIN_NAME); \
	    exit 0; \
	fi; \
\
	echo "Downloading ToolChain ..." && \
	wget -O $(OUTPUT_DIR)/$(VSP_TARBALL) $(CONFIG_VSP_WEBSITE)/$(CONFIG_TOOLCHAIN_RPATH) && \
	echo "De-compressing ToolChain ..." && \
	tar xf $(OUTPUT_DIR)/$(VSP_TARBALL) -C $(OUTPUT_DIR); \
	cp -f $(OUTPUT_DIR)/$(VSP_TARBALL) /tmp; \
	rm -rf /tmp/$(CONFIG_TOOLCHAIN_NAME); \
	tar xf /tmp/$(VSP_TARBALL) -C /tmp; \
)
endif
endif

reconfig: distclean
	$(TOP_Q)+( \
	if [ -d $(CONFIG_DIR) ]; then \
	    $(RECURSIVE_MAKE) config DEFAULT_BLD=not-exist-actually; \
	else \
	    $(RECURSIVE_MAKE) config; \
	fi)
	$(TOP_Q)rm -f $(STAMP_PRJ_CFG)

clean:
	$(TOP_Q) \
	for i in $(SUBDIRS) $(COMP_LIB_COMPONENTS); do \
	    if [ -d $(OUTPUT_DIR)/$${i} ]; then \
	        $(MAKE) --no-print-directory -C $(OUTPUT_DIR)/$${i} clean; \
	    fi \
	done

	$(TOP_Q) \
	rm -rf \
	        $(LIBOBJ_TMPDIR) \
	        $(COMPILE_LOG) \
	        $(DIST_DIR)/* \
	        $(STAMP_DIR) \
	        $(SYSROOT_INC)/* $(SYSROOT_LIB)/* $(SYSROOT_LIB)/../bin/* \
	        $(shell $(SHELL_DBG) find $(OUTPUT_DIR) -name "$(COMPILE_LOG)" \
	                             -o -name "$(WARNING_LOG)" \
	                             -o -name "$(STAMP_BUILD)" \
	                             -o -name "$(STAMP_INSTALL)" \
	                             -o -name "$(STAMP_POSTINS)" \
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

