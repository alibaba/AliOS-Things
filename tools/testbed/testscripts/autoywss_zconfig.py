#!/usr/bin/python

import os, sys, optparse, time, threading
from autotest import Autotest


dev_registrar = 'registrar'
dev_enrollee = 'enrollee'
required_devices = [ [2, 'zywss'] ]
models = {
    'mk3060':'0x13200'
}


class LogMonitor(threading.Thread):
    def __init__(self, phone_id, log_file_name):
        threading.Thread.__init__(self)
        self.phone_id = phone_id
        self.log_file_name = log_file_name

    def run(self):
        os.popen("adb -s " + self.phone_id + " logcat -c")
        logcat_cmd = "adb -s " + self.phone_id + " logcat -v threadtime >> " + self.log_file_name
        os.popen(logcat_cmd)


def parse_arg():
    global device_model
    global fw_registrar
    global fw_enrollee
    global iot_device_name
    global wifi_ssid
    global wifi_password
    
    parser = optparse.OptionParser()
    parser.add_option('-m', '--model', dest='device_model', default='mk3060', help='Device model')
    parser.add_option('-f', '--firmware', dest='firmware', default='linkkitapp@mk3060-autoywss.bin', help='Firmware file to be programmed into device')
    parser.add_option('-e', '--firmware-enrollee', dest='firmware_enrollee', default='linkkitapp@mk3060-enrollee.bin', help='Firmware file to be programmed into device')
    parser.add_option('-n', '--iot-device-name', dest='iot_dev_name', default='VNHU6iQgPaBybZTSYQLN', help='IoT device name')
    parser.add_option('-s', '--wifissid', dest='wifi_ssid', default='Yuemewifi-3766', help='WiFi SSID')
    parser.add_option('-p', '--wifipass', dest='wifi_pw', default='aos12345', help='WiFi password')

    (options, args) = parser.parse_args()
    try:
        device_model = options.device_model
        fw_registrar = options.firmware
        fw_enrollee = options.firmware_enrollee
        iot_device_name = options.iot_dev_name
        wifi_ssid = options.wifi_ssid
        wifi_password = options.wifi_pw
    except Exception as e:
        print('invalid argument:{}'.format(e))
        sys.exit(1)


def program_device(atobj, dmodel, devices, dev, addr, fw):
    if dmodel in ['stm32', 'starterkit', 'developerkit']:
        result = atobj.device_erase(dev)
        if result == False:
            print 'erase device failed'
            return 1

    succeed = False; retry = 5
    for i in range(retry):
        if atobj.device_program(dev, addr, fw) == True:
            succeed = True
            break
        print 'program device failed, trying once more ...'
        time.sleep(1)
    if succeed == False:
        print 'error: program device {0}:{1} failed'.format(dev, devices[dev])
        return False

    time.sleep(2)
    app_start_time = time.time()
    succeed = False; retry = 2
    for i in range(retry):
        if atobj.device_control(dev, 'reset') == True:
            succeed = True
            break
        print 'failed to reset device, trying once more ...'
        time.sleep(1)
    if succeed == False:
        print("error: reset device {0}:{1} failed".format(dev, devices[dev]))
        return False

    time.sleep(3)
    print 'program succeed'

    return True


def allocate_and_program_device(at, dmodel):
    #request device allocation
    number, purpose = required_devices[0]
    timeout = 10
    allocated = at.device_allocate(str(dmodel), number, timeout, purpose)
    if len(allocated) != number:
        return 1

    global zconfig_devices

    zconfig_devices = {dev_registrar:allocated[0], dev_enrollee:allocated[1]} #construct device list
    print 'allocated:'
    for device in zconfig_devices:
        print '    {}: {}'.format(device, zconfig_devices[device])

    #subscribe and reboot devices
    result = at.device_subscribe(zconfig_devices)
    if result == False:
        print 'subscribe devices failed'
        return 1
    at.device_control(dev_registrar, 'reset')  #reboot to capture the device (prevent others use it)
    at.device_control(dev_enrollee, 'reset')  #reboot to capture the device (prevent others use it)

    if (program_device(at, dmodel, zconfig_devices, dev_registrar, models[dmodel], fw_registrar) == False or 
        program_device(at, dmodel, zconfig_devices, dev_enrollee, models[dmodel], fw_enrollee) == False):
        return 1

    return 0


def print_device_log():
    print '---------- The device log start ----------'
    with open("./testlog/" + dev_log_name) as f:
        print(f.read())
    print '----------  The device log end  ----------'


def print_test_summary(message):
    print('\n=====Test Summary:=====\n')
    print("    !!!" + str(message + "!!!\n"))


def go_exit(at, exit_code):
    at.stop()
    print('AT stopped.')

    os.popen("rm -f wifi_connect_test_adb_log_*")
    os.popen("rm -f wifi_connect_test_log_*")

    print_device_log()

    if exit_code == 0:
        summary_str = 'SUCCESS'
    else:
        summary_str = 'FAILED'

    print_test_summary(summary_str)

    print('We are going to exit, the adb process for now:')
    print(os.popen("ps -elf|grep adb").read())

    sys.exit(exit_code)


def get_first_availble_phone_id():
    phone_id = None
    adb_devices = os.popen("adb devices | grep -v List | grep device").read()

    if adb_devices is not None:
        phone_id = ((adb_devices.split('\n'))[0].split('\t'))[0]

    return phone_id


def kill_adb_thread(pid):
    ps_ef_content = os.popen("ps -ef | grep 'adb -s " + pid + "'").read()
    logcat_thread_list = ps_ef_content.split('\n')
    for num in range(0, len(logcat_thread_list) - 1):
        os.popen("kill " + logcat_thread_list[num].split()[1] + " > /dev/null 2>&1")


def app_init_wait(logcat_file_name):
    global app_init_count
    is_init_finish = False
    app_init_result = False
    time_out_stamp = time.time() + 90
    while True:
        app_init_times = 0
        logcat_file = open(logcat_file_name, "r")
        logcat_file_content = logcat_file.readlines()
        for line in range(0, len(logcat_file_content)):
            if "AWSS_CFG,mode:" in logcat_file_content[line]:
                app_init_times += 1
            if "AWSS_RESULT, ready to start" in logcat_file_content[line] and app_init_times > app_init_count:
                print("App init succeed.")
                app_init_result = True
                is_init_finish = True
                break
            if "AWSS_RESULT, exit deviceadd activity" in logcat_file_content[line]:
                print("APP init failed.")
                app_init_result = False
                is_init_finish = True
                break
        logcat_file.close()

        if time.time() > time_out_stamp:
            print("APP init timeout.")
            break

        if is_init_finish:
            break
        else:
            time.sleep(0.5)
    app_init_count = app_init_times

    return app_init_result


def reset_device(at, device, log_file):
    global reset_success_count

    print("Device to reset: " + device)
    at.device_run_cmd(device, 'reset')

    is_reset_finish = False
    time_out_stamp = time.time() + 10
    while True:
        tmp_reset_success_count = 0
        iot_device_file = open(log_file, "r")
        iot_device_file_content = iot_device_file.readlines()
        for line in range(0, len(iot_device_file_content)):
            to_match1 = ':' + device + ':'
            to_match2 = 'SSID cleared. Please reboot the system'
            if to_match2 in iot_device_file_content[line]:
                tmp_reset_success_count += 1
            if tmp_reset_success_count > reset_success_count and to_match1 in iot_device_file_content[line]:
                is_reset_finish = True
                reset_success_count += 1
                print("reset device succeed.\n")
                break
        iot_device_file.close()

        if time.time() > time_out_stamp:
            print("No reset success log found\n")
            break

        if is_reset_finish:
            time.sleep(5)
            print("Compete 5s wait after reset\n")
            break
        else:
            time.sleep(0.5)

    return is_reset_finish;


def do_active_awss(at, device, log_file):
    global active_awss_success_count

    at.device_run_cmd(device, "active_awss")
    print("Device into awss mode: active_awss.")

    is_active_awss_finish = False
    time_out_stamp = time.time() + 10
    while True:
        tmp_active_awss_success_count = 0
        iot_device_file = open(log_file, "r")
        iot_device_file_content = iot_device_file.readlines()
        for line in range(0, len(iot_device_file_content)):
            to_match1 = ':' + device + ':'
            to_match2 = 'do_awss_active'
            if to_match1 in iot_device_file_content[line] and to_match2 in iot_device_file_content[line]:
                tmp_active_awss_success_count += 1
            if tmp_active_awss_success_count > active_awss_success_count:
                is_active_awss_finish = True
                active_awss_success_count += 1
                print("active awss succeed.")
                break
        iot_device_file.close()

        if time.time() > time_out_stamp:
            print("Device failed to find entering awss mode success log.")
            break

        if is_active_awss_finish:
            time.sleep(5)
            print("Complete 5s wait for awss_active.")
            break
        else:
            time.sleep(0.5)

    return is_active_awss_finish


def app_connect_wait(logcat_file_name):
    global app_init_count
    global connect_test_success_count
    is_connect_finish = False
    connect_result = False
    time_out_stamp = time.time() + 90
    while True:
        connect_times = 0
        logcat_file = open(logcat_file_name, "r")
        logcat_file_content = logcat_file.readlines()
        for line in range(0, len(logcat_file_content)):
            if "AWSS_CFG,mode:" in logcat_file_content[line]:
                connect_times += 1
            if "AWSS_RESULT, bind success" in logcat_file_content[line] and connect_times >= app_init_count:
                connect_result = True
                is_connect_finish = True
                connect_test_success_count += 1
                print("Smart config succeed.")
                break
            if "AWSS_RESULT, exit deviceadd activity" in logcat_file_content[line] and connect_times >= app_init_count:
                connect_result = False
                is_connect_finish = True
                print("Smart config failed.")
                break
        logcat_file.close()

        if time.time() > time_out_stamp:
            print("90s timeout reached\n")
            break

        if is_connect_finish:
            break
        else:
            time.sleep(0.5)

    return connect_result


def do_smart_config(at, dev, dev_log, ssid, pw):
    ret = {"code":1, "id":None}

    phone_id = get_first_availble_phone_id()
    if phone_id is None:
        print('Failed to find test phone.')
        return ret
    else:
        ret["id"] = phone_id
        print("Phone found, id is %s." % phone_id)

    kill_adb_thread(phone_id)

    print("Starting logcat thread ...")
    time_stamp = time.time()
    time_str = time.strftime('%Y-%m-%d_%H-%M-%S', time.localtime(time_stamp))
    adb_log_name = 'wifi_connect_test_adb_log_' + phone_id + time_str + '.txt'
    log_thread = LogMonitor(phone_id, adb_log_name)
    log_thread.start()

    time.sleep(3)

    ps_ef_content = os.popen("ps -ef | grep 'adb -s " + phone_id + "'").read()
    logcat_thread_list = ps_ef_content.split('\n')
    is_logcat_thread_active = False
    for line in range(0, len(logcat_thread_list) - 1):
        if "adb -s " + phone_id + " logcat -v threadtime" in logcat_thread_list[line]:
            is_logcat_thread_active = True
            break

    if is_logcat_thread_active == False:
        print('Failed to start logcat thread.')
        return ret
    else:
        print("Logcat thread started.")

    os.popen("adb -s " + phone_id + " shell am force-stop com.aliyun.iot.devicetools.iotdevicetools")
    print("Adb force terminate App: force-stop ...")

    time.sleep(3)

    os.popen("adb -s " + phone_id + " shell am start -n com.aliyun.iot.devicetools.iotdevicetools/.MainActivity")
    print("Adb start App: start ...\n")

    time.sleep(3)

    os.popen("adb -s " + phone_id + " shell am start -n com.aliyun.iot.devicetools.iotdevicetools/.DeviceAdd.MainDeviceAddActivity --es configs m:ForceAliLinkTypeBroadcast,s:" + str(ssid) + ",p:" + str(pw) + "")
    print("Adb distribute config, init APP: start, configs ...\n")

    if not app_init_wait(adb_log_name):
        print("Adb app failed to be started.")
        return ret

    if not reset_device(at, dev, dev_log):
        print("Failed to reset device.")
        return ret

    if not do_active_awss(at, dev, dev_log):
        print("Failed to do active awss.")
        return ret

    os.popen("adb -s " + phone_id + " shell am broadcast -a startdeviceadd --es devicename " + str(iot_device_name))
    print("APP entering awss: broadcast.")

    if not app_connect_wait(adb_log_name):
        print("Adb app failed to be connected.")
        return ret

    print("APP smart config completed.")

    ret["code"] = 0
    return ret


def exit_smart_config(at, dev, pid):
    print('Exiting smart config for device: ' + dev)
    print("Resetting device ...")
    at.device_run_cmd(dev, "reset")

    time.sleep(1)

    os.popen("adb -s " + pid + " shell am force-stop com.aliyun.iot.devicetools.iotdevicetools")
    print("Adb force terminate App: force-stop ...")

    ps_ef_content = os.popen("ps -ef | grep 'adb -s " + pid + "'").read()
    logcat_thread_list = ps_ef_content.split('\n')
    for num in range(0, len(logcat_thread_list) - 1):
       os.popen("kill " + logcat_thread_list[num].split()[1] + " > /dev/null 2>&1")
    print('Adb logcat thread killed.')

    time.sleep(10)
    print("10s wait finished.")
    print('Finished exiting smart config for device: ' + dev)


def is_zconfig_success(at, reg_dev, enr_dname, log_file):
    enrolle_found = False
    iot_device_file = open(log_file, "r")
    iot_device_file_content = iot_device_file.readlines()
    for line in range(0, len(iot_device_file_content)):
        to_match1 = ':' + reg_dev + ':'
        to_match2 = 'new enrollee['
        if to_match1 in iot_device_file_content[line] and \
           to_match2 in iot_device_file_content[line] and \
            str(enr_dname) in iot_device_file_content[line]:
            enrolle_found = True
            print('Enrolle found OK.')
            print(iot_device_file_content[line])
            break

    return enrolle_found


if __name__ == "__main__":
    global reset_success_count
    reset_success_count = 0
    global active_awss_success_count
    active_awss_success_count = 0
    global app_init_count
    app_init_count = 0
    global connect_test_success_count
    connect_test_success_count = 0
    global device_model
    global fw_registrar
    global fw_enrollee
    global iot_device_name
    global wifi_ssid
    global wifi_password
    enrollee_dname = 'yo2zSNU51SgbXh9x4y7o'

    parse_arg()

    server = '11.238.148.46'
    port = 34568
    at=Autotest()
    global dev_log_name
    dev_log_name = "iot_device_log.txt"
    if at.start(server, port, dev_log_name) == False:
        print("Open IoT Device Failed! Exit~")
        sys.exit(1)

    ret = allocate_and_program_device(at, device_model)
    if ret != 0:
        print('Failed to allocate/program device. Exit~')
        go_exit(at, 1)

    print("Open IoT Device: OK\n")

    sconfig_ret = do_smart_config(at, dev_registrar, "./testlog/" + dev_log_name, wifi_ssid, wifi_password)
    if sconfig_ret["code"] != 0:
        print('Smart config for registrar failed! Exit~')
        if sconfig_ret["id"] is not None:
            exit_smart_config(at, dev_registrar, sconfig_ret["id"])
        go_exit(at, 1)
    else:
        print('Registrar smart config completed.')

    if reset_device(at, dev_enrollee, "./testlog/" + dev_log_name) == False:
        print('Enrollee reset failed, Exit!')
        if sconfig_ret["id"] is not None:
            exit_smart_config(at, dev_registrar, sconfig_ret["id"])
        go_exit(at, 1)
    print('Reset enrollee device succeed.')

    time.sleep(10)
    print('10s wait finished for enrollee to be found.')

    zconfig_fail = 0
    if is_zconfig_success(at, dev_registrar, enrollee_dname, "./testlog/" + dev_log_name) == False:
        print('Zconfig result check failed, Exit!')
        zconfig_fail = 1
    else:
        print('Zconfig test succeed.')

    exit_smart_config(at, dev_registrar, sconfig_ret["id"])
    print('Registrar smart config exit.')

    print('The test result is ' + str(zconfig_fail))

    go_exit(at, zconfig_fail)
