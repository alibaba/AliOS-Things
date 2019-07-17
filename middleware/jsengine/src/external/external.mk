.PHONY: $(dirs) clean  

include $(PROJECT_ROOT)/.config

exclude_dirs:=libuv cJSON

# 判断是否需要编译 iotkit, 只有配置了MQTT才需要编译iotkit
ifeq ($(JSE_HW_ADDON_MQTT),y)
dirs += iotkit-embedded
endif

#ESP32的idf sdk使用自带的cjson
ifneq ($(JSE_ESP32_BOARD),y)
dirs += cJSON
endif

ECHO ?= $(Q)echo

all:
	$(ECHO) "compile $(dirs)..."
	$(Q)$(foreach N,$(dirs),make -C $(N) -j $(JOBS) || exit "$$?";)  

install:
	$(Q)$(foreach N,$(dirs),make -C $(N) -j $(JOBS) install;) 

clean:  
	$(foreach N,$(dirs),make -C $(N) clean;)  
