import os

src = Split('''
        bsp/entry.c
        bsp/setboot_cli.c
        hal/uart.c
        hal/flash.c
        hal/wifi_port.c
        hal/ais_ota_port.c
        hal/misc.c
        hal/i2c.c
        hal/gpio.c
        hal/pwm.c
        bsp/tcpip_adapter_lwip.c
        bsp/wlanif.c
        bsp/ethernetif.c
''')

global_includes = Split('''
        bsp/include
        bsp/include/esp32/include
        bsp/include/soc/esp32/include
        bsp/include/soc/include
        bsp/include/driver/include
        bsp/include/ethernet/include
        bsp/include/log/include
        bsp/include/nvs_flash/include
        bsp/include/spi_flash/include
        bsp/include/container/include
        bsp/include/app_update/include

''')

local_includes = []

global_cflags = Split('''
        -ffunction-sections
        -fdata-sections
        -fstrict-volatile-bitfields
        -mlongcalls
        -DESPOS_FOR_ESP32
''')
local_cflags = ['-std=gnu99']

global_ldflags = Split('''
        -nostdlib
        -Lplatform/mcu/esp32/
        -lc
        -lgcc
        -lstdc++
        -lgcov
        -lm

''')


prebuild_libs = Split('''
        lib/libesp32.a
        lib/libsoc.a
        lib/libhal.a
        lib/libnewlib.a
        lib/libvfs.a
        lib/libspi_flash.a
        lib/liblog.a
        lib/libdriver.a
        lib/libcontainer.a
        lib/librtc.a
        lib/libcoexist.a
        lib/libcore.a
        lib/libnet80211.a
        lib/libpp.a
        lib/libwpa.a
        lib/libwpa2.a
        lib/libwps.a
        lib/libphy.a
        lib/libnvs_flash.a
        lib/libcxx.a
        lib/libstdcc++-cache-workaround.a
        lib/libwpa_supplicant.a
        lib/libapp_update.a
        lib/libbootloader_support.a
''')

global_macro = Split('''
        SYSINFO_PRODUCT_MODEL=\\"ALI_AOS_ESP32\\"
        SYSINFO_DEVICE_NAME=\\"ESP32\\"
''')

dependencis = Split('''
        kernel/hal
        osal
        kernel/init
        kernel/fs/kv
        network/lwip
        security/alicrypto
''')

ld_files = Split('''
        bsp/ld/esp32.ld.S
        bsp/ld/esp32.common.ld
        bsp/ld/esp32.rom.ld
        bsp/ld/esp32.peripherals.ld
        bsp/ld/esp32.rom.spiram_incompatible_fns.ld
''')

if aos_global_config.get('wifi'):
    local_cflags.append('ENABLE_WIFI')
else:
    local_includes.append('bsp')

if aos_global_config.get('vcall') == 'freertos':
    global_includes.append(os.path.join(os.getenv('IDF_PATH'), 'components/espos/include'))
    global_includes.append(os.path.join(os.getenv('IDF_PATH'), 'components/freertos/include'))
    prebuild_libs.append('lib/libespos.a')
    prebuild_libs.append('lib/libfreertos.a')
    prebuild_libs.append('lib/libheap.a')
else:
    dependencis.append('kernel/rhino')
    dependencis.append('platform/arch/xtensa/lx6')
    src.append('aos/hook_impl.c')
    src.append('aos/soc_impl.c')
    src.append('aos/heap_wrapper.c')

ble = aos_global_config.get('mesh', 0)
if ble:
    dependencis.append('network/bluetooth')
    global_includes.append('bsp/include/bt/include')
    local_includes.append('#network/bluetooth/port')
    local_includes.append('#network/bluetooth/host')
    local_includes.append('#network/bluetooth/host/bt_mesh')
    local_includes.append('#network/bluetooth/core/include')
    local_includes.append('#network/bluetooth/include/bluetooth')

    if aos_global_config.get('hci_h4') != 1:
        src.append('ble_hci_driver/hci_driver.c')
    else:
        dependencis.append('kernel/bluetooth/nrf51822')

    prebuild_libs.append('lib/libbt.a')
    prebuild_libs.append('lib/libbtdm_app.a')
    global_macro.append('CONFIG_ESP32_WITH_BLE')
    global_macro.append('CONFIG_XTENSA')

component = aos_mcu_component('esp32', 'xtensa-esp32-elf-', src)

component.add_includes(*local_includes)
component.add_global_includes(*global_includes)

for cflag in global_cflags:
    component.add_global_cflags(cflag)
for cflag in local_cflags:
    component.add_cflags(cflag)

for ldflag in global_ldflags:
    component.add_global_ldflags(ldflag)

for lib in prebuild_libs:
    component.add_prebuilt_libs(lib)

for macro in global_macro:
    component.add_global_macros(macro)

for ld in ld_files:
    aos_global_config.add_ld_files(ld)

component.set_global_arch('xtensa')

component.add_comp_deps(*dependencis)
