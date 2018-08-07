define CompLib_Map
$(eval \
    COMP_LIB_COMPONENTS += \
        $(if \
            $(filter y,$(FEATURE_$(strip $(1)))),$(strip $(2)) \
        ) \
)
endef

define Post_Distro
    @rm -rf $(FINAL_DIR)/include/{LITE*,mbed*}
    @rm -rf $(FINAL_DIR)/lib/libiot_{utils,log}.a

    @cp -rf $(EXPORT_DIR)/{*.h,imports,exports} $(FINAL_DIR)/include

    @if [ "$(filter -D_PLATFORM_IS_WINDOWS_,$(CFLAGS))" != "" ]; then \
        cd $(FINAL_DIR)/bin; \
        for i in $$(ls); do mv $${i} $${i}.exe; done; \
        cd $${OLDPWD}; \
    fi

    @find $(FINAL_DIR) -name "*.[ch]" -exec chmod a-x {} \;
    @mkdir -p $(FINAL_DIR)/src

    $(if $(filter y,$(FEATURE_MQTT_COMM_ENABLED)),
        @cp -f examples/mqtt/mqtt-example.c $(FINAL_DIR)/src/mqtt-example.c
        @cat doc/export.sdk.demo/mqtt.mk >> $(FINAL_DIR)/src/Makefile)
    $(if $(filter y,$(FEATURE_COAP_COMM_ENABLED)),
        @cp -f examples/coap/coap-example.c $(FINAL_DIR)/src/coap-example.c
        @cat doc/export.sdk.demo/coap.mk >> $(FINAL_DIR)/src/Makefile)
    $(if $(filter y,$(FEATURE_HTTP_COMM_ENABLED)),
        @cp -f examples/http/http-example.c $(FINAL_DIR)/src/http-example.c
        @cat doc/export.sdk.demo/http.mk >> $(FINAL_DIR)/src/Makefile)
   $(if $(filter y,$(FEATURE_HTTP2_COMM_ENABLED)),
        @cp -f examples/http2/http2-example.c $(FINAL_DIR)/src/http2-example.c
        @cat doc/export.sdk.demo/http2.mk >> $(FINAL_DIR)/src/Makefile)

    @$(SED) -i 's!CC *:= gcc!CC := $(CC)!g' $(FINAL_DIR)/src/Makefile

    @chmod a-x $(FINAL_DIR)/src/*

    @echo ""
    @echo "========================================================================="
    @echo "o BUILD COMPLETE WITH FOLLOWING SWITCHES:"
    @echo "----"
    @( \
    $(foreach V,$(SWITCH_VARS), \
        $(if $(findstring FEATURE_,$(V)), \
            printf "%-40s : %-s\n" "    $(V)" "$($(V))"; \
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

    @if [ "$(CONFIG_LIB_EXPORT)" = "static" ]; then \
        echo "o BINARY FOOTPRINT CONSIST:"; \
        echo "----"; \
        STAGED=$(LIBOBJ_TMPDIR) STRIP=$(strip $(STRIP)) bash $(RULE_DIR)/scripts/stats_static_lib.sh $(FINAL_DIR)/lib/$(COMP_LIB); \
    fi

    @echo "========================================================================="
    @echo ""
endef

