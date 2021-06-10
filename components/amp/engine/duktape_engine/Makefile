TARGET = libjsengine.a
MODULE = duktape_engine

MOD_SOURCES += \
		addons/hardware/adc/module_adc.c \
		addons/hardware/dac/module_dac.c \
		addons/hardware/can/module_can.c \
		addons/hardware/gpio/module_gpio.c \
		addons/hardware/i2c/module_i2c.c \
		addons/hardware/ir/module_ir.c \
		addons/hardware/lcd/module_lcd.c \
		addons/hardware/pwm/module_pwm.c \
		addons/hardware/rtc/module_rtc.c \
		addons/hardware/spi/module_spi.c \
		addons/hardware/timer/module_timer.c \
		addons/hardware/uart/module_uart.c \
		addons/hardware/wdg/module_wdg.c

MOD_SOURCES += \
		addons/network/http/module_http.c \
		addons/network/mqtt/module_mqtt.c \
		addons/network/mqtt/module_mqtt_client.c \
		addons/network/tcp/module_tcp.c \
		addons/network/udp/module_udp.c \
		addons/network/wifi/module_wifi.c \
		addons/network/cellular/module_cellular.c

MOD_SOURCES += \
		addons/utils/builtin/module_builtin.c \
		addons/utils/checksum/module_checksum.c \
		addons/utils/crypto/module_crypto.c \
		addons/utils/fs/module_fs.c \
		addons/utils/kv/module_kv.c \
		addons/utils/system/module_system.c \
		addons/utils/systimer/module_systimer.c \
		addons/utils/pm/module_pm.c \
		addons/utils/pm/module_battery.c \
		addons/utils/pm/module_charger.c

MOD_SOURCES += \
		addons/advanced/audio/module_tts.c \
		addons/advanced/und/module_und.c \
		addons/advanced/aiot/module_aiot_device.c \
		addons/advanced/aiot/module_aiot_gateway.c \
		addons/advanced/aiot/module_aiot_mqtt.c \
		addons/advanced/aiot/module_aiot_dynreg.c \
		addons/advanced/aiot/module_aiot_activeinfo.c \

MOD_SOURCES += \
		duktape/duktape.c

MOD_SOURCES += \
		be_module_node.c \
		be_refs.c \
		be.c \
		repl.c \
		startup/app_entry.c

# including libjs
MOD_SOURCES += \
		addons/libjs.c

MOD_INCLUDES := \
		./ \
		duktape \
		duktape/addons/advanced/aiot \
		../../main \
		../../adapter/include \
		../../adapter/include/peripheral \
		../../adapter/platform/linux \
		../../utils/mbedtls/include \
		../../utils/mbedtls/platform/include \
		../../utils/mbedtls/platform/amp/include \
		../../utils/cJSON \
		../../utils/checksum \
		../../utils/list \
		../../services/amp_utils \
		../../services/board_mgr \
		../../services/recovery \
		../../components/linksdk/components/bootstrap \
		../../components/linksdk/components/data-model \
		../../components/linksdk/components/devinfo \
		../../components/linksdk/components/diag \
		../../components/linksdk/components/dynreg \
		../../components/linksdk/components/ntp \
		../../components/linksdk/components/subdev \
		../../components/linksdk/core \
		../../components/linksdk/core/sysdep \
		../../components/linksdk/core/utils \
		../../components/ulog \
		../../components/http/include \
		../../components/ota/include \
		../../components/und/include

ifeq ($(ADDON), ui)
MOD_SOURCES += \
		addons/utils/ui/module_vm.c \
		addons/utils/ui/module_ui.c \
		startup/page_entry.c

MOD_INCLUDES += \
		../../ui/render/include \
		../../ui/aui/aui_core \
		../../ui/aui/aui_draw \
		../../ui/aui/aui_fonts \
		../../ui/aui/aui_hal \
		../../ui/aui/aui_misc \
		../../ui/aui/aui_objx \
		../../ui/aui/aui_themes \
		../../ui/aui/libs \
		../../ui/aui/libs/lvgl \
		../../ui/aui/libs/lvgl/src\
		../../ui/aui/libs/lvgl/src/lv_misc \
		../../ui/aui/libs/lvgl/src/lv_font \
		../../ui/aui/libs/lvgl/src/lv_core \
		../../ui/aui/libs/lvgl/src/lv_draw \
		../../ui/aui/libs/lvgl/src/lv_hal \
		../../ui/aui/libs/lvgl/src/lv_objx \
		../../ui/aui/libs/lvgl/src/lv_themes \
		../../utils/ezxml \
		../../utils/lexbor \
		../../utils/lexbor/css \
		../../adapter/include

endif

include $(TOOLS_DIR)/rules.mk
