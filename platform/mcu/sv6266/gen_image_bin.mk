EXTRA_POST_BUILD_TARGETS += gen_standard_images

gen_standard_images:
	$(QUIET)$(CP) $(BIN_OUTPUT_FILE) $(OTA_BIN_OUTPUT_FILE)
