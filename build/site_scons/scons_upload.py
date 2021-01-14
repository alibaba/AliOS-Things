import os, json
import subprocess
import sys
import platform
import serial
import time
import shutil
from serial.tools import miniterm
from serial.tools.list_ports import comports
from scons_util import *

# Global variables
PORT = None

def _check_in_docker(host_os):
    if host_os != 'Linux64':
        return False
    if not os.path.exists("/docker_share"):
        return False
    if not os.path.exists("/proc/1/cgroup"):
        return False
    pattern = re.compile(r"name=(.*)/docker/[0-9a-fA-F]+")
    bFound = False
    with open("/proc/1/cgroup", "r") as f:
        for line in f.readlines():
            match = pattern.search(line.strip())
            if match:
                bFound = True
                break
    return bFound

def _communicate_with_host_pc(req, timeout):
    resp = ""
    timeout *= 5
    if os.path.exists("/docker_share/.docker_burn_response"):
        os.remove("/docker_share/.docker_burn_response")
    for i in range(5):
        if os.path.exists("/docker_share/.docker_burn_request"):
            time.sleep(0.1)
    with open("/docker_share/.docker_burn_request", "w+") as f:
        f.write(req)
    while True:
        if os.path.exists("/docker_share/.docker_burn_response"):
            time.sleep(0.1)
            with open("/docker_share/.docker_burn_response", "r") as f:
                resp = f.read().strip()
                if resp:
                    break
        else:
            timeout -= 1
            if timeout == 0: 
                break
            time.sleep(0.2)
    
    return resp

def _get_host_pc_os():
    host_pc_os = _communicate_with_host_pc("get_os", 5)
    if host_pc_os:
        print("OS of host pc is:%s" % host_pc_os)
    else:
        print("Communicated with host pc is failed. Please start flash programmer in host pc.")
    return host_pc_os

def _check_serial_port(serialport, is_in_docker):
    has_comm = False
    if not is_in_docker:
        for n, (port, desc, hwid) in enumerate(sorted(comports()), 1):
            if serialport == port:
                has_comm = True
                break
    else:
        serial_port_list = _communicate_with_host_pc("get_serialport", 5).split('\n')
        pattern = re.compile(r"---  \d{1,2}: (\S*)")
        if serial_port_list:
            for line in serial_port_list:
                line = line.strip()
                match = pattern.search(line)
                if match and match.group(1):
                    if serialport == match.group(1):
                        has_comm = True
                        break
    return has_comm

def _get_serial_port():
    serial_port = ""
    serial_port_list = _communicate_with_host_pc("get_serialport", 60).split('\n')
    pattern = re.compile(r"---  \d{1,2}: (\S*)")
    ports = []
    if serial_port_list:
        for line in serial_port_list:
            line = line.strip()
            print(line)
            match = pattern.search(line)
            if match and match.group(1):
                ports.append(match.group(1))
    else:
        return serial_port
    
    while True:
        if sys.version_info[0] < 3:
            port = raw_input('--- Enter port index or full name: ')
        else:
            port = input('--- Enter port index or full name: ')
        try:
            index = int(port) - 1
            if not 0 <= index < len(ports):
                sys.stderr.write('--- Invalid index!\n')
                continue
        except ValueError:
            pass
        else:
            serial_port = ports[index]
        return serial_port

def _burn_firmware(exec_cmd):
    exec_cmd_new = []
    for item in exec_cmd:
        if os.path.isfile(item):
            filename = os.path.basename(item)
            exec_cmd_new.append(filename)
            shutil.copyfile(item, os.path.join("/docker_share", filename))
        else:
            exec_cmd_new.append(item)
    info("please find the firmware uploading log in host pc terminal......")
    ret = _communicate_with_host_pc("burn_firmware\n%s" % '\n'.join(exec_cmd_new), 900)
    if ret == "0":
        return 0
    else:
        return 1

def get_config(cur_dir):
    configs = {}
    config_file = os.path.join(cur_dir, '.aos_config_burn')
    if os.path.isfile(config_file):
        configs = read_json(config_file)
        if not configs:
            configs = {}
    if 'serialport' not in configs:
        configs['serialport'] = ""

    return configs

def save_config(cur_dir, config):
    """ save configuration to .config_burn file, only update chip_haas1000 portion """ 
    if config:
        config_file = os.path.join(cur_dir, '.aos_config_burn')
        write_json(config_file, config)

# Functions
def _run_upload_cmd(target, aos_path, cmd_file, program_path=None, bin_dir=None):
    """ Run the command from cmd file """
    ret = 0
    usermsg = None
    global PORT

    host_os = get_host_os()
    if not host_os:
        error("Unsupported Operating System!")

    is_in_docker = _check_in_docker(host_os)
    info("Check whether in docker: %s" % is_in_docker)
    if is_in_docker:
        host_os = _get_host_pc_os()
        if not host_os:
            error("Unsupported Operating System of Host PC!")

    configs = read_json(cmd_file)

    if not configs:
        error("Can not read flash configs from %s" % cmd_file)

    if 'description' in configs:
        usermsg = configs['description']
        if usermsg:
            log(usermsg + '\n\n')

    needsave = False
    myconfig = {}
    if not PORT and '@PORT@' in configs['cmd']:
        myconfig = get_config(program_path if program_path else aos_path)
        if myconfig["serialport"]:
            if not _check_serial_port(myconfig["serialport"], is_in_docker):
                myconfig["serialport"] = ""
        if not myconfig["serialport"]:
            if not is_in_docker:
                PORT = miniterm.ask_for_port()
            else:
                PORT = _get_serial_port()
            myconfig["serialport"] = PORT
            needsave = True
        else:
            PORT = myconfig["serialport"]

    exec_cmd = []
    for item in configs['cmd']:
        if type(item) == dict:
            if host_os in item:
                item = item[host_os]
            else:
                error("Flash command is not defined for %s!" % host_os)

        if '@AOSROOT@/out/@TARGET@' in item:
            if program_path:
                item = item.replace('@AOSROOT@', program_path)

        item = item.replace('@AOSROOT@', aos_path)
        item = item.replace('@TARGET@', target)
        if PORT:
            item = item.replace('@PORT@', PORT)

        exec_cmd += [item]

    # Support user defined binaries' dir
    if bin_dir and os.path.isdir(bin_dir):
        for index, value in enumerate(exec_cmd):
            if '.bin' in value or '.cfg' in value or '.elf' in value:
                basename = os.path.basename(value)
                exec_cmd[index] = os.path.join(bin_dir, basename)

    info("Running cmd:\n\t'%s'" % ' '.join(exec_cmd))

    if not is_in_docker:
        if (host_os == 'Win32'):
            ret = subprocess.call(exec_cmd, shell=True)
            log("---host_os:%s\n" % host_os)
        else:
            ret = subprocess.call(exec_cmd, stdout=sys.stdout, stderr=sys.stderr)
            log("---host_os:%s\n" % host_os)
    else:
        ret = _burn_firmware(exec_cmd)

    if needsave and ret == 0:
        save_config(program_path if program_path else aos_path, myconfig)
    return ret

def _upload_image(target, aos_path, registry_file, program_path=None, bin_dir=None):
    """ Upload image according to configs """
    (app, board) = target.split('@')
    cmd_file_dir = os.path.dirname(registry_file)
    cmd_files = None
    ret = 0

    # Check binary exist
    elf_file = os.path.join(program_path if program_path else aos_path, "out", target, "binary", "%s.bin" % target)
    if not os.path.exists(elf_file):
        error("Please build target[%s] first" % target)

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
        for cmd_file in cmd_files:
            if "not support" in cmd_file:
                info("This command is not supported on %s" % board)
                return 1
            ret = _run_upload_cmd(target, aos_path, os.path.join(cmd_file_dir, cmd_file), program_path, bin_dir)
    else:
        error("The board %s is not registered in %s" % (board, registry_file))

    return ret

def aos_upload(target, work_path=None, bin_dir=None):
    program_path = None

    if '@' not in target or len(target.split('@')) != 2:
        error("Target invalid!")

    if work_path:
        aos_path = os.environ.get("AOS_SDK_PATH")
        if not aos_path or not os.path.isdir(aos_path):
            error("Looks like AOS_SDK_PATH is not correctly set." )
        program_path = os.getcwd()
    else:
        if os.path.isdir('./core') or os.path.isdir('./include/aos'):
            info("Currently in aos_sdk_path: '%s'\n" % os.getcwd())
            aos_path = os.getcwd()
        else:
            info("Not in aos_sdk_path, curr_path:'%s'\n" % os.getcwd())
            aos_path = os.environ.get("AOS_SDK_PATH")
            if not aos_path or not os.path.isdir(aos_path):
                error("Looks like AOS_SDK_PATH is not correctly set." )
            else:
                info("Load aos configs success, set '%s' as sdk path\n" % aos_path)

    registry_file = os.path.split(os.path.realpath(__file__))[0] + '/upload/registry_board.json'
    if os.path.isfile(registry_file):
        ret = _upload_image(target, aos_path, registry_file, program_path, bin_dir)
    else:
        error("Can not find file: %s" % registry_file)

    if ret == 0:
        info("Firmware upload succeed!\n")
    else:
        error("Firmware upload failed!\n")

    return ret

