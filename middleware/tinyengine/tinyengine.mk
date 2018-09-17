NAME := tiny_engine


$(NAME)_SOURCES +=  bone-framwork/3rdlibs/http-parser/http_parser.c \
					bone-framwork/bone-engine/core/bone_engine.c \
					bone-framwork/bone-engine/jse/be_jse_executor.c \
					bone-framwork/bone-engine/jse/be_jse_lex.c \
					bone-framwork/bone-engine/jse/be_jse_module.c \
					bone-framwork/bone-engine/jse/be_jse_symbol.c \
					bone-framwork/bone-engine/jse/be_jse_utils.c \
					bone-framwork/osal/be_osal.c \
					bone-framwork/osal/be_osal_fs.c \
					bone-framwork/osal/be_osal_net.c \
					bone-framwork/common/utils/be_log.c \
					bone-framwork/common/utils/be_utils.c \
					bone-framwork/service/app-mgr/app_mgr.c \
					bone-framwork/service/be-debuger/be_debuger.c \
					bone-framwork/service/board-mgr/board_mgr.c \
					bone-framwork/service/cli/cli_app.c \
					bone-framwork/service/cli/cli_dev.c \
					bone-framwork/service/cli/cli_ext.c \
					bone-framwork/service/cli/cli_file.c \
					bone-framwork/service/cli/cli_js.c \
					bone-framwork/service/cli/cli_sub_call.c \
					bone-framwork/bone-engine/native-modules/js-lite/module_js_lite.c \
					bone-framwork/bone-engine/native-modules/js-lite/module_process.c \
					bone-framwork/bone-engine/native-modules/js-lite/module_timers.c \
					bone-framwork/bone-engine/native-modules/mqtt/module_mqtt.c \
					bone-framwork/bone-engine/native-modules/wifi/module_wifi.c \
					bone-framwork/bone-engine/native-modules/net/module_net.c \
					bone-framwork/bone-engine/native-modules/fs/module_fs.c \
					bone-framwork/bone-engine/native-modules/gpio/module_gpio.c \
					bone-framwork/bone-engine/native-modules/uart/module_uart.c  \
					bone-framwork/bone-engine/native-modules/hw/module_hw.c \
					bone-framwork/bone-engine/native-modules/hw/sensor_dht11.c \
					bone-framwork/bone-engine/native-modules/http/module_http.c \
					bone-framwork/bone-engine/native-modules/i2c/module_i2c.c \

					
#不同的硬件版型有不同的硬件native扩展支持
ifeq ($(CONFIG_SYSINFO_PRODUCT_MODEL),ALI_AOS_ESP32)
$(NAME)_SOURCES += 	bone-framwork/bone-engine/native-modules/adc/module_adc.c \
					bone-framwork/bone-engine/native-modules/pwm/module_pwm.c \
					bone-framwork/bone-engine/native-modules/rtc/module_rtc.c \
					bone-framwork/bone-engine/native-modules/wdg/module_wdg.c \
					bone-framwork/bone-engine/native-modules/dac/module_dac.c 

else ifeq ($(CONFIG_SYSINFO_PRODUCT_MODEL),ALI_AOS_developerkit)
$(NAME)_SOURCES += 	bone-framwork/bone-engine/native-modules/lcd/module_lcd.c 

else	
$(NAME)_SOURCES += 	
endif

$(NAME)_SOURCES += apps/src/tiny_engine_main.c 

# 根据board类型区别编译

$(warning BUILD_TYPE=$(BUILD_TYPE) )
$(warning $(NAME)_BUILD_TYPE=$(bone_engine_BUILD_TYPE) )
$(warning HOST_ARCH=$(HOST_ARCH) )
$(warning CONFIG_SYSINFO_PRODUCT_MODEL=$(CONFIG_SYSINFO_PRODUCT_MODEL) )

#GLOBAL_INCLUDES += boards/$(CONFIG_SYSINFO_PRODUCT_MODEL)
$(NAME)_INCLUDES += bone-framwork/bone-engine/include
$(NAME)_INCLUDES += bone-framwork/common/utils/
$(NAME)_INCLUDES += bone-framwork/osal/include/
$(NAME)_INCLUDES += bone-framwork/common/inc/utils
$(NAME)_INCLUDES+= bone-framwork/common/inc/
$(NAME)_INCLUDES += bone-framwork/service/
$(NAME)_INCLUDES += bone-framwork/3rdlibs/
$(NAME)_INCLUDES += bone-framwork/bone-engine/native-modules/
$(NAME)_INCLUDES += ../../kernel/hal/include/soc/
$(NAME)_INCLUDES += ../../kernel/hal/include/


