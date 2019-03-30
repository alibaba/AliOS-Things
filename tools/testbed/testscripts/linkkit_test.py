import sys, os, time, httplib, json, subprocess, pdb,  getpass

import logging
import jpype

from autotest import Autotest

models={'mk3060':'0x13200'}
testnames={'short2pps':0}

DEBUG = True




def start_jvm():
    parent_dir = os.path.dirname((os.path.abspath(__file__)))
    print(parent_dir)
    jar_path = os.path.join(parent_dir, "device.jar")
    print(jar_path)
    print(jpype.getDefaultJVMPath())


    if not jpype.isJVMStarted():
        jpype.startJVM(jpype.getDefaultJVMPath(), "-Djava.class.path={}".format(jar_path))

        print(jpype.getDefaultJVMPath())


def attach_jvm():
    if not jpype.isThreadAttachedToJVM():
        jpype.attachThreadToJVM()

def detach_jvm():
    if jpype.isThreadAttachedToJVM():
        jpype.detachThreadFromJVM()


def stop_jvm():
    detach_jvm()
    if jpype.isJVMStarted():
        jpype.shutdownJVM()


#main function
def main(firmware='linkkitapp@mk3060-general.bin', model='mk3060', testname='short2pps'):
    global DEBUG
    userid = '500001169232518525'
    linkkit_test_server = 'pre-iotx-qs.alibaba.com'
    server = '11.238.148.46'
    #server = '116.62.245.240'
    port   = 34568
    wifissid = 'aos_test_01'
    wifipass = 'Alios@Embedded'

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
        elif arg.startswith('--testname='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --testname=5pps'.format(arg)
            testname = args[1]
        elif arg.startswith('--userid='):
            args = arg.split('=')
            if len(args) != 2 or args[1].isdigit() == False:
                print 'wrong argument {0} input, example: --userid=123456789012345678'.format(arg)
            userid = args[1]
        elif arg.startswith('--server='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --server=192.168.10.16'.format(arg)
            server = args[1]
        elif arg.startswith('--port='):
            args = arg.split('=')
            if len(args) != 2 or args[1].isdigit() == False:
                print 'wrong argument {0} input, example: --port=34568'.format(arg)
            port = int(args[1])
        elif arg.startswith('--wifissid='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --wifissid=test_wifi'.format(arg)
            wifissid = args[1]
        elif arg.startswith('--wifipass='):
            args = arg.split('=')
            if len(args) != 2:
                print 'wrong argument {0} input, example: --wifipass=test_password'.format(arg)
            wifipass = args[1]
        elif arg.startswith('--debug='):
            args = arg.split('=')
            if len(args) != 2 or args[1].isdigit() == False:
                print 'wrong argument {0} input, example: --debug=1'.format(arg)
            DEBUG = (args[1] != '0')
        elif arg=='--help':
            print 'Usage: python {0} [--firmware=xxx.bin] [--model=xxx] [--testname=xxxx] [--userid=xxxxx] [--server=xx.x.x.x] [--port=xx] [--wifissid=wifi_ssid] [--wifipass=password] [--debug=0/1]'.format(sys.argv[0])
            return [0, 'help']
        i += 1

    if DEBUG:
        print "firmware: {0}".format(firmware)
        print "model: {0}".format(model)
        print "testname: {0}".format(testname)
        print "userid: {0}".format(userid)
        print "server: {0}".format(server)
        print "port: {0}".format(port)
        print "wifissid: {0}".format(wifissid)
        print "wifipass: {0}".format(wifipass)

    if testname not in testnames:
        print "error: unsupported testname {0}".format(repr(testname))
        return [1, 'testname {0} unsupported'.format(repr(testname))]

    if not model or model.lower() not in models:
        print "error: unsupported model {0}".format(repr(model))
        return [1, 'model {0} unsupported'.format(repr(model))]
    model = model.lower()

    logname=time.strftime('-%Y-%m-%d@%H-%M')
    logname = testname + logname +'.log'
    at=Autotest()

    print "at.start === : {0}".format(server)
    if at.start(server, port, logname) == False:
        print 'error: start failed'
        return [1, 'connect testbed failed']


    print "at.ok === : {0}".format(server)
    number = 1

    timeout = 120
    allocted = at.device_allocate(model, number, timeout, 'alink')
    if len(allocted) != number:
        print "error: request device allocation failed"
        return [1, 'allocate device failed']
    print "allocted device", allocted[0]

    devices = {'A':allocted[0]}
    device = 'A'

    #subscribe device
    if at.device_subscribe(devices) == False:
        print 'error: subscribe to device failed, some devices may not exist in testbed'
        return [1, 'subscribe device failed']

    #program device
    succeed = False; retry = 5
    addr = models[model]
    print 'programming device {0} ...'.format(devices[device])
    for i in range(retry):
        print "firmware = {0}".format(firmware)
        if at.device_program(device, addr, firmware) == True:
            succeed = True
            break
        time.sleep(0.5)
    if succeed == False:
        print 'error: program device {0} failed'.format(devices[device])
        return [1, 'program device failed']
    print 'program device {0} succeed'.format(devices[device])
    time.sleep(5)

    #connect to linkkit
    succeed = False; retry = 5
    key_name = None
    uuid = None
    startflag=0

    while retry > 0:
        #clear previous setting and reboot
        #at.device_run_cmd(device, 'kv del wifi')
  
        #at.device_run_cmd(device, 'netmgr clear')
        time.sleep(2)
        at.device_control(device, 'reset')

        time.sleep(5)
        netmgr=2
        while(netmgr > 0):
            print "after reset"
            #connect device to linkkit
            at.device_run_cmd(device, 'netmgr connect {0} {1}'.format(wifissid, wifipass), timeout=1.5)
            netmgr-=1
            time.sleep(2)
			
        print "after connect"
        #at.device_control(device, 'reboot')
        time.sleep(20)
        print "after reboot"

        loop = 5
        while (loop > 0):
            filter = ['KEY_NAME:']
            response = at.device_run_cmd(device, 'prtkey', 1, 5.5, filter)

            print "after prtkey {0}".format(response)
            lenresp = len(response)
            if response == False or len(response) != 1:
                loop -= 1
                continue
            key_name = response[0].split()[-1]
            print " key_name is : {0}".format(key_name)

            productKey = key_name.split('.',2)[0]
            deviceName = key_name.split('.',2)[1]

            print " productKey is :{0}".format(productKey)
            print " deviceName is :{0}".format(deviceName)

            print "connect linkkit succeed, key_name:{0}".format(key_name)
            startflag = 1
            break

        retry-=1
        if startflag == 0:
            continue;
			
        time.sleep(2)	
        thing_crt_flag = 0
        thing_check = 10
        while (thing_check > 0):
            filter = ['THING_ID_CREAT']
            response = at.device_run_cmd(device, 'prtkey', 1, 2.5, filter)
            print "after prtkey {0}".format(response)
            if response == False or len(response) != 1:
                thing_check -= 1
                time.sleep(2)
                continue
            thing_crt_flag = 1
            break

        if(thing_crt_flag == 0):
            print "thing creat fail"
            continue;
        else:
            break;

    if(startflag == 0):
        print "device start fail"
        time.sleep(2)
        at.device_control(device, 'reset')
        time.sleep(2)
        return [1, 'device start fail']
    if(thing_crt_flag == 0):
        print "device thing creat fail"
        time.sleep(2)
        at.device_control(device, 'reset')
        time.sleep(2)
        return [1, 'device thing creat fail']

    sys_usr=getpass.getuser()	
    print("user",sys_usr)
	
    if sys_usr == "iot":
        os.environ['JAVA_HOME'] = '/usr/local/jdk1.8.0_151'
    if sys_usr == "lc122798":
        os.environ['JAVA_HOME'] = '/home/lc122798/tools/jdk1.8.0_151'

    time.sleep(5)
    start_jvm()
    attach_jvm()
    print(jpype.getDefaultJVMPath())

    MyDevice = jpype.JClass('MyDevice')
    jardevice = MyDevice(productKey, deviceName)



    time.sleep(10)
    loopcnt = 100
    successcnt = loopcnt
    while (loopcnt > 0):
        print("start setProperty")
        if jardevice.setProperty('LightSwitch', 1):
            time.sleep(0.5)
        else:
            time.sleep(1)
            continue
        loopcnt-= 1
        cycle_ms = 1
        #time.sleep(cycle_ms)

    time.sleep(60)

    retry = 100
    recordcnt = 0
    while (retry > 0):
        filter = ['PROPERTY_SET_COUNT::']
        response = at.device_run_cmd(device, 'prtkey', 1, 2.5, filter)
        print "after prtkey {0}".format(response)
        if response == False or len(response) != 1:
            retry -= 1
            time.sleep(2)
            continue
        returnstr = response[0].split()[-1]
        print " returnstr is : {0}".format(returnstr)
        recordcnt=int(returnstr)
        print("out int",recordcnt)
        break

    jpype.detachThreadFromJVM()
    time.sleep(2)
    at.device_control(device, 'reset')
    time.sleep(2)
    print("out put",successcnt,recordcnt)
    if(successcnt == recordcnt):
        print "linkkit success"
        return [0, 'linkkit success']
    else:
        print "linkkit fail"
        return [1, 'linkkit fail']


if __name__ == '__main__':
    #flush to output immediately
    sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
    sys.stderr = os.fdopen(sys.stderr.fileno(), 'w', 0)
    [code, msg] = main()
    sys.exit(code)

