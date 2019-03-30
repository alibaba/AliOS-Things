import os, sys, time
from autotest import Autotest

required_devices = [ [1, 'general'] ]
models = {
    'mk3060':'0x13200',
    'esp32':'0x10000',
    'esp8266': '0x1000'
}

def main(firmware='coapapp@esp32-general.bin', model='esp32'):
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
            firmware = args[1]
        elif arg.startswith('--model='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --model=mk3060'.format(arg)
            model = args[1]
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
        elif arg.startswith('--wifissid='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --wifissid=test_wifi'.format(arg)
            ap_ssid = args[1]
        elif arg.startswith('--wifipass='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --wifipass=test_password'.format(arg)
            ap_pass = args[1]
        elif arg=='--help':
            print 'Usage: python {0} [--firmware=xxx.bin] [--wifissid=wifi_ssid] [--wifipass=password]'.format(sys.argv[0])
            return [0, 'help']
        i += 1

    if model not in models:
        print "error: unsupported device model {0}".format(repr(model))
        return [1, 'unsupported model {}'.format(model)]

    at=Autotest()
    logname=time.strftime('%Y-%m-%d@%H-%M')
    logname = 'mqtt_test-' + logname +'.log'
    if at.start(server, port, logname) == False:
        return [1, 'connect testbed failed']

    #request device allocation
    number, purpose = required_devices[0]
    timeout = 10
    allocated = at.device_allocate(model, number, timeout, purpose)
    if len(allocated) != number:
        return [1, 'allocate device failed']

    devices={'A':allocated[0]} #construct device list
    print 'allocated:'
    for device in devices:
        print '    {}: {}'.format(device, devices[device])

    #subscribe and reboot devices
    result = at.device_subscribe(devices)
    if result == False:
        print 'subscribe devices failed'
        return [1, 'subscribe devices failed']
    at.device_control('A', 'reset')  #reboot to capture the device (prevent others use it)

    #program devices
    device_list = list(devices)
    device_list.sort()
    address = models[model]
    for device in device_list:
        result = at.device_program(device, address, firmware)
        if result == False:
            print 'program device failed'
            return [1, 'program device failed']
        time.sleep(2)
        app_start_time = time.time()
        result = at.device_control(device, 'reset')
        if result == False:
            print("device %s reset failed " % allocated[0])
            return [1, 'reset device failed']
        time.sleep(3)
        print 'program succeed'

    #connect device to alibaba cloud
    at.device_run_cmd('A', 'netmgr clear')  #run 'netmgr clear' command at device A
    time.sleep(0.5)
    at.device_control('A', 'reset')              #control device A, let it reboot
    time.sleep(5)                                #wait some time
    at.device_run_cmd('A', 'netmgr connect {0} {1}'.format(ap_ssid, ap_pass)) #connect device A to wifi
    time.sleep(20)                               #wait some time

    number_of_responses = 50
    timeout = 100
    filter = ['aos framework init', 'send msg: Len:', 'Message response code: 69']
    responses = at.device_read_log('A', number_of_responses, timeout, filter)

    #disconnect device from alibaba cloud
    at.device_run_cmd('A', 'netmgr clear')  #run 'netmgr clear' command at device A
    time.sleep(0.5)
    at.device_control('A', 'reset')              #control device A, let it reboot
    at.stop()

    ret = 0; result = 'succeed'
    if len(responses) < number_of_responses:
        print 'error: can not get {} lines of log containing {}'.format(number_of_responses, filter)
        ret = 1; result = 'failed'

    for response in responses:
        if 'aos framework init' not in response:
            continue
        print 'error: device restart detected'
        ret = 1; result = 'failed'
        break

    print '\nlogs:'
    if result == 'succeed':
        for response in responses: print response
    else:
        flog = open('testlog/' + logname, 'r')
        line = flog.readline()
        while line:
            logtime = line.split(':')[0]
            sec_int, sec_frac = logtime.split('.')
            sec_int = time.mktime(time.strptime(sec_int, "%Y-%m-%d %H-%M-%S"))
            sec_frac = float('0.' + sec_frac)
            logtime = sec_int + sec_frac
            if logtime >= app_start_time:
                print line,
            line = flog.readline()

    print '\ncoap test {}'.format(result)
    return [ret, result]

if __name__ == '__main__':
    [code, msg] = main()
    sys.exit(code)
