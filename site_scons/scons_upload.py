import os
import subprocess
import sys
import platform
import serial
from serial.tools import miniterm
from serial.tools.list_ports import comports


def log(msg):
    sys.stdout.write(msg)
    sys.stdout.flush()

def get_host_os():
    host_os = platform.system()
    if host_os == 'Windows':
        host_os = 'Win32'
    elif host_os == 'Linux':
        if platform.machine().endswith('64'):
            bit = '64'
        else:
            bit = '32'
        host_os += bit
    elif host_os == 'Darwin':
        host_os = 'OSX'
    else:
        host_os = None
    return host_os

def upload_mk3060(target, aos_path):
    host_os = get_host_os()
    if host_os == None:
        error('Unsupported Operating System!')
    cmd = os.path.join(aos_path, 'build', 'aos_firmware_update.py').replace('\\', '/')
    device_port = miniterm.ask_for_port()
    log('\n[INFO]:cmd: %s\n' % cmd)
    addr = '0x13200'
    image_path = os.path.join(os.getcwd(), 'out', target, 'binary', target + '.bin').replace('\\', '/')
    log("\n[INFO]:image_path: '%s'\n" % image_path)

    exec_cmd = [
    cmd,
    device_port, #/dev/ttyUSBx
    addr,
    image_path
    ];
    log("\n[INFO]:exec_cmd: %s\n" % exec_cmd)
    if (host_os == 'Win32'):
        ret = subprocess.call(exec_cmd, shell=True)
        log("---host_os:%s\n" % host_os)
    else:
        ret = subprocess.call(exec_cmd, stdout=sys.stdout, stderr=sys.stderr)
        log("---host_os:%s\n" % host_os)
    return ret

def upload_esp32(target, aos_path):
    host_os = get_host_os()
    if host_os == None:
        error('Unsupported Operating System!')
    cmd = 'esptool.py'
    port = miniterm.ask_for_port()
    image_path = os.path.join(os.getcwd(), 'out', target, 'binary', target + '.bin').replace('\\', '/')
    bootloader_path = os.path.join(aos_path, 'platform/mcu/esp32/bsp/bootloader.bin').replace('\\', '/')
    partition_path = os.path.join(aos_path, 'platform/mcu/esp32/bsp/custom_partitions.bin').replace('\\', '/')
    log("\n[INFO]:image_path: %s\n" % image_path)
    log("[INFO]:bootloader_path: %s\n" % bootloader_path)
    log("[INFO]:partition_path: %s\n" % partition_path)

    exec_cmd = [
        cmd,
        '--chip',
        'esp32',
        '--port', port,
        '--baud', '921600',
        '--before', 'default_reset',
        '--after', 'hard_reset',
        'write_flash',
        '-z',
        '--flash_mode',
        'dio',
        '--flash_freq',
        '40m',
        '--flash_size',
        'detect',
        '0x1000', bootloader_path,
        '0x8000', partition_path,
        '0x10000', image_path
    ]
    log("\n[INFO]:exec_cmd: %s\n" % exec_cmd)
    if (host_os == 'Win32'):
        ret = subprocess.call(exec_cmd, shell=True)
        log("---host_os:%s\n" % host_os)
    else:
        ret = subprocess.call(exec_cmd, stdout=sys.stdout, stderr=sys.stderr)
        log("---host_os:%s\n" % host_os)
    return ret

def upload_esp8266(target, aos_path):
    host_os = get_host_os()
    if host_os == None:
        error('Unsupported Operating System!')
    cmd = 'esptool.py'
    port = miniterm.ask_for_port()
    image_path = os.path.join(os.getcwd(), 'out', target, 'binary', target + '-0x1000.bin').replace('\\', '/')
    boot_bin_path = os.path.join(aos_path, 'platform/mcu/esp8266/bsp/boot_v1.7_921600.bin').replace('\\', '/')
    init_bin_path = os.path.join(aos_path, 'platform/mcu/esp8266/bsp/esp_init_data_default.bin').replace('\\', '/')
    blank_bin_path = os.path.join(aos_path, 'platform/mcu/esp8266/bsp/blank.bin').replace('\\', '/')

    exec_cmd = [
        cmd,
        '--chip', 'esp8266',
        '--port', port,
        '--baud', '921600',
        'write_flash',
        '--flash_size',
        'detect',
        '0x0', boot_bin_path,
        '0x3fc000', init_bin_path,
        '0x3fe000', blank_bin_path,
        '0x1000', image_path
    ]
    log("\n[INFO]:exec_cmd: %s\n" % exec_cmd)
    if (host_os == 'Win32'):
        ret = subprocess.call(exec_cmd, shell=True)
        log("---host_os:%s\n" % host_os)
    else:
        ret = subprocess.call(exec_cmd, stdout=sys.stdout, stderr=sys.stderr)
        log("---host_os:%s\n" % host_os)
    return ret

def upload_atsam(target, aos_path):
    """Prerequisite to use this function:
    1. Atmel Studio 7 installed
    2. Add atprogram.exe file path (<AS7 install folder>/Atmel/Studio/7.0/atbackend) to Windows PATH environment variable
    """
    host_os = get_host_os()
    if (host_os == None) or (host_os != 'Win32'):
        error('Unsupported Operating System!')

    image_path = os.path.join(os.getcwd(), 'out', target, 'binary', target + '.elf').replace('\\', '/')
    board = target.split('@')[1]
    if 'atsame54' in board:
        exec_cmd = 'atprogram.exe -t edbg -i SWD --clock 4mhz -d ATSAME54P20A program --chiperase --verify -f ' + image_path
    else:
        error('Unsupported board!')
    log("[INFO]:exec_cmd: %s\n" % exec_cmd)
    ret = subprocess.call(exec_cmd, shell=True)
    return ret

def upload_stm32_stlink(target, aos_path):
    upload_cmds = {
        'Win32': 'win32/st-flash.exe',
        'Linux64': 'linux64/st-flash',
        'Linux32': 'linux32/st-flash',
        'OSX': 'osx/st-flash'
    }
    board = target.split('@')[1]
    host_os = get_host_os()
    if host_os == None:
        error('Unsupported Operating System!')

    cmds_dir = os.path.join(aos_path, 'build', 'cmd').replace('\\', '/')
    upload_cmd = os.path.join(cmds_dir, upload_cmds[host_os]).replace('\\', '/')
    print("\n[INFO]:upload_cmd: '%s', host_os:'%s'\n" %(upload_cmd, host_os))
    addr = '0x08000000'

    if 'starterkit' in board:
        # remove unnecessay erase step
        erase_cmd = [upload_cmd, 'erase']

        ret = subprocess.call(erase_cmd, stdout=sys.stdout, stderr=sys.stderr)
        if ret == 0:
            print('erase %s succeed' % board)
        else:
            print('erase %s failed' % board)

    image_path = os.path.join(os.getcwd(), 'out', target, 'binary', target + '.bin')
    print("\n[INFO]:image_path: '%s'\n" % image_path)

    exec_cmd = [upload_cmd, '--reset', 'write', image_path, addr];
    print("\n[INFO]:exec_cmd: '%s'\n" % exec_cmd)
    ret = subprocess.call(exec_cmd, stdout=sys.stdout, stderr=sys.stderr)
    return ret

def upload_stm32_openocd(target, aos_path):
    upload_openocd_cmds = {
        'Win32': 'bin/openocd_win',
        'Linux64': 'bin/openocd_linux',
        'Linux32': 'bin/openocd_linux',
        'OSX': 'bin/openocd_osx'
    }
    board = target.split('@')[1]
    host_os = get_host_os()
    if host_os == None:
        error('Unsupported Operating System!')

    cmds_dir = os.path.join(aos_path, 'build', 'openocd_master').replace('\\', '/')
    print('cur_path:%s\n' % os.getcwd())
    upload_cmd = os.path.join(cmds_dir, upload_openocd_cmds[host_os]).replace('\\', '/')
    config_file = os.path.join(cmds_dir, 'share/openocd/scripts/board/st_nucleo_l4.cfg ').replace('\\', '/')
    image_path = os.path.join(os.getcwd(), 'out', target, 'binary', target + '.bin')
    print("\n[INFO]:image_path: '%s'\n" % image_path)
    cmd_str = 'init; reset halt;' + 'flash write_image erase ' + image_path + ' 0x08000000; reset run; reset; exit;'
    print("\n[INFO]:upload_cmd: '%s'\n" % upload_cmd)
    exec_cmd = [upload_cmd, '-f', config_file, '-c', cmd_str];
    print("\n[INFO]:exec_cmd: '%s'\n" % exec_cmd)
    ret = subprocess.call(exec_cmd, stdout=sys.stdout, stderr=sys.stderr)
    return ret

def upload_stm32(target, aos_path):
    openocd_vaild = False

    if os.path.isdir('./build/openocd_master') == False:
        print('[Info]: Can not find openocd, upload via st-link!\n')
    #else:
        #openocd_vaild = True
    if openocd_vaild == True:
        print('[Info]: Find openocd now, upload via openocd!')
        ret = upload_stm32_openocd(target, aos_path)
    else:
        ret = upload_stm32_stlink(target, aos_path)

    return ret


def _run_upload(target, aos_path):
    app = target.split('@')[0]
    board = target.split('@')[1]
    ret = None

    if 'esp32' in board:
        ret = upload_esp32(target, aos_path)
    elif 'mk3060' in board:
        ret = upload_mk3060(target, aos_path)
    elif 'esp8266' in board:
        ret = upload_esp8266(target, aos_path)
    elif 'atsam' in board:
        ret = upload_atsam(target, aos_path)
    else:
        ret = upload_stm32(target, aos_path)

    return ret

def aos_upload(target):
    print("[INFO]:target: %s\n" % target)

    if os.path.isdir('./include/aos') == False:
        print("[INFO]:Not in aos_sdk_path, curr_path:'%s'\n" % os.getcwd())
        aos_path = Global().get_cfg(AOS_SDK_PATH)
        if aos_path == None:
            error("[ERRO]:Not in aos_sdk_path, aos_sdk unavailable as well. Please run 'aos new $prj_name'!")
        else:
            print("[INFO]:Config Loading OK, use '%s' as sdk path\n" % aos_path)
    else:
        aos_path = os.getcwd()
        print("[INFO]:Currently in aos_sdk_path: '%s'\n" % os.getcwd())

    if '@' not in target or len(target.split('@')) != 2:
        error('target invalid')
        return -1;

    ret = _run_upload(target, aos_path)

    if ret == 0:
        print('[INFO]:firmware upload succeed!\n')
    else:
        print('[ERROR]:firmware upload failed!\n')
    return ret