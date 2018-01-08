import os, sys, time, serial, subprocess, traceback, glob

def list_devices(os):
    return glob.glob('/dev/espif-*')

def new_device(port):
    try:
        ser = serial.Serial(port, 115200, timeout = 0.02)
        ser.setRTS(True)
        ser.setDTR(False)
        time.sleep(0.1)
        ser.setDTR(True)
    except:
        ser = None
        print 'esp32: open {0} error'.format(port)
    return ser

def erase(port):
    retry = 3
    baudrate = 230400
    error = 'fail'
    flash_tool_path = os.path.dirname(os.path.realpath(__file__)) + '/esptool.py'
    while retry > 0:
        script = ['python', flash_tool_path]
        script += ['--chip']
        script += ['esp32']
        script += ['--port']
        script += [port]
        script += ['--baud']
        script += [str(baudrate)]
        script += ['erase_flash']
        ret = subprocess.call(script)
        if ret == 0:
            error = 'success'
            break
        retry -= 1
        baudrate = baudrate / 2
    return error

def program(port, address, file):
    retry = 3
    baudrate = 230400
    error = 'fail'
    flash_tool_path = os.path.dirname(os.path.realpath(__file__)) + '/esptool.py'
    while retry > 0:
        script = ['python', flash_tool_path]
        script += ['--chip']
        script += ['esp32']
        script += ['--port']
        script += [port]
        script += ['--baud']
        script += [str(baudrate)]
        script += ['--before']
        script += ['default_reset']
        script += ['--after']
        script += ['hard_reset']
        script += ['write_flash']
        script += ['-z']
        script += ['--flash_mode']
        script += ['dio']
        script += ['--flash_freq']
        script += ['40m']
        script += ['--flash_size']
        script += ['4MB']
        script += [address]
        script += [file]
        ret = subprocess.call(script)
        if ret == 0:
            error =  'success'
            break
        retry -= 1
        baudrate = baudrate / 2
    return error

def control(port, operation):
    try:
        ser = serial.Serial(port, 115200)
    except:
        traceback.print_exc()
        print 'esp32 control error: unable to open {0}'.format(port)
        return 'fail'
    ret = 'fail'
    try:
        if operation == 'reset':
            ser.setDTR(False)
            time.sleep(0.1)
            ser.setDTR(True)
            ret = 'success'
        elif operation == 'stop':
            ser.setDTR(False)
            ret = 'success'
        elif operation == 'start':
            ser.setDTR(True)
            ret = 'success'
    except:
        pass
    ser.close()
    return ret
