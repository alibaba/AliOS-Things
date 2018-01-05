import os, sys, time, serial, subprocess, traceback, glob

def list_devices(os):
    return glob.glob('/dev/mxchip-*')

def new_device(port):
    try:
        ser = serial.Serial(port, 921600, timeout = 0.02)
        ser.setRTS(False)
    except:
        ser = None
        print 'mk3060: open {0} error'.format(port)
    return ser

def erase(port):
    error = 'fail'
    return error

def program(port, address, file):
    retry = 3
    error = 'fail'
    flash_tool_path = os.path.dirname(os.path.realpath(__file__)) + '/mk3060_firmware_update.py'
    while retry > 0:
        script = ['python', flash_tool_path]
        script += [port]
        script += [address]
        script += [file]
        script += ['--hardreset']
        ret = subprocess.call(script)
        if ret == 0:
            error =  'success'
            break
        retry -= 1
        time.sleep(4)
    return error

def control(port, operation):
    try:
        ser = serial.Serial(port, 921600)
    except:
        traceback.print_exc()
        print 'mk3060 control error: unable to open {0}'.format(port)
        return 'fail'
    ret = 'fail'
    try:
        if operation == 'reset':
            ser.setRTS(True)
            time.sleep(0.1)
            ser.setRTS(False)
            ret = 'success'
        elif operation == 'stop':
            ser.setRTS(True)
            ret = 'success'
        elif operation == 'start':
            ser.setRTS(False)
            ret = 'success'
    except:
        pass
    ser.close()
    return ret
