#-------------------------------------------------------------------------------
# Copyright (c) 2012 Freescale Semiconductor, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# o Redistributions of source code must retain the above copyright notice, this list
#   of conditions and the following disclaimer.
#
# o Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
#
# o Neither the name of Freescale Semiconductor, Inc. nor the names of its
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#-------------------------------------------------------------------------------

#
# Power test application
#

include $(SDK_ROOT)/mk/common.mk

# Determine if the target is either the MX6DQ or MX6SDL.
ifeq "$(TARGET)" "mx6dq"
is_dq_or_sdl = 1
is_dq = 1
else ifeq "$(TARGET)" "mx6sdl"
is_dq_or_sdl = 1
is_sdl_or_sl = 1
else ifeq "$(TARGET)" "mx6sl"
is_sdl_or_sl = 1
is_sl = 1
endif

# Set up to link application.
APP_NAME = obds
LINK_APP = 1

# Local source files.
#    $(wildcard src/tests/*)
#    src/tests/audio_esai_test.c
#    src/tests/ipu_test.c
#    src/tests/sdio_card_test.c
#    src/tests/usb_tests_obds.c
#   src/tests/weim_nor_flash_test.c
define SOURCES
    $(APPS_ROOT)/common/platform_init.c
    $(APPS_ROOT)/common/print_clock_info.c
    $(APPS_ROOT)/common/print_version.c
    $(APPS_ROOT)/common/ivt.c
    $(SDK_LIB_ROOT)/common/pmic/pfuse/pf0100.c
    $(SDK_LIB_ROOT)/drivers/audio/src/snd_card.c
    $(SDK_LIB_ROOT)/drivers/camera/test/camera_test.c
    $(SDK_LIB_ROOT)/drivers/camera/test/adv7180_capture.c
    $(SDK_LIB_ROOT)/drivers/spi/test/spi_nor_numonyx.c
    $(SDK_LIB_ROOT)/drivers/spi/test/imx_spi_nor_atmel.c
    $(SDK_LIB_ROOT)/drivers/usdhc/test/usdhc_test.c
    src/obds.c
    src/obds_utils.c
    src/tests/audio_esai_test.c
    src/tests/audio_ssi_test.c
    src/tests/audio_ssi_test.c
    src/tests/ddr_test.c
    src/tests/flexcan_test.c
    src/tests/gpio_led.c
    src/tests/gps_test.c
    src/tests/i2c_check_light_sensor_isl29023.c
    src/tests/i2c_device_barometer_ppl3115.c
    src/tests/i2c_device_check_cs42888.c
    src/tests/i2c_device_digital_compass_mag3110.c
    src/tests/i2c_device_digital_compass_mag3112.c
    src/tests/i2c_device_id_check_mma8450.c
    src/tests/i2c_device_id_check_mma8451.c
    src/tests/i2c_device_temp_sensor_emc1046.c
    src/tests/i2c_device_zigbee_mc1323.c
    src/tests/i2c_eeprom_at24cxx_test.c
    src/tests/imx_spi_nor_test.c
    src/tests/mmcsd_test.c
    src/tests/pmic_da9053.c
    src/tests/pmic_lt3589.c
    src/tests/pmic_max17135.c
    src/tests/pmic_mc13892.c
    src/tests/program_board_id.c
    src/tests/program_mac_address.c
    src/tests/snvs_srtc_test.c
    src/tests/touch_button_test.c
    src/tests/touch_screen.c
    src/tests/touch_screen_max11801.c
    $(SDK_LIB_ROOT)/drivers/uart/test/uart_test.c
    src/tests/usb_tests_obds.c
    $(SDK_LIB_ROOT)/drivers/usb/test/usb0_host_test.c
    $(SDK_LIB_ROOT)/drivers/usb/test/usb0_host_testmodes_test.c
    src/tests/weim_nor_flash_test.c
endef

# Add drivers that are only available on mx6dq or mx6sdl.
ifdef is_dq_or_sdl
SOURCES += \
    $(SDK_LIB_ROOT)/common/ioexpander/test/max7310_test.c \
    $(SDK_LIB_ROOT)/drivers/camera/test/sensor_capture.c \
    $(SDK_LIB_ROOT)/drivers/camera/test/csi_test_mode.c \
    $(SDK_LIB_ROOT)/drivers/hdmi/test/hdmi_test.c \
    $(SDK_LIB_ROOT)/drivers/hdmi/test/init_dma_data.c \
    $(SDK_LIB_ROOT)/drivers/ipu/test/ips_display.c \
    $(SDK_LIB_ROOT)/drivers/ipu/test/ipu_image.c \
    src/tests/audio_ssi_test.c \
    src/tests/android_buttons_test.c \
    src/tests/deserializer_ds90ur124.c \
    src/tests/enet_mx6dldq_test.c \
    src/tests/fm_tuner_si476x_test.c \
    src/tests/i2c_device_id_check_p1003_tsc.c \
    src/tests/display_test.c \
    src/tests/mlb_os81050.c \
    src/tests/video_dec_adv7180.c \
    src/tests/video_dec_ARD_MB_reset_button.c \
    src/tests/video_dec_ARD_MBrst_from_cpu_expander.c
endif

# Add drivers that are only available on mx6dq.
ifdef is_dq
SOURCES += \
    $(SDK_LIB_ROOT)/drivers/sata/test/sata_test.c
endif

# Add drivers that are only available on mx6sdl or mx6sl.
ifdef is_sdl_or_sl
SOURCES += \
    $(SDK_LIB_ROOT)/drivers/epdc/test/epdc_test.c
endif
# Add drivers that are only available on mx6sl.
ifdef is_sl
SOURCES += \
    $(SDK_LIB_ROOT)/drivers/camera/test/csi_sensor_capture.c \
    $(SDK_LIB_ROOT)/drivers/fec/test/fec_test.c \
    $(SDK_LIB_ROOT)/drivers/lcdif/test/lcdif_display.c \
    src/tests/keyboard_test.c
endif

# Need to include the SDK library!
LIBRARIES = \
    $(LIBSDK) \
    $(LIBBOARD)

# Specify our linker script.
LD_FILE = $(APPS_ROOT)/common/basic_sdk_app.ld.S

# Add common to include paths.
INCLUDES += -I$(APPS_ROOT)/common -I$(APPS_ROOT)/obds/src


include $(SDK_ROOT)/mk/targets.mk
