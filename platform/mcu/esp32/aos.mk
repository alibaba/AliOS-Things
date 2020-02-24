HOST_OPENOCD := esp32

NAME := mcu_esp32

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.2
$(NAME)_SUMMARY    := driver & sdk for platform/mcu esp32

$(NAME)_COMPONENTS += lwip netmgr mbedtls

ESP_INC_PATH    := bsp/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)
GLOBAL_INCLUDES += $(ESP_INC_PATH)/esp32/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/soc/esp32/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/soc/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/driver/include
#GLOBAL_INCLUDES += $(ESP_INC_PATH)/ethernet/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/log/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/nvs_flash/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/spi_flash/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/container/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/app_update/include

GLOBAL_CFLAGS  += -DOTA_DUBANK
GLOBAL_CFLAGS  += -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -DESPOS_FOR_ESP32
GLOBAL_LDFLAGS += -nostdlib -Lplatform/mcu/esp32/ -lc
GLOBAL_LDFLAGS += -lgcc -lstdc++ -lgcov -lm

GLOBAL_LDS_FILES += platform/mcu/esp32/bsp/ld/esp32.ld.S
ifeq ($(esp32_sram),1)
GLOBAL_LDS_FILES += platform/mcu/esp32/bsp/ld/esp32.common.sram.ld
else
GLOBAL_LDS_FILES += platform/mcu/esp32/bsp/ld/esp32.common.ld
endif
GLOBAL_LDS_FILES += platform/mcu/esp32/bsp/ld/esp32.rom.ld
GLOBAL_LDS_FILES += platform/mcu/esp32/bsp/ld/esp32.peripherals.ld
GLOBAL_LDS_FILES += platform/mcu/esp32/bsp/ld/esp32.rom.spiram_incompatible_fns.ld
GLOBAL_LDFLAGS   += -L platform/mcu/esp32/bsp/ld

$(NAME)_SOURCES := bsp/entry.c
$(NAME)_SOURCES += bsp/setboot_cli.c
$(NAME)_SOURCES += hal/uart.c
$(NAME)_SOURCES += hal/flash.c
$(NAME)_SOURCES += hal/wifi_port.c
$(NAME)_SOURCES += bsp/heap_oram.c
ifneq ($(EN_BLE_HOST),)
$(NAME)_SOURCES += hal/ble_port.c
endif
$(NAME)_SOURCES += hal/misc.c
$(NAME)_SOURCES += hal/i2c.c
$(NAME)_SOURCES += hal/gpio.c
$(NAME)_SOURCES += hal/pwm.c
$(NAME)_SOURCES += bsp/tcpip_adapter_lwip.c bsp/wlanif.c
$(NAME)_CFLAGS  := -std=gnu99

ESP_LIB_CORE_TYPE =
ESP_LIB_SRAM =

ifeq ($(OS_MUTICORE_NUM), 2)
ESP_LIB_CORE_TYPE := _dual
GLOBAL_INCLUDES += $(ESP_INC_PATH)/app_update/include
GLOBAL_INCLUDES += $(ESP_INC_PATH)/esp32/include/rom
GLOBAL_INCLUDES += smp
$(NAME)_SOURCES += smp/smp_int.c
$(NAME)_SOURCES += smp/smp_load.c
$(NAME)_SOURCES += smp/smp_sync.c
endif

ifeq ($(esp32_sram),1)
ESP_LIB_SRAM := sram_
endif
ifneq ($(wifi),0)
$(NAME)_CFLAGS += -DENABLE_WIFI
endif
$(NAME)_CFLAGS += -I platform/mcu/esp32/bsp

ifeq (0,1)
libs                     := $(wildcard platform/mcu/esp32/lib/*.a)
libs                     := $(foreach lib,$(libs),lib/$(notdir $(lib)))
$(NAME)_PREBUILT_LIBRARY := $(libs)
endif

$(NAME)_PREBUILT_LIBRARY := lib$(ESP_LIB_CORE_TYPE)/lib$(ESP_LIB_SRAM)esp32.a
$(NAME)_PREBUILT_LIBRARY += lib$(ESP_LIB_CORE_TYPE)/lib$(ESP_LIB_SRAM)spi_flash.a
$(NAME)_PREBUILT_LIBRARY += lib$(ESP_LIB_CORE_TYPE)/libsoc.a
$(NAME)_PREBUILT_LIBRARY += lib$(ESP_LIB_CORE_TYPE)/libbootloader_support.a

$(NAME)_PREBUILT_LIBRARY += lib/libhal.a
$(NAME)_PREBUILT_LIBRARY += lib/libnewlib.a
$(NAME)_PREBUILT_LIBRARY += lib/libvfs.a
$(NAME)_PREBUILT_LIBRARY += lib/liblog.a
$(NAME)_PREBUILT_LIBRARY += lib/libdriver.a
$(NAME)_PREBUILT_LIBRARY += lib/libcontainer.a
$(NAME)_PREBUILT_LIBRARY += lib/librtc.a

$(NAME)_PREBUILT_LIBRARY += lib/libcoexist.a
$(NAME)_PREBUILT_LIBRARY += lib/libcore.a
$(NAME)_PREBUILT_LIBRARY += lib/libnet80211.a
$(NAME)_PREBUILT_LIBRARY += lib/libpp.a
$(NAME)_PREBUILT_LIBRARY += lib/libwpa.a
$(NAME)_PREBUILT_LIBRARY += lib/libwpa2.a
$(NAME)_PREBUILT_LIBRARY += lib/libwps.a
$(NAME)_PREBUILT_LIBRARY += lib/libphy.a
$(NAME)_PREBUILT_LIBRARY += lib/libnvs_flash.a
$(NAME)_PREBUILT_LIBRARY += lib/libcxx.a
$(NAME)_PREBUILT_LIBRARY += lib/libstdcc++-cache-workaround.a
$(NAME)_PREBUILT_LIBRARY += lib/libwpa_supplicant.a
$(NAME)_PREBUILT_LIBRARY += lib/libapp_update.a
$(NAME)_PREBUILT_LIBRARY += lib/libmesh.a

ifeq ($(osal),freertos)
GLOBAL_CFLAGS            += -I $(IDF_PATH)/components/espos/include
GLOBAL_CFLAGS            += -I $(IDF_PATH)/components/freertos/include
$(NAME)_PREBUILT_LIBRARY += lib/libespos.a
$(NAME)_PREBUILT_LIBRARY += lib/libfreertos.a
$(NAME)_PREBUILT_LIBRARY += lib/libheap.a
else
$(NAME)_COMPONENTS       += rhino arch_xtensa_lx6
$(NAME)_SOURCES          += aos/hook_impl.c
$(NAME)_SOURCES          += aos/soc_impl.c
$(NAME)_SOURCES          += aos/heap_wrapper.c
endif

$(NAME)_COMPONENTS-$((EN_BLE_HOST&&!bt_mesh_standalone_deploy)) += bt_host
ifneq ($(EN_BLE_HOST),)
GLOBAL_INCLUDES          += $(ESP_INC_PATH)/bt/include
ifneq ($(hci_h4),1)
$(NAME)_SOURCES          += ble_hci_driver/hci_driver.c
GLOBAL_CFLAGS            += -DBLE_4_2
else

ifeq ($(ble_controller),nrf52840)
GLOBAL_DEFINES += HCI_H4_NRF52840
$(NAME)_SOURCES          += nrf52840/nrf52840.c
GLOBAL_CFLAGS            += -DBLE_4_2
else
error("Invalid ble controller.")
endif
endif
$(NAME)_PREBUILT_LIBRARY += lib/libbt.a
$(NAME)_PREBUILT_LIBRARY += lib/libbtdm_app.a
GLOBAL_DEFINES           += CONFIG_ESP32_WITH_BLE
GLOBAL_DEFINES           += CONFIG_XTENSA
endif

$(NAME)_SOURCES  += hal/adc.c
$(NAME)_SOURCES  += hal/dac.c
$(NAME)_SOURCES  += hal/wdg.c

$(NAME)_SOURCES  += hal/rtc.c

$(NAME)_SOURCES     += bsp/fatfs/fatfs.c
$(NAME)_SOURCES     += bsp/fatfs/diskio.c
$(NAME)_SOURCES     += bsp/fatfs/ff/ff.c
$(NAME)_SOURCES     += bsp/fatfs/ff/ffunicode.c
$(NAME)_SOURCES     += bsp/fatfs/diskio_rawflash.c

GLOBAL_INCLUDES     += bsp/fatfs/include bsp/fatfs/ff/include
GLOBAL_DEFINES      += AOS_FATFS

include platform/mcu/esp32/espos/espos.mk

EXTRA_TARGET_MAKEFILES += $($(HOST_MCU_FAMILY)_LOCATION)/gen_image_bin.mk

