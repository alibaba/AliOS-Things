
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
	    if [ -d $(IMPORT_DIR)/$${VDR_NAME}/$(PREBUILT_LIBDIR) ]; then \
	        cp -f $(IMPORT_DIR)/$${VDR_NAME}/$(PREBUILT_LIBDIR)/lib* $(FINAL_DIR)/lib; \
	    fi; \
	fi

	$(TOP_Q) \
	if [ "$$(ls $(FINAL_DIR)/bin/)" != "" ]; then \
	    $(STRIP) $(FINAL_DIR)/bin/* || (echo "$(STRIP) $(FINAL_DIR)/bin/* failed!" && exit 1); \
	fi
	$(TOP_Q)$(STRIP) --strip-debug $(FINAL_DIR)/lib/* || (echo "$(STRIP) $(FINAL_DIR)/lib/* failed!" && exit 1)

	$(TOP_Q)+$(call $(POST_FINAL_OUT_HOOK))
