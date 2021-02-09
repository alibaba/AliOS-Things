NAME := edu_demo

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.9
$(NAME)_SUMMARY := edu_demo

$(NAME)_SOURCES :=   app_entry.c maintask.c
$(NAME)_COMPONENTS += netmgr cjson
$(NAME)_COMPONENTS-$(AOS_COMP_OLED) += oled
$(NAME)_COMPONENTS-$(AOS_COMP_LCD) += lcd
$(NAME)_COMPONENTS-$(AOS_COMP_SENSOR) += sensor
#AOS_LCD_ILI9341 ?= n
AOS_LCD_ST7789v ?= y
AOS_OLED_SH1106 ?= y
#AOS_TOUCH_FT6336?= y

$(NAME)_COMPONENTS-$(ENABLE_AOS_OTA) += ota

ifeq ($(EDU_DEMO_CONFIG_LINKKIT),y)
$(NAME)_DEFINES += CONFIG_ENABLE_LINKKIT

$(NAME)_COMPONENTS += libiot_devmodel libiot_dynreg

$(NAME)_COMPONENTS-$(AOS_COMP_UND) += und
$(NAME)_COMPONENTS-$(AOS_COMP_SDK_AWSS) += libiot_awss
$(NAME)_SOURCES += common/linkkit/linkkit_entry.c
$(NAME)_SOURCES += common/linkkit/linkkit_event.c
endif
$(NAME)_SOURCES += k1_apps/shakeshake/shakeshake.c
$(NAME)_INCLUDES += k1_apps/shakeshake/
$(NAME)_SOURCES += k1_apps/greedySnake/greedySnake.c
$(NAME)_INCLUDES += k1_apps/greedySnake/
$(NAME)_SOURCES += k1_apps/gyroscope/gyroscope.c
$(NAME)_INCLUDES += k1_apps/gyroscope/
$(NAME)_SOURCES += k1_apps/compass/compass.c
$(NAME)_INCLUDES += k1_apps/compass/
$(NAME)_SOURCES += k1_apps/barometer/barometer.c
$(NAME)_INCLUDES += k1_apps/barometer/
$(NAME)_SOURCES += k1_apps/homepage/homepage.c
$(NAME)_INCLUDES += k1_apps/homepage/
$(NAME)_SOURCES += k1_apps/humiture/humiture.c
$(NAME)_INCLUDES += k1_apps/humiture/
$(NAME)_SOURCES += k1_apps/gyroscope/gyroscope.c
$(NAME)_INCLUDES += k1_apps/gyroscope/
$(NAME)_SOURCES += k1_apps/lightmeter/lightmeter.c
$(NAME)_INCLUDES += k1_apps/lightmeter/
$(NAME)_SOURCES += k1_apps/musicbox/musicbox.c
$(NAME)_INCLUDES += k1_apps/musicbox/
$(NAME)_SOURCES += k1_apps/systemDiag/systemDiag.c
$(NAME)_INCLUDES += k1_apps/systemDiag/
$(NAME)_SOURCES += k1_apps/aircraftBattle/aircraftBattle.c
$(NAME)_INCLUDES += k1_apps/aircraftBattle/
$(NAME)_SOURCES += k1_apps/systemDiag/led_diag/led_diag.c
$(NAME)_INCLUDES += k1_apps/systemDiag/led_diag/
$(NAME)_SOURCES += k1_apps/menu.c
$(NAME)_INCLUDES += k1_apps/

$(NAME)_INCLUDES += ../../../components/peripherals/sensor/include/

ifeq ($(EDU_DEMO_CONFIG_PRINT_HEAP),y)
$(NAME)_DEFINES += CONFIG_PRINT_HEAP
endif

GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=3072
GLOBAL_DEFINES += CONFIG_AOS_FATFS_SUPPORT_MMC

$(NAME)_INCLUDES += ./

ifeq ($(EDK_DEMO_FACTORY_TEST_ENABLIE), y)
GLOBAL_DEFINES += ENABLE_FACTORY_TEST
$(NAME)_COMPONENTS += osal_aos cli kv iobox

$(NAME)_SOURCES +=  mfg_test/adc_test.c \
					mfg_test/oled_test.c \
					mfg_test/sensors_test.c \
					mfg_test/board_test.c \
					mfg_test/key_test.c \
					mfg_test/led_test.c \
					mfg_test/pwm_test.c \
					mfg_test/sd_test.c \
					mfg_test/uart_test.c \
					mfg_test/watchdog_test.c

$(NAME)_INCLUDES += ./mfg_test
$(NAME)_COMPONENTS += osal_aos fatfs littlefs cli
endif
