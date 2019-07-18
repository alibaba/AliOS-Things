JSE_ADDONS_DIR := src/jse/mini_engine/addons

# JS Core Addon Modules Selection
#通过Kconfig配置addon模块是否参与编译
ifeq ($(JSE_CORE_ADDON_BUILDIN),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/builtin/module_builtin.c
endif

ifeq ($(JSE_CORE_ADDON_TIMER),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/timer/module_timer.c
${NAME}_INCLUDES += ${JSE_ADDONS_DIR}/timer
endif

ifeq ($(JSE_CORE_ADDON_PROCESS),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/process/module_process.c
endif

ifeq ($(JSE_CORE_ADDON_CRYPTO),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/crypto/module_crypto.c
endif

ifeq ($(JSE_CORE_ADDON_FS),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/fs/module_fs.c
endif

# JSE Protocol & Network & System Addon Modules Selection
# 判断是否需要编译 iotkit, 只有配置了MQTT才需要编译iotkit
ifeq ($(JSE_NET_ADDON_MQTT),y)

${NAME}_SOURCES +=  \
										${JSE_ADDONS_DIR}/mqtt/module_mqtt.c \
										${JSE_ADDONS_DIR}/mqtt/core/mqtt_instance.c \
										${JSE_ADDONS_DIR}/mqtt/core/mqtt_task.c

${NAME}_INCLUDES += \
										${JSE_ADDONS_DIR}/mqtt/core \
										${JSE_ADDONS_DIR}/mqtt
endif
ifeq ($(JSE_NET_ADDON_NET),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/net/module_net.c
endif

ifeq ($(JSE_NET_ADDON_UDP),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/udp/module_udp.c
endif

ifeq ($(JSE_NET_ADDON_HTTP),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/http/module_http.c
endif

ifeq ($(JSE_NET_ADDON_MIIO),y)
${NAME}_INCLUDES += components/miio
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/miio/module_miio.c
endif

# JSE Hardware Addon Modules Selection
ifeq ($(JSE_HW_ADDON_GPIO),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/gpio/module_gpio.c
endif

ifeq ($(JSE_HW_ADDON_UART),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/uart/module_uart.c
endif

ifeq ($(JSE_HW_ADDON_ADC),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/adc/module_adc.c
endif

ifeq ($(JSE_HW_ADDON_DAC),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/dac/module_dac.c
endif

ifeq ($(JSE_HW_ADDON_I2C),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/i2c/module_i2c.c
endif

ifeq ($(JSE_HW_ADDON_IR),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/ir/module_ir.c
endif

ifeq ($(JSE_HW_ADDON_LCD),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/lcd/module_lcd.c
endif

ifeq ($(JSE_HW_ADDON_PWM),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/pwm/module_pwm.c
endif

ifeq ($(JSE_HW_ADDON_WIFI),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/wifi/module_wifi.c
endif

ifeq ($(JSE_HW_ADDON_RTC),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/rtc/module_rtc.c
endif

ifeq ($(JSE_HW_ADDON_WDG),y)
${NAME}_SOURCES += ${JSE_ADDONS_DIR}/wdg/module_wdg.c
endif

