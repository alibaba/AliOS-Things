# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# compile C with /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc
C_DEFINES = -DAOS_KERNEL_BUILD=1 -DESP_PLATFORM=1 -DFFCONF_H=\"/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/oofatfs/ffconf.h\" -DGENERIC -DHAVE_CONFIG_H -DLFS1_NO_ASSERT -DLFS1_NO_DEBUG -DLFS1_NO_ERROR -DLFS1_NO_MALLOC -DLFS1_NO_WARN -DLFS2_NO_ASSERT -DLFS2_NO_DEBUG -DLFS2_NO_ERROR -DLFS2_NO_MALLOC -DLFS2_NO_WARN -DLV_KCONFIG_IGNORE -DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\" -DMICROPY_ESP_IDF_4=1 -DMICROPY_MODULE_FROZEN_MPY="(1)" -DMICROPY_PY_ALIYUNIOT=1 -DMICROPY_PY_DRIVER=1 -DMICROPY_PY_HTTP=1 -DMICROPY_PY_KV=1 -DMICROPY_PY_MODBUS=1 -DMICROPY_PY_OTA=1 -DMICROPY_PY_SNTP=1 -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool -DSYSINFO_SYSTEM_VERSION=\"2.1.0\" -D__DBINTERFACE_PRIVATE=1 -Dvirt_fd_t="void*"

C_INCLUDES = -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/config -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/berkeley-db-1.xx/PORT/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32 -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/boards/GENERIC -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC -I/Users/ethan.lcz1/Desktop/git/aos33/components/osal_aos/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ulog/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/soc/soc/esp32/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/aliyunIoT -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/data-model -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/dynreg -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/subdev -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/devinfo -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/ntp -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/mqtt-upload -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/core -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/network/http -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbmaster/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/kv/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ota/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/amp_adapter/include/peripheral -I/Users/ethan.lcz1/Desktop/git/aos33/components/cjson/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/external/amp_boot -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/external/amp_task -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/external/amp_utils -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/external/app_mgr -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/external/common -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/newlib/platform_include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/freertos/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/freertos/xtensa/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/heap/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/log/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/lwip/include/apps -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/lwip/include/apps/sntp -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/lwip/lwip/src/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/lwip/port/esp32/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/lwip/port/esp32/include/arch -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/soc/src/esp32/. -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/soc/src/esp32/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/soc/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_rom/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_common/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_system/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/xtensa/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/xtensa/esp32/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp32/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/driver/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/driver/esp32/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_ringbuf/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/efuse/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/efuse/esp32/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/espcoredump/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_timer/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_ipc/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/soc/soc/esp32/../include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/soc/soc/esp32/private_include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/vfs/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/esp32/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_event/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_netif/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_eth/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/tcpip_adapter/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/app_trace/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/app_update/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/spi_flash/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bootloader_support/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/common/osi/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/porting/nimble/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/port/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/services/ans/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/services/bas/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/services/gap/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/services/gatt/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/services/ias/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/services/ipss/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/services/lls/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/services/tps/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/util/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/store/ram/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/nimble/host/store/config/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/nimble/porting/npl/freertos/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/bt/host/nimble/esp-hci/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/nvs_flash/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/mbedtls/port/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/mbedtls/mbedtls/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/mbedtls/esp_crt_bundle/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_adc_cal/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_http_client/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/nghttp/port/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/nghttp/nghttp2/lib/includes -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp-tls -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/fatfs/diskio -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/fatfs/vfs -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/fatfs/src -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/wear_levelling/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/sdmmc/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/mdns/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/console -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/ulp/include

C_FLAGS = -mlongcalls -Wno-frame-address -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -ggdb -Os -freorder-blocks -std=gnu99 -Wno-old-style-declaration -D_GNU_SOURCE -DIDF_VER=\"rel_3.3.0-20220517_1000-dirty\" -DESP_PLATFORM -Wno-clobbered -Wno-deprecated-declarations -Wno-missing-field-initializers -Wno-implicit-function-declaration -Wno-unused-const-variable -Wno-format-truncation -Wno-pointer-sign

