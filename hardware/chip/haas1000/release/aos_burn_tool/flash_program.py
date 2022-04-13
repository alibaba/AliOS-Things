#!/usr/bin/env python
import os, sys, re, time, json
from flash_program_ll import burn_bin_files

try:
    import serial
    from serial.tools import miniterm
except:
    print("\nNot found pyserial, please install it by: \nsudo python%d -m pip install pyserial" % (sys.version_info.major))
    sys.exit(-1)

def get_bin_file():
    """ get binary file from sys.argv --bin=/xxx/yyy/zzz.bin """ 
    bin_files = []
    pattern = re.compile(r'--(.*)=(.*)')
    for arg in sys.argv[1:]:
        if arg.startswith("--"):
            match = pattern.match(arg)
            if match:
                key = match.group(1)
                value = match.group(2)
                if key == 'bin':
                    bin_files.append(value)
    return bin_files

def read_json(json_file):
    data = None
    if os.path.isfile(json_file):
        with open(json_file, 'r') as f:
            data = json.load(f)
    return data

def write_json(json_file, data):
    with open(json_file, 'w') as f:
        f.write(json.dumps(data, indent=4, separators=(',', ': ')))

def get_config():
    """ get configuration from .config_burn file, if it is not existed, 
        generate default configuration of chip_haas1000 """ 
    configs = {}
    config_file = os.path.join(os.getcwd(), '.config_burn')
    if os.path.isfile(config_file):
        configs = read_json(config_file)
        if not configs:
            configs = {}
    if 'chip_haas1000' not in configs:
        configs['chip_haas1000'] = {}
    if 'serialport' not in configs['chip_haas1000']:
        configs['chip_haas1000']['serialport'] = ""
    if 'baudrate' not in configs['chip_haas1000']:
        configs['chip_haas1000']['baudrate'] = "1500000"
    if 'binfile' not in configs['chip_haas1000']:
        configs['chip_haas1000']['binfile'] = []

    return configs['chip_haas1000']

def save_config(config):
    """ save configuration to .config_burn file, only update chip_haas1000 portion """ 
    if config:
        configs = {}
        config_file = os.path.join(os.getcwd(), '.config_burn')
        if os.path.isfile(config_file):
            configs = read_json(config_file)
            if not configs:
                configs = {}
        configs['chip_haas1000'] = config
        write_json(config_file, configs)

def main():
    # step 1: get binary file
    needsave = False
    myconfig = get_config()
    bin_files = get_bin_file()
    if bin_files:
        myconfig["binfile"] = bin_files
        needsave = True
    if not myconfig["binfile"]:
        print("no specified binary file")
        return
    print("binary file is %s" % myconfig["binfile"])

    # step 2: get serial port
    if not myconfig["serialport"]:
        myconfig["serialport"] = miniterm.ask_for_port()
        if not myconfig["serialport"]:
            print("no specified serial port")
            return
        else:
            needsave = True

    print("serial port is %s" % myconfig["serialport"])
    print("the settings were restored in the file %s" % os.path.join(os.getcwd(), '.config_burn'))

    # step 3: burn binary file into flash
    bin_files = []
    for bin_file in myconfig["binfile"]:
        filename = bin_file
        address = "0"
        if "#" in bin_file:
            filename = bin_file.split("#", 1)[0]
            address = bin_file.split("#", 1)[1]
        bin_files.append((filename, address))
    print("bin_files is ", bin_files)
    burn_bin_files(myconfig["serialport"], myconfig['baudrate'], bin_files)
    if needsave:
        save_config(myconfig)

if __name__ == "__main__":
    main()
