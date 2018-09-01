import os, json
import subprocess
import sys
import platform
import serial
from serial.tools import miniterm
from serial.tools.list_ports import comports

def log(msg):
    sys.stdout.write(msg)
    sys.stdout.flush()

def info(msg):
    log("[INFO]: %s\n" % msg)

def error(msg, code=-1):
    sys.stderr.write("[ERROR]: %s\n" % msg)
    sys.exit(code)

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

def read_json(json_file):
    """ Read flash command from json file """
    cmd = None
    if os.path.isfile(json_file):
        with open(json_file, 'r') as f:
            cmd = json.load(f)
    else:
        error("Can not find file: %s" % json_file)

    return cmd

def _run_upload_cmd(port, target, aos_path, cmd_file, bin_dir=None):
    """ Run the command from cmd file """
    ret = 0
    usermsg = None

    host_os = get_host_os()
    if not host_os:
        error("Unsupported Operating System!")

    configs = read_json(cmd_file)

    if not configs:
        error("Can not read flash configs from %s" % cmd_file)

    exec_cmd = []
    for item in configs['cmd']:
        if type(item) == dict:
            if host_os in item:
                item = item[host_os]
            else:
                error("Flash command is not defined for %s!" % host_os)

        item = item.replace('@AOSROOT@', aos_path)
        item = item.replace('@PORT@', port)
        item = item.replace('@TARGET@', target)
        exec_cmd += [item]

    if 'description' in configs:
        usermsg = configs['description']

    # Support user defined binaries' dir
    if bin_dir and os.path.isdir(bin_dir):
        for index, value in enumerate(exec_cmd):
            if '.bin' in value:
                binname = os.path.basename(value)
                exec_cmd[index] = os.path.join(bin_dir, binname)
            if '.cfg' in value:
                cfgname = os.path.basename(value)
                exec_cmd[index] = os.path.join(bin_dir, cfgname)

    if usermsg:
        info(usermsg)

    info("Running cmd:\n\t'%s'" % ' '.join(exec_cmd))

    if (host_os == 'Win32'):
        ret = subprocess.call(exec_cmd, shell=True)
        log("---host_os:%s\n" % host_os)
    else:
        ret = subprocess.call(exec_cmd, stdout=sys.stdout, stderr=sys.stderr)
        log("---host_os:%s\n" % host_os)

    return ret

def _upload_image(target, aos_path, registry_file, bin_dir=None):
    """ Upload image according to configs """
    (app, board) = target.split('@')
    cmd_file_dir = os.path.dirname(registry_file)
    cmd_files = None
    ret = 0

    # Get valid board from registry file
    registry_board = read_json(registry_file)

    if not registry_board:
        error("Can not read registered boards from %s" % registry_file)

    # 1. Fully match with board name: key = board
    for key in registry_board:
        if key == board:
            cmd_files = registry_board[key]

    # 2. Part match with board name: key in board
    if not cmd_files:
        for key in registry_board:
            if key in board:
                cmd_files = registry_board[key]

    # 3. Use 'ALLOTHERS' if cmd_files is still None
    if not cmd_files:
        if 'ALLOTHERS' in registry_board:
            cmd_files = registry_board['ALLOTHERS']

    if cmd_files:
        port = miniterm.ask_for_port()
        for cmd_file in cmd_files:
            ret = _run_upload_cmd(port, target, aos_path, os.path.join(cmd_file_dir, cmd_file), bin_dir)
    else:
        error("The board %s is not registered in %s" % (board, registry_file))

    return ret

def aos_upload(target, work_path=None, bin_dir=None):
    info("Target: %s" % target)

    if '@' not in target or len(target.split('@')) != 2:
        error("Target invalid!")

    if work_path:
        aos_path = work_path
    else:
        if os.path.isdir('./kernel/rhino'):
            info("Currently in aos_sdk_path: '%s'\n" % os.getcwd())
            aos_path = os.getcwd()
        else:
            info("Not in aos_sdk_path, curr_path:'%s'\n" % os.getcwd())
            aos_path = Global().get_cfg(AOS_SDK_PATH)
            if not aos_path:
                error("aos_sdk is unavailable, please run 'aos new $prj_name'!")
            else:
                info("Load aos configs success, set '%s' as sdk path\n" % aos_path)

    registry_file = os.path.split(os.path.realpath(__file__))[0] + '/upload/registry_board.json'
    if os.path.isfile(registry_file):
        ret = _upload_image(target, aos_path, registry_file, bin_dir)
    else:
        error("Can not find file: %s" % registry_file)

    if ret == 0:
        info("Firmware upload succeed!\n")
    else:
        error("Firmware upload failed!\n")

    return ret
