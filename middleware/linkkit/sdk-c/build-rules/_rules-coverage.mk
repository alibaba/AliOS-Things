.PHONY: coverage lcov test

ifneq (,$(COVERAGE_LIST))
COVERAGE_PROGS  := \(
COVERAGE_PROGS  += $(COVERAGE_LIST)
COVERAGE_PROGS  += \)
COVERAGE_CMD    := $(RULE_DIR)/scripts/exe_coverage_progs.sh
endif

ifeq (,$(COVERAGE_CMD))
coverage lcov test:
	@echo "COVERAGE_CMD not defined, skip"
else
coverage lcov test: all $(UTEST_RECP)
#
#	SKIP --coverage existing in $(CFLAGS) check for now
#
	$(Q)rm -rf $(OUTPUT_DIR)/$(LCOV_DIR) $(DIST_DIR)/$(LCOV_DIR)
	$(Q)rm -f $(OUTPUT_DIR)/{files,tests,all,final}.info
	$(Q)find $(OUTPUT_DIR) -name "*.gcno" -o -name "*.gcda" -exec rm -f {} \;
	$(Q)OUTPUT_DIR=$(OUTPUT_DIR) bash <($(SED) '2iPROGS=$(COVERAGE_PROGS)' $(COVERAGE_CMD)) || true
	$(Q)lcov --quiet \
	    --capture --initial --directory $(OUTPUT_DIR)/ \
	    -o $(OUTPUT_DIR)/files.info
	$(Q)lcov --quiet \
	    --capture --directory $(OUTPUT_DIR) \
	    -o $(OUTPUT_DIR)/tests.info
	$(Q)lcov --quiet \
	    --add-tracefile $(OUTPUT_DIR)/files.info \
	    --add-tracefile $(OUTPUT_DIR)/tests.info \
	    -o $(OUTPUT_DIR)/all.info
	$(Q)lcov --quiet \
	    --remove $(OUTPUT_DIR)/all.info \
	    -o $(OUTPUT_DIR)/final.info '*.h'
	$(Q)genhtml --quiet \
	    --legend --no-branch-coverage -o $(OUTPUT_DIR)/$(LCOV_DIR) $(OUTPUT_DIR)/final.info
	$(Q)cp -rf $(OUTPUT_DIR)/$(LCOV_DIR) $(DIST_DIR)/$(LCOV_DIR)
	$(Q)cd $(DIST_DIR)/$(LCOV_DIR) && \
	    $(SED) -i 's:\(coverFile.*\)>$(OUTPUT_DIR)/:\1>:g' index.html
	$(Q)bash $(RULE_DIR)/scripts/gen_lcov_report.sh $(DIST_DIR)/$(LCOV_DIR)
endif
