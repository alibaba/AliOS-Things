#!/usr/bin/python

# -*- coding:utf-8 -*-


import os
import sys
import optparse
import time
import threading
import os, sys, time
from autotest import Autotest



required_devices = [ [1, 'ywss'] ]
models = {
    'mk3060':'0x13200',
    'esp32':'0x10000',
    'esp8266': '0x1000',
    'starterkit': '0x8000000',
    'developerkit': '0x8000000'
}



class LogMonitor(threading.Thread):
    def __init__(self, phone_id, start_time_str):
        threading.Thread.__init__(self)
        self.phone_id = phone_id
        self.start_time_str = start_time_str

    def run(self):
        os.popen("adb -s " + self.phone_id + " logcat -c")
        log_name  = "wifi_connect_test_adb_log_" + self.phone_id + "_" + self.start_time_str + ".txt"
        logcat_cmd = "adb -s " + self.phone_id + " logcat -v threadtime >> " + log_name
        os.popen(logcat_cmd)



def para_proc(log_file):
    global IOT_DEVICE_INFO
    global IOT_DEVICE_NAME
    global BAUD_RATE
    global TOTAL_COUNT
    global TEST_CASE_LIST
    global DEVICE_MODEL
    global FIRMWARE # image to program onto device
    global TEST_CASE_ID_LIST
    TEST_CASE_ID_LIST = []
    global PHONE_DEVICE_ID_LIST
    PHONE_DEVICE_ID_LIST = []
    global PHONE_MODEL_LIST
    PHONE_MODEL_LIST = []
    global ACCESS_POINT_MODEL_LIST
    ACCESS_POINT_MODEL_LIST = []
    global WIFI_SSID_LIST
    WIFI_SSID_LIST = []
    global WIFI_PASSWORD_LIST
    WIFI_PASSWORD_LIST = []

    parser = optparse.OptionParser()

    parser.add_option('-i', '--iot-device-info', dest='IOT_DEVICE_INFO', default='IoTDevice', help='IoT Device Information')
    parser.add_option('-n', '--iot-device-name', dest='IOT_DEVICE_NAME', default='VNHU6iQgPaBybZTSYQLN', help='IoT Device Name')
    parser.add_option('-b', '--baud-rate', dest='BAUD_RATE', default=921600, help='IoT Device Baud Rate')
    parser.add_option('-c', '--total-count', dest='TOTAL_COUNT', default=3, help='Total Wifi Connect Test Times')
    parser.add_option('-t', '--test-case-list', dest='TEST_CASE_LIST', default='all', help='Test Case List')
    parser.add_option('-m', '--model', dest='DEVICE_MODEL', default='mk3060', help='Device model')
    parser.add_option('-f', '--firmware', dest='FIRMWARE', default='linkkitapp@mk3060-autoywss.bin', help='Firmware image to program onto device')

    (options, args) = parser.parse_args()

    try:
        IOT_DEVICE_INFO = options.IOT_DEVICE_INFO
        IOT_DEVICE_NAME = options.IOT_DEVICE_NAME
        BAUD_RATE = (int) (options.BAUD_RATE)
        TOTAL_COUNT = (int) (options.TOTAL_COUNT)
        TEST_CASE_LIST = options.TEST_CASE_LIST.split(',')
        DEVICE_MODEL = options.DEVICE_MODEL
        FIRMWARE = options.FIRMWARE
    except Exception as e:
        print('invalid argument:{}'.format(e))
        sys.exit(1)

    try:
        cfg_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'autoywss.cfg')
        testbed_cfg_file = open(cfg_file_path, "r")
        testbed_cfg_file_content = testbed_cfg_file.readlines()

        for test_case_index in range(0, len(TEST_CASE_LIST)):
            for line in range(0, len(testbed_cfg_file_content)):
                if testbed_cfg_file_content[line][0] == "#":
                    continue
                else:
                    test_case_item_list = testbed_cfg_file_content[line].split(",")
                    if len(test_case_item_list) < 6:
                        continue
                    else:
                        if test_case_item_list[0].strip() == TEST_CASE_LIST[test_case_index].strip() or \
                            "all" == TEST_CASE_LIST[test_case_index].strip():
                            TEST_CASE_ID_LIST.append(test_case_item_list[0].strip())
                            PHONE_DEVICE_ID_LIST.append(test_case_item_list[1].strip())
                            PHONE_MODEL_LIST.append(test_case_item_list[2].strip())
                            ACCESS_POINT_MODEL_LIST.append(test_case_item_list[3].strip())
                            WIFI_SSID_LIST.append(test_case_item_list[4].strip())
                            WIFI_PASSWORD_LIST.append(test_case_item_list[5].strip())
                            print_write(log_file, testbed_cfg_file_content[line])

        testbed_cfg_file.close()
    except Exception as e:
        print('Exception: {}'.format(e))
        sys.exit(1)



def print_write(file_pointer, str):
    print(str)
    file_pointer.write(str)
    file_pointer.flush()



def device_reset(at, log_file):
    global reset_success_count

    #my_serial.write_cmd_async("reset")
    at.device_run_cmd('A', 'reset')
    print_write(log_file, "Device to reset: reset\n")

    is_reset_finish = False
    time_out_stamp = time.time() + 10
    while True:
        tmp_reset_success_count = 0
        iot_device_file = open("./testlog/" + logname, "r")
        iot_device_file_content = iot_device_file.readlines()
        for line in range(0, len(iot_device_file_content)):
            if "SSID cleared. Please reboot the system" in iot_device_file_content[line]:
                tmp_reset_success_count += 1
            if tmp_reset_success_count > reset_success_count:
                is_reset_finish = True
                reset_success_count += 1
                print_write(log_file, "reset device succeed.\n")
                break
        iot_device_file.close()

        if time.time() > time_out_stamp:
            print_write(log_file, "No reset success log found\n")
            break

        if is_reset_finish:
            time.sleep(5)
            print_write(log_file, "Compete 5s wait after reset\n")
            break
        else:
            time.sleep(0.5)



def device_active_awss(at, log_file):
    global active_awss_success_count

    at.device_run_cmd('A', "active_awss")
    print_write(log_file, "Device into awss mode: active_awss\n")

    is_active_awss_finish = False
    time_out_stamp = time.time() + 10
    while True:
        tmp_active_awss_success_count = 0
        iot_device_file = open("./testlog/" + logname, "r")
        iot_device_file_content = iot_device_file.readlines()
        for line in range(0, len(iot_device_file_content)):
            if "do_awss_active" in iot_device_file_content[line]:
                tmp_active_awss_success_count += 1
            if tmp_active_awss_success_count > active_awss_success_count:
                is_active_awss_finish = True
                active_awss_success_count += 1
                print_write(log_file, "active awss succeed.\n")
                break
        iot_device_file.close()

        if time.time() > time_out_stamp:
            print_write(log_file, "Device failed to find entering awss mode success log,\n")
            break

        if is_active_awss_finish:
            time.sleep(5)
            print_write(log_file, "Complete 5s wait for awss_active\n")
            break
        else:
            time.sleep(0.5)



def app_init_wait(logcat_file_name, num):
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
                print_write(log_file, "App init succeed\n")
                app_init_result = True
                is_init_finish = True
                break
            if "AWSS_RESULT, exit deviceadd activity" in logcat_file_content[line] and app_init_times > app_init_count:
                print_write(log_file, "APP init failed\n")
                app_init_result = False
                is_init_finish = True
                break
        logcat_file.close()

        if time.time() > time_out_stamp:
            print_write(log_file, "APP init timeout\n")
            break

        if is_init_finish:
            break
        else:
            time.sleep(0.5)
    app_init_count = app_init_times

    return app_init_result



def app_connect_wait(logcat_file_name, num):
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
                print_write(log_file, "Smart config succeed\n")
                print_write(log_file, "Finished " + str(num + 1) + " times smart config test, of which " + str(connect_test_success_count) + " times succeed\n")
                break
            if "AWSS_RESULT, exit deviceadd activity" in logcat_file_content[line] and connect_times >= app_init_count:
                connect_result = False
                is_connect_finish = True
                print_write(log_file, "Smart config failed\n")
                print_write(log_file, "Finished " + str(num + 1) + " times smart config test, of which " + str(connect_test_success_count) + " times succeed\n")
                break
        logcat_file.close()

        if time.time() > time_out_stamp:
            print_write(log_file, "90s timeout reached\n")
            print_write(log_file, "Finished " + str(num + 1) + " times smart config, of which " + str(connect_test_success_count) + " times succeed\n")
            break

        if is_connect_finish:
            break
        else:
            time.sleep(0.5)



def allocate_and_program_device():
    #request device allocation
    number, purpose = required_devices[0]
    timeout = 10
    allocated = at.device_allocate(str(DEVICE_MODEL), number, timeout, purpose)
    if len(allocated) != number:
        return 1

    devices={'A':allocated[0]} #construct device list
    print 'allocated:'
    for device in devices:
        print '    {}: {}'.format(device, devices[device])

    #subscribe and reboot devices
    result = at.device_subscribe(devices)
    if result == False:
        print 'subscribe devices failed'
        return 1
    at.device_control('A', 'reset')  #reboot to capture the device (prevent others use it)

    #program devices
    device_list = list(devices)
    device_list.sort()
    dmodel = str(DEVICE_MODEL)
    address = models[dmodel]
    for device in device_list:
        if dmodel in ['stm32l432', 'starterkit', 'developerkit']:
            result = at.device_erase(device)
            if result == False:
                print 'erase device failed'
                return 1

        succeed = False; retry = 5
        for i in range(retry):
            if at.device_program(device, address, str(FIRMWARE)) == True:
                succeed = True
                break
            print 'program device failed, trying once more ...'
            time.sleep(1)
        if succeed == False:
            print 'error: program device {0}:{1} failed'.format(device, devices[device])
            return 1

        time.sleep(2)
        app_start_time = time.time()
        succeed = False; retry = 2
        for i in range(retry):
            if at.device_control(device, 'reset') == True:
                succeed = True
                break
            print 'failed to reset device, trying once more ...'
            time.sleep(1)
        if succeed == False:
            print("device %s reset failed " % allocated[0])
            return 1
        time.sleep(3)
        print 'program succeed'

    return 0



if __name__ == "__main__":
    start_time_stamp = time.time()
    start_time = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(start_time_stamp))
    start_time_str = time.strftime('%Y-%m-%d_%H-%M-%S',time.localtime(start_time_stamp))
    shell_content = os.popen("adb devices | grep -v List | grep device").read()
    device_list = shell_content.split('\n')
    log_file = open("wifi_connect_test_log_" + start_time_str + ".txt", "a+")

    para_proc(log_file)

    server = '11.238.148.46'
    port = 34568
    at=Autotest()
    global logname
    logname = "iot_device_log.txt"
    if at.start(server, port, logname) == False:
        print("Open IoT Device Failed! Exit~")
        sys.exit(1)

    ret = allocate_and_program_device()
    if ret != 0:
        print('Failed to allocate device. Exit~')
        sys.exit(1)

    print_write(log_file, "Open IoT Device: OK\n")

    global reset_success_count
    reset_success_count = 0
    global active_awss_success_count
    active_awss_success_count = 0
    print_write(log_file, "Start IoT Device Logcat...\n\n")

    total_success_cnt = 0

    for test_case_index in range(0, len(TEST_CASE_ID_LIST)):
        print_write(log_file, "\n\n====================Test case ID: " + str(TEST_CASE_ID_LIST[test_case_index]) + ", Testing ...====================\n\n")

        test_case_id = TEST_CASE_ID_LIST[test_case_index]
        phone_device_id = PHONE_DEVICE_ID_LIST[test_case_index]
        phone_model = PHONE_MODEL_LIST[test_case_index]
        access_point_model = ACCESS_POINT_MODEL_LIST[test_case_index]
        wifi_ssid = WIFI_SSID_LIST[test_case_index]
        wifi_password = WIFI_PASSWORD_LIST[test_case_index]
        print_write(log_file, str(test_case_id) + ", " + str(phone_device_id) + ", " + str(phone_model) + ", " + str(access_point_model) + ", " + str(wifi_ssid) + ", " + str(wifi_password) + "\n")

        is_phone_exist = False
        for index in range(0, len(device_list) - 1):
            if phone_device_id in device_list[index]:
                is_phone_exist = True
                break

        if not is_phone_exist:
            print_write(log_file, "In test case ID: " + test_case_id + " no phone found\n\n")
            continue

        ps_ef_content = os.popen("ps -ef | grep 'adb -s " + phone_device_id + "'").read()
        logcat_thread_list = ps_ef_content.split('\n')
        for num in range(0, len(logcat_thread_list) - 1):
            os.popen("kill " + logcat_thread_list[num].split()[1] + " > /dev/null 2>&1")

        log_thread = LogMonitor(phone_device_id, start_time_str + "_" + test_case_id)
        log_thread.start()
        time.sleep(3)
        logcat_file_name = "wifi_connect_test_adb_log_" + phone_device_id + "_" + start_time_str + "_" + test_case_id + ".txt"
        global app_init_count
        app_init_count = 0
        global connect_test_success_count
        connect_test_success_count = 0

        for num in range(0, TOTAL_COUNT):
            # try max TOTAL_COUNT times until success.
            if connect_test_success_count > 0:
                break

            ps_ef_content = os.popen("ps -ef | grep 'adb -s " + phone_device_id + "'").read()
            logcat_thread_list = ps_ef_content.split('\n')
            is_logcat_thread_active = False
            for line in range(0, len(logcat_thread_list) - 1):
                if "adb -s " + phone_device_id + " logcat -v threadtime" in logcat_thread_list[line]:
                    is_logcat_thread_active = True
                    break
            if not is_logcat_thread_active:
                print_write(log_file, "New logcat process\n")
                log_thread = LogMonitor(phone_device_id, start_time_str + "_" + test_case_id)
                log_thread.start()
                time.sleep(3)

            print_write(log_file, "\n\n**********Phone ID: " + str(phone_device_id) + ", No. " + str(num + 1) + " times test**********\n\n")

            result_str = os.popen("adb -s " + phone_device_id + " shell am force-stop com.aliyun.iot.devicetools.iotdevicetools").read()
            print_write(log_file, "Adb force terminate App: force-stop...\n")

            time.sleep(3)
            print_write(log_file, "3s wait finished\n")

            os.popen("adb -s " + phone_device_id + " shell am start -n com.aliyun.iot.devicetools.iotdevicetools/.MainActivity").read()
            print_write(log_file, "Adb start App: start...\n")

            time.sleep(3)
            print_write(log_file, "3s wait finished\n")

            os.popen("adb -s " + phone_device_id + " shell am start -n com.aliyun.iot.devicetools.iotdevicetools/.DeviceAdd.MainDeviceAddActivity --es configs m:ForceAliLinkTypeBroadcast,s:" + str(wifi_ssid) + ",p:" + str(wifi_password) + "").read()
            print_write(log_file, "Adb distribute config, init APP: start, configs...\n")

            if not app_init_wait(logcat_file_name, num):
                continue

            device_reset(at, log_file)

            device_active_awss(at, log_file)

            os.popen("adb -s " + phone_device_id + " shell am broadcast -a startdeviceadd --es devicename " + str(IOT_DEVICE_NAME)).read()
            print_write(log_file, "APP entering awss: broadcast\n")

            app_connect_wait(logcat_file_name, num)

            device_reset(at, log_file)

            at.device_run_cmd('A', "reboot")
            print_write(log_file, "Device to reboot: reboot\n")

            result_str = os.popen("adb -s " + phone_device_id + " shell am force-stop com.aliyun.iot.devicetools.iotdevicetools").read()
            print_write(log_file, "Adb force terminate App: force-stop...\n")

            time.sleep(10)
            print_write(log_file, "10s wait finished\n\n\n\n\n")

            print '---------- The adb test log start ----------'
            adb_file = open(logcat_file_name)
            line = adb_file.readline()
            while line:
                print line
                line = adb_file.readline()
            print '----------  The adb test log end  ----------'
            adb_file.close()

        # ro.build.fingerprint=Coolpad/Coolpad7295A/Coolpad7295A:4.1.2/JZO54K/4.1.029.P0.141219.7295A:user/release-keys
        try:
            cmd_fp = os.popen("adb -s " + phone_device_id + " shell cat /system/build.prop | grep \"ro.build.fingerprint\"")
            fp = cmd_fp.read()
            fpl = fp.split("/")

            # ro.product.model=Coolpad 7295A
            cmd_pro = os.popen("adb -s " + phone_device_id + " shell cat /system/build.prop | grep \"ro.product.model\"")
            pro = cmd_pro.read()


            device_brand = fpl[0].split("=")[1]
            android_version = fpl[2].split(":")[1]
            device_type = pro.split("\n")[0].split("=")[1].split("\r")[0]
        except Exception as e:
            device_brand = "Null"
            android_version = "Null"
            device_type = "Null"
            print_write("Failed to obtain phone info\n")

        ps_ef_content = os.popen("ps -ef | grep 'adb -s " + phone_device_id + "'").read()
        logcat_thread_list = ps_ef_content.split('\n')
        for num in range(0, len(logcat_thread_list) - 1):
            os.popen("kill " + logcat_thread_list[num].split()[1] + " > /dev/null 2>&1")

        if connect_test_success_count > 0:
            total_success_cnt += 1
            print("====================Test case ID: " + str(TEST_CASE_ID_LIST[test_case_index]) + ", Succeed====================\n\n")
        else:
            print("====================Test case ID: " + str(TEST_CASE_ID_LIST[test_case_index]) + ", Failed====================\n\n")

    at.stop()

    print '---------- The device log start ----------'
    with open("./testlog/" + logname) as f:
        print(f.read())
    print '----------  The device log end  ----------'

    os.popen("rm -f wifi_connect_test_adb_log_*")
    os.popen("rm -f wifi_connect_test_log_*")

    test_case_cnt = len(TEST_CASE_ID_LIST)
    print("\n======Test summary:======\n")
    if total_success_cnt == test_case_cnt:
        print("    All test cases passed.")
        print("    !!!Test SUCCEED!!!\n")
        sys.exit(0)
    else:
        print("    %d out of %d test cases failed." % (test_case_cnt - total_success_cnt, test_case_cnt))
        print("\n    !!!Test FAILED!!!\n")
        sys.exit(1)
