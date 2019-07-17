JSE_ADDONS_DIR := src/jse/mini_engine/addons

# JS Core Addon Modules Selection
#通过Kconfig配置addon模块是否参与编译
ifeq ($(JSE_CORE_ADDON_BUILDIN),y)
${NAME}_SOURCES += builtin/module_builtin.c
endif

ifeq ($(JSE_CORE_ADDON_TIMER),y)
${NAME}_SOURCES += timer/module_timer.c
endif

ifeq ($(JSE_CORE_ADDON_PROCESS),y)
${NAME}_SOURCES += process/module_process.c
endif

ifeq ($(JSE_CORE_ADDON_CRYPTO),y)
# TODO:
# C_SOURCES += crypto/module_crypto.c
endif

ifeq ($(JSE_CORE_ADDON_FS),y)
${NAME}_SOURCES += fs/module_fs.c
endif

# JSE Protocol & Network & System Addon Modules Selection
# 判断是否需要编译 iotkit, 只有配置了MQTT才需要编译iotkit
ifeq ($(JSE_NET_ADDON_MQTT),y)
${NAME}_CFLAGS += -Wno-int-conversion

${NAME}_SOURCES +=  \
										mqtt/module_mqtt.c \
										mqtt/core/mqtt_instance.c

${NAME}_INCLUDES += \
										mqtt/core \
										mqtt

# iotkit头文件
# C_INCLUDES += -I$(EXTERNAL_ROOT)/iotkit-embedded/src/utils/digest
# C_INCLUDES += -I$(EXTERNAL_ROOT)/iotkit-embedded/src/utils/misc
# C_INCLUDES += -I$(PROJECT_ROOT)/port/iotkit
# C_INCLUDES += -I$(EXTERNAL_ROOT)/iotkit-embedded/src/sdk-impl
# C_INCLUDES += -I$(EXTERNAL_ROOT)/iotkit-embedded/src/system
# C_INCLUDES += -I$(EXTERNAL_ROOT)/iotkit-embedded/src/packages/LITE-utils
# C_INCLUDES += -I$(EXTERNAL_ROOT)/iotkit-embedded/src/packages/LITE-log
# C_INCLUDES += -I$(EXTERNAL_ROOT)/iotkit-embedded/src/sdk-impl/exports

# 加密库 需要 cJson
# ifeq ($(JSE_ESP32_BOARD),y)
# C_INCLUDES += -I$(VENDOR_ROOT)/esp-idf/components/mbedtls/mbedtls/include
# C_INCLUDES += -I$(VENDOR_ROOT)/esp-idf/components/json/cJSON
# endif

endif

ifeq ($(JSE_NET_ADDON_NET),y)
# TODO:
${NAME}_SOURCES += net/module_net.c
endif

ifeq ($(JSE_NET_ADDON_UDP),y)
${NAME}_SOURCES += udp/module_udp.c
endif

ifeq ($(JSE_NET_ADDON_HTTP),y)
# TODO:
# C_SOURCES += http/module_http.c
endif

ifeq ($(JSE_NET_ADDON_MIIO),y)
# ${NAME}_INCLUDES += $(PROJECT_ROOT)/components/miio
# ${NAME}_SOURCES += miio/module_miio.c \
# $(PROJECT_ROOT)/components/miio/miio-discover.c \
# $(PROJECT_ROOT)/components/miio/miio-device.c
endif

# JSE Hardware Addon Modules Selection
ifeq ($(JSE_HW_ADDON_GPIO),y)
${NAME}_SOURCES += gpio/module_gpio.c
endif

ifeq ($(JSE_HW_ADDON_UART),y)
${NAME}_SOURCES += uart/module_uart.c
endif

ifeq ($(JSE_HW_ADDON_ADC),y)
${NAME}_SOURCES += adc/module_adc.c
endif

ifeq ($(JSE_HW_ADDON_DAC),y)
${NAME}_SOURCES += dac/module_dac.c
endif

ifeq ($(JSE_HW_ADDON_I2C),y)
${NAME}_SOURCES += i2c/module_i2c.c
endif

ifeq ($(JSE_HW_ADDON_IR),y)
${NAME}_SOURCES += ir/module_ir.c
endif

ifeq ($(JSE_HW_ADDON_LCD),y)
${NAME}_SOURCES += lcd/module_lcd.c
endif

ifeq ($(JSE_HW_ADDON_PWM),y)
${NAME}_SOURCES += pwm/module_pwm.c
endif

ifeq ($(JSE_HW_ADDON_WIFI),y)
${NAME}_SOURCES += wifi/module_wifi.c
endif

ifeq ($(JSE_HW_ADDON_RTC),y)
${NAME}_SOURCES += rtc/module_rtc.c
endif

ifeq ($(JSE_HW_ADDON_WDG),y)
${NAME}_SOURCES += wdg/module_wdg.c
endif
