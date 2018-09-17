
final-out: sub-mods
ifneq (,$(COMP_LIB_NAME))
	$(TOP_Q) \
	if  [ ! -f $(SYSROOT_LIB)/lib$(COMP_LIB_NAME).a ] && \
	    [ ! -f $(SYSROOT_LIB)/lib$(COMP_LIB_NAME).so ]; then \
	    $(call Build_CompLib, FORCE) \
	fi;
endif

	$(TOP_Q) \
	if [ -f $(STAMP_PRJ_CFG) ]; then true; else \
	    rm -rf $(DIST_DIR); \
	    mkdir -p $(DIST_DIR) $(FINAL_DIR); \
	    for i in bin lib include; do \
	        if [ -d $(OUTPUT_DIR)/usr/$${i} ]; then \
	            cp -rf $(OUTPUT_DIR)/usr/$${i} $(FINAL_DIR); \
	        fi; \
	    done; \
	    VDR_NAME=$$(grep -m 1 "VENDOR *:" $(CONFIG_TPL) 2>/dev/null|awk '{ print $$NF }'); \
	    if [ "$$(ls $(IMPORT_DIR)/$${VDR_NAME}/$(PREBUILT_LIBDIR)/lib* 2>/dev/null)" != "" ]; then \
	        cp -f $(IMPORT_DIR)/$${VDR_NAME}/$(PREBUILT_LIBDIR)/lib* $(FINAL_DIR)/lib; \
	    fi; \
	fi

	$(TOP_Q) \
	if [ "$$(ls $(FINAL_DIR)/bin/ 2>/dev/null)" != "" ]; then \
	    $(STRIP) $(FINAL_DIR)/bin/* 2>/dev/null || (echo "$(STRIP) $(FINAL_DIR)/bin/* failed!" && exit 1); \
	fi
	$(TOP_Q) \
	if [ "$$(ls $(FINAL_DIR)/lib/*.so 2>/dev/null)" != "" ]; then \
	    $(STRIP) $(STRIP_DBGOPT) $(FINAL_DIR)/lib/*.so 2>/dev/null || (echo "$(STRIP) $(FINAL_DIR)/lib/*.so failed!" && exit 1); \
	fi

ifeq ($(strip $(HAS_POST_HOOK)), 1)
	$(TOP_Q)+$(call $(POST_FINAL_OUT_HOOK))
endif
ifneq (,$(filter all,$(strip $(MAKECMDGOALS))))
	$(TOP_Q)+$(call $(POST_FINAL_OUT_HOOK))
endif

	$(TOP_Q)$(foreach V,$(INFO_ENV_VARS),$(V)="$($(V))") \
	    CFLAGS=$(CFLAGS) \
	    bash $(RULE_DIR)/scripts/gen_rom_stats.sh
