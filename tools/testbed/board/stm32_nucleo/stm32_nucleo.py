import os, sys, time, serial, subprocess, traceback, glob

nucleo_stlink_serials = {}

def list_devices(os):
    return glob.glob('/dev/stm32-*')

def new_device(port):
    info = subprocess.check_output(['udevadm', 'info', '-q', 'property', '-n', port])
    vendor_id = None
    model_id  = None
    serial_id = None
    for line in info.split('\n'):
        line.replace('\n', '')
        if line.startswith('ID_VENDOR_ID='):
            vendor_id = line.replace('ID_VENDOR_ID=', '')
        if line.startswith('ID_MODEL_ID='):
            model_id = line.replace('ID_MODEL_ID=', '')
        if line.startswith('ID_SERIAL_SHORT='):
            serial_id = line.replace('ID_SERIAL_SHORT=', '')
    if vendor_id != '0483' or model_id != '374b' or serial_id == None:
        print('stm32_nucleo: parse stlink serial_id for {0} failed'.format(port))
        print('stm32_nucleo: vendor_id:{0} model_id:{1} serial_id:{2}'.format(repr(vendor_id), repr(model_id), repr(serial_id)))
        return None
    if len(serial_id) > 15:
        serial_id = serial_id[:15]
    serial_hexid = ''
    for c in serial_id:
        serial_hexid += '{0:02x}'.format(ord(c))
    nucleo_stlink_serials[port] = serial_hexid
    try:
        ser = serial.Serial(port, 115200, timeout = 0.02)
        subprocess.call(['st-flash', '--serial', nucleo_stlink_serials[port], 'reset'])
    except:
        ser = None
        print('stm32_nucleo: open {0} error'.format(port))
    return ser

def erase(port):
    retry = 3
    error = 'fail'
    if port not in nucleo_stlink_serials:
        return error
    while retry > 0:
        script = ['st-flash', '--serial', nucleo_stlink_serials[port], 'erase']
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
    if port not in nucleo_stlink_serials:
        return error
    while retry > 0:
        script = ['st-flash', '--serial', nucleo_stlink_serials[port]]
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
    if port not in nucleo_stlink_serials:
        return ret
    try:
        if operation == 'reset':
            subprocess.call(['st-flash', '--serial', nucleo_stlink_serials[port], 'reset'])
            ret = 'success'
        elif operation == 'stop':
            ret = 'fail'
        elif operation == 'start':
            ret = 'fail'
    except:
        pass
    return ret
