import os, sys, time
from autotest import Autotest
from mesh_common import *

required_devices = [ [1, 'general'] ]

def main(firmware='~/lb-all.bin', model='mk3060', duration = 12 * 3600, withalink=True):
    ap_ssid = 'aos_test_01'
    ap_pass = 'Alios@Embedded'
    server = '11.238.148.46'
    port = 34568

    #parse input
    i = 1
    while i < len(sys.argv):
        arg = sys.argv[i]
        if arg.startswith('--firmware='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --firmware=firmware.bin'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            firmware = args[1]
        elif arg.startswith('--model='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --model=mk3060'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            model = args[1].lower()
        elif arg.startswith('--duration='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --duration=12'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            try:
                tmp = float(args[1])
            except:
                print 'wrong argument {0} input, example: --duration=12'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            duration = tmp * 3600
        elif arg.startswith('--withalink='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --withalink=1/0'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            withalink = (args[1] == '1')
        elif arg.startswith('--wifissid='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --wifissid=test_wifi'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            ap_ssid = args[1]
        elif arg.startswith('--wifipass='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --wifipass=test_password'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            ap_pass = args[1]
        elif arg.startswith('--server='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --server=192.168.10.16'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            server = args[1]
        elif arg.startswith('--port='):
            args = arg.split('=')
            if len(args) != 2 or args[1].isdigit() == False:
                print 'wrong argument {0} input, example: --port=34568'.format(arg)
                return [1, 'argument {0} error'.format(arg)]
            port = int(args[1])
        elif arg=='--help':
            print 'Usage: python {0} [--firmware=xxx.bin] [--model=xxx] [--duration=xx] [--withalink=1/0] [--wifissid=wifi_ssid] [--wifipass=password]'.format(sys.argv[0])
            return [0, 'help']
        i += 1

    at=Autotest()
    logname=time.strftime('%Y-%m-%d@%H-%M')
    logname = 'stability-' + logname +'.log'
    if at.start(server, port, logname) == False:
        print 'error: start failed'
        return [1, 'connect testbed failed']

    #request device allocation
    number, purpose = required_devices[0]
    timeout = 3600
    allocated = allocate_devices(at, model, number, timeout, purpose)
    if len(allocated) != number:
        return [1, 'allocate device failed']

    devices = {'A':allocated[0]}
    device = 'A'

    #subscribe and reboot devices
    result = subscribe_and_reboot_devices(at, devices)
    if result == False:
        return [1, 'subscribe devices failed']

    #program devices
    result = program_devices(at, devices, model, firmware)
    if result == False:
        return [1, 'program device failed']
    time.sleep(5)

    #restore state to default
    restore_device_status(at, devices)
    at.device_run_cmd(device, 'netmgr clear')
    at.device_run_cmd(device, 'kv del wifi')
    at.device_control(device, 'reset')
    time.sleep(3)

    #connect alink
    if withalink:
        at.device_run_cmd(device, 'netmgr connect {0} {1}'.format(ap_ssid, ap_pass))

    #start stability test
    start_time = time.time()
    pass_cnt = 0
    fail_cnt = 0
    fail_num = 0
    succees = False
    while True:
        if (time.time() - start_time) >= duration:
            succeed = True
            break
        if fail_num >= 6:
            succeed = False
            break
        response = at.device_run_cmd(device, 'devname', 1, 0.8, ['device name:'])
        if response == False or len(response) != 1 or 'device name:' not in response[0]:
            if fail_num == 0:
                end_time = time.time()
            fail_num += 1
            fail_cnt += 1
        else:
            fail_num = 0
            pass_cnt += 1
        time.sleep(5)

    at.device_control(device, 'reset')
    at.stop()

    print "pass: {0}, fail: {1}, success_rate: {2:.02f}%".format(pass_cnt, fail_cnt, pass_cnt*100.0/(pass_cnt+fail_cnt))
    if succeed:
        print "Passed statbility test in {0:0.3f} hours".format(duration/3600)
        return [0, 'succeed. duration={0:0.3f} hours'.format(duration/3600) ]
    else:
        duration = end_time - start_time
        print "Failed stability test, device went dead in {0:0.3f} hours".format(duration/3600)
        return [1, 'failed. duration={0:0.3f} hours'.format(duration/3600) ]

if __name__ == '__main__':
    #flush to output immediately
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
    sys.stderr = os.fdopen(sys.stderr.fileno(), 'w', 0)
    [code, msg] = main()
    sys.exit(code)

