import os, sys, time, serial, subprocess, traceback, glob

eml3047_stlink_serials = {
    '/dev/lora-001':'001_serial_id',
    '/dev/lora-002':'002_serial_id'
    }

def list_devices(os):
    return glob.glob('/dev/lora-*')

def new_device(port):
    if port not in eml3047_stlink_serials:
        print('eml3047: unknow board {0}'.format(port))
        return None
    try:
        ser = serial.Serial(port, 115200, timeout = 0.02)
        subprocess.call(['st-flash', '--serial', eml3047_stlink_serials[port], 'reset'])
    except:
        ser = None
        print('eml3047: open {0} error'.format(port))
    return ser

def erase(port):
    retry = 3
    error = 'fail'
    if port not in eml3407_stlink_serials:
        return error
    while retry > 0:
        script = ['st-flash', '--serial', eml3407_stlink_serials[port], 'erase']
        ret = subprocess.call(script)
        if ret == 0:
            error =  'success'
            break
        retry -= 1
        time.sleep(4)
    return error

def program(port, address, file):
    retry = 3
    error = 'fail'
    if port not in eml3047_stlink_serials:
        return error
    while retry > 0:
        script = ['st-flash', '--serial', eml3047_stlink_serials[port]]
        script += ['write', file, address]
        ret = subprocess.call(script)
        if ret == 0:
            error =  'success'
            break
        retry -= 1
        time.sleep(4)
    return error

def control(port, operation):
    ret = 'fail'
    if port not in eml3047_stlink_serials:
        return ret
    try:
        if operation == 'reset':
            subprocess.call(['st-flash', '--serial', eml3047_stlink_serials[port], 'reset'])
            ret = 'success'
        elif operation == 'stop':
            ret = 'fail'
        elif operation == 'start':
            ret = 'fail'
    except:
        pass
    return ret
