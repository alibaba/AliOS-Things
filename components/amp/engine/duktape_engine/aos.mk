NAME = libamp_jsengine

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 0.0.1
$(NAME)_SUMMARY := amp component jsengine

$(NAME)_SOURCES += \
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

$(NAME)_SOURCES += \
		addons/network/iot/module_iot.c \
		addons/network/http/module_http.c \
		addons/network/mqtt/module_mqtt.c \
		addons/network/tcp/module_tcp.c \
		addons/network/udp/module_udp.c \
		addons/network/wifi/module_wifi.c \
		addons/network/cellular/module_cellular.c

$(NAME)_SOURCES += \
		addons/utils/builtin/module_builtin.c \
		addons/utils/crypto/module_crypto.c \
		addons/utils/fs/module_fs.c \
		addons/utils/kv/module_kv.c \
		addons/utils/system/module_system.c \
		addons/utils/systimer/module_systimer.c \
		addons/utils/pm/module_pm.c \
		addons/utils/pm/module_battery.c \
		addons/utils/pm/module_charger.c \

$(NAME)_SOURCES += \
		addons/advanced/audio/module_audio.c \
		addons/advanced/audio/module_tts.c \
		addons/advanced/und/module_und.c

$(NAME)_SOURCES += \
		duktape/duktape.c

$(NAME)_SOURCES += \
		be_module_node.c \
		be_refs.c \
		be.c \
		repl.c \
		startup/app_entry.c

# including libjs
$(NAME)_SOURCES += \
		addons/libjs.c

$(NAME)_INCLUDES := \
		./ \
		duktape \
		../../main \
		../../adapter/include \
		../../adapter/include/peripheral \
		../../adapter/platform/aos \
		../../utils/mbedtls/include \
		../../utils/cJSON \
		../../services/amp_utils \
		../../services/board_mgr \
		../../services/recovery \
		../../components/linkkit \
		../../components/linkkit/mqtt \
		../../components/linkkit/infra \
		../../components/linkkit/dev_model \
		../../components/ulog \
		../../components/http/include \
		../../components/und/include

