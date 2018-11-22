import os, sys, json
import platform
import re

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
    """ Read data from json file """
    data = None
    if os.path.isfile(json_file):
        with open(json_file, 'r') as f:
            data = json.load(f)
    else:
        error("Can not find file: %s" % json_file)

    return data

def get_config_value(keyword):
    """ Get predefined value for keyword from .aos """
    value = None
    config_file = '.aos'
    if not os.path.isfile(config_file):
        return value

    with open(config_file) as f:
        for line in f.readlines():
            m = re.match(r'^([\w+-]+)\=(.*)$', line)
            if m and m.group(1) == keyword:
                value = m.group(2)

    return value
