define Post_Distro
    @rm -rf $(FINAL_DIR)/include/{LITE*,mbed*}
    @rm -rf $(FINAL_DIR)/lib/libiot_{utils,log}.a

    @cp -rf $(EXPORT_DIR)/{*.h,imports,exports} $(FINAL_DIR)/include

    @if [ "$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS))" != "" ]; then \
        cd $(FINAL_DIR)/bin; \
        for i in $$(ls); do mv $${i} $${i}.exe; done; \
        cd $${OLDPWD}; \
    fi

    @echo ""
    @echo "========================================================================="
    @echo "o BUILD COMPLETE WITH FOLLOWING SWITCHES:"
    @echo "----"
    @( \
    $(foreach V,$(SWITCH_VARS), \
        $(if $(findstring FEATURE_,$(V)), \
            printf "%-40s : %-s\n" "    $(V)" "$(if $($(V)),$($(V)),n)"; \
        ) \
    ) )
    @echo ""
    @echo "o RELEASE PACKAGE LAYOUT:"
    @echo "----"
    @cd $(FINAL_DIR) && echo -n "    " && pwd && echo "" && \
     find . -not -path "./include/mbedtls/*" -print | awk '!/\.$$/ { \
        for (i = 1; i < NF-1; i++) { \
            printf("|   "); \
        } \
        print "+-- "$$NF}' FS='/' | sed 's!\(.*\)!    &!g'
    @echo ""
    @echo "========================================================================="
endef

define Post_Env
( \
    $(foreach V,$(SWITCH_VARS), \
        $(if $(findstring FEATURE_,$(V)), \
            printf "%-40s : %-s\n" "+ $(V)" "$(if $($(V)),$($(V)),n)"; \
        ) \
    ) \
    echo ""; \
)
endef

