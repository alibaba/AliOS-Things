# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# compile C with /Users/ethan.lcz1/.aliot/arm-ali-aoseabi/main/bin/arm-ali-aoseabi-gcc
# compile CXX with /Users/ethan.lcz1/.aliot/arm-ali-aoseabi/main/bin/arm-ali-aoseabi-g++
C_DEFINES = -DAOS_KERNEL_BUILD=1 -DCONFIG_BT=1 -DFFCONF_H=\"/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/oofatfs/ffconf.h\" -DHaaSEDUk1 -DLFS1_NO_ASSERT -DLFS1_NO_DEBUG -DLFS1_NO_ERROR -DLFS1_NO_MALLOC -DLFS1_NO_WARN -DLFS2_NO_ASSERT -DLFS2_NO_DEBUG -DLFS2_NO_ERROR -DLFS2_NO_MALLOC -DLFS2_NO_WARN -DMICROPY_MODULE_FROZEN_MPY="(1)" -DMICROPY_PY_AIAGENT=1 -DMICROPY_PY_ALIYUNIOT=1 -DMICROPY_PY_AUDIO=1 -DMICROPY_PY_BLE=1 -DMICROPY_PY_BLECFG=1 -DMICROPY_PY_DRIVER=1 -DMICROPY_PY_HTTP=1 -DMICROPY_PY_KV=1 -DMICROPY_PY_LVGL=1 -DMICROPY_PY_MINICV=1 -DMICROPY_PY_MODBUS=1 -DMICROPY_PY_OSS=1 -DMICROPY_PY_OTA=1 -DMICROPY_PY_SNTP=1 -DMICROPY_PY_UCAMERA=1 -DMICROPY_PY_UCLOUD_AI=1 -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool -DRHINO_CONFIG_MM_TLF_BLK_SIZE=0x800 -DSYSINFO_SYSTEM_VERSION=\"2.1.0\" -D__DBINTERFACE_PRIVATE=1 -Dvirt_fd_t="void*"

C_INCLUDES = -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/berkeley-db-1.xx/PORT/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/haas/boards/HaaSEDUk1 -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/haas -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/haas/build-HaaSEDUk1 -I/Users/ethan.lcz1/Desktop/git/aos33/components/osal_aos/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ulog/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/aliyunIoT -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/data-model -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/dynreg -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/subdev -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/devinfo -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/ntp -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/mqtt-upload -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/core -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/ble -I/Users/ethan.lcz1/Desktop/git/aos33/components/ble_host/bt_host/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ble_host/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/http/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/network/http -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbmaster/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/kv/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ota/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/oss/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/oss/src -I/Users/ethan.lcz1/Desktop/git/aos33/components/ai_agent/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/minicv -I/Users/ethan.lcz1/Desktop/git/aos33/components/ucloud_ai/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ucamera/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ucamera/include/device/wifi -I/Users/ethan.lcz1/Desktop/git/aos33/components/ucamera/include/device/uart -I/Users/ethan.lcz1/Desktop/git/aos33/components/amp_adapter/include/peripheral -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/peripheral/gpio/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/core/base/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/cjson/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/uvoice/internal -I/Users/ethan.lcz1/Desktop/git/aos33/components/uvoice/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/uvoice/application/alicloudtts/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/sntp/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/lvgl -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/lvgl/src -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/display -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/display/haas -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/external_device/ft6336/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/amp_boot -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/amp_task -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/amp_utils -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/app_mgr -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/common -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbedtls/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbedtls/platform/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbedtls/platform/aos/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/lwip/lwip2.0.0/src/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/lwip/lwip2.0.0/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/fatfs/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/peripheral/uart/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/peripheral/flash/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/vfs/include -I/Users/ethan.lcz1/Desktop/git/aos33/kernel/rhino/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/arch/armv8m/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/drivers/platform/hal -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000 -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/aos/arch -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/aos -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/drivers/rtos/rhino -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/drivers/rtos/rhino/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/board/haaseduk1/config -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng

C_FLAGS =  -mthumb -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -g -Os -std=gnu11 -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -Wno-clobbered -Wno-deprecated-declarations -Wno-missing-field-initializers -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_ALLOCATORS -Wno-unused-function

CXX_DEFINES = -DAOS_KERNEL_BUILD=1 -DCONFIG_BT=1 -DFFCONF_H=\"/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/oofatfs/ffconf.h\" -DHaaSEDUk1 -DLFS1_NO_ASSERT -DLFS1_NO_DEBUG -DLFS1_NO_ERROR -DLFS1_NO_MALLOC -DLFS1_NO_WARN -DLFS2_NO_ASSERT -DLFS2_NO_DEBUG -DLFS2_NO_ERROR -DLFS2_NO_MALLOC -DLFS2_NO_WARN -DMICROPY_MODULE_FROZEN_MPY="(1)" -DMICROPY_PY_AIAGENT=1 -DMICROPY_PY_ALIYUNIOT=1 -DMICROPY_PY_AUDIO=1 -DMICROPY_PY_BLE=1 -DMICROPY_PY_BLECFG=1 -DMICROPY_PY_DRIVER=1 -DMICROPY_PY_HTTP=1 -DMICROPY_PY_KV=1 -DMICROPY_PY_LVGL=1 -DMICROPY_PY_MINICV=1 -DMICROPY_PY_MODBUS=1 -DMICROPY_PY_OSS=1 -DMICROPY_PY_OTA=1 -DMICROPY_PY_SNTP=1 -DMICROPY_PY_UCAMERA=1 -DMICROPY_PY_UCLOUD_AI=1 -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool -DRHINO_CONFIG_MM_TLF_BLK_SIZE=0x800 -DSYSINFO_SYSTEM_VERSION=\"2.1.0\" -D__DBINTERFACE_PRIVATE=1 -Dvirt_fd_t="void*"

CXX_INCLUDES = -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/berkeley-db-1.xx/PORT/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/haas/boards/HaaSEDUk1 -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/haas -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/haas/build-HaaSEDUk1 -I/Users/ethan.lcz1/Desktop/git/aos33/components/osal_aos/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ulog/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/aliyunIoT -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/data-model -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/dynreg -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/subdev -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/devinfo -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/ntp -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/components/mqtt-upload -I/Users/ethan.lcz1/Desktop/git/aos33/components/linksdk/core -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/ble -I/Users/ethan.lcz1/Desktop/git/aos33/components/ble_host/bt_host/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ble_host/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/http/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/network/http -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbmaster/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/kv/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ota/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/oss/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/oss/src -I/Users/ethan.lcz1/Desktop/git/aos33/components/ai_agent/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/minicv -I/Users/ethan.lcz1/Desktop/git/aos33/components/ucloud_ai/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ucamera/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/ucamera/include/device/wifi -I/Users/ethan.lcz1/Desktop/git/aos33/components/ucamera/include/device/uart -I/Users/ethan.lcz1/Desktop/git/aos33/components/amp_adapter/include/peripheral -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/peripheral/gpio/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/core/base/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/cjson/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/uvoice/internal -I/Users/ethan.lcz1/Desktop/git/aos33/components/uvoice/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/uvoice/application/alicloudtts/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/sntp/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/lvgl -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/lvgl/src -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/display -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/modules/display/haas -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/external_device/ft6336/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/amp_boot -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/amp_task -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/amp_utils -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/app_mgr -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/../external/common -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbedtls/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbedtls/platform/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/mbedtls/platform/aos/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/lwip/lwip2.0.0/src/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/lwip/lwip2.0.0/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/fatfs/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/peripheral/uart/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/drivers/peripheral/flash/include -I/Users/ethan.lcz1/Desktop/git/aos33/components/vfs/include -I/Users/ethan.lcz1/Desktop/git/aos33/kernel/rhino/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/arch/armv8m/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/drivers/platform/hal -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000 -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/aos/arch -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/aos -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/drivers/rtos/rhino -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/haas1000/drivers/rtos/rhino/include -I/Users/ethan.lcz1/Desktop/git/aos33/hardware/board/haaseduk1/config -I/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/engine/lib/lv_bindings/driver/png/lodepng

CXX_FLAGS =  -mthumb -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -g -Os -std=gnu11 -fno-common -fmessage-length=0 -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -Wno-clobbered -Wno-deprecated-declarations -Wno-missing-field-initializers -DLODEPNG_NO_COMPILE_ENCODER -DLODEPNG_NO_COMPILE_ALLOCATORS -Wno-unused-function

