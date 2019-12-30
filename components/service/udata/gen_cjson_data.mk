CJSON_INPUT_FILE := $(SOURCE_ROOT)/middleware/udata/config/udata_config.json
CJSON_OUTPUT_FILE := $(SOURCE_ROOT)/middleware/udata/config/udata_config.data
EXTRA_PRE_BUILD_TARGETS += gen_cjson_data
gen_cjson_data:
	@python $(SOURCE_ROOT)/middleware/udata/gen_cjson_data.py $(CJSON_INPUT_FILE) $(CJSON_OUTPUT_FILE)
