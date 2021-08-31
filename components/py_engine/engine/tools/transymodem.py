#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import os, sys, re, codecs, time, json
import argparse
import inspect
from ymodemfile import YModemfile

try:
    import serial
    from serial.tools import miniterm
    from serial.tools.list_ports import comports
except:
    print("\n\nNot found pyserial, please install: \nsudo pip install pyserial")
    sys.exit(0)



def read_json(json_file):
    data = None
    if os.path.isfile(json_file):
        with open(json_file, 'r') as f:
            data = json.load(f)
    return data

def write_json(json_file, data):
    with open(json_file, 'w') as f:
        f.write(json.dumps(data, indent=4, separators=(',', ': ')))


def ymodemTrans(serialport, filename):
    def sender_getc(size):
        return serialport.read(size) or None

    def sender_putc(data, timeout=15):
        return serialport.write(data)

    sender = YModemfile(sender_getc, sender_putc)
    sent = sender.send_file(filename)


def send_check_recv_data(serialport, pattern, timeout):
    """ receive serial data, and check it with pattern """
    matcher = re.compile(pattern)
    tic     = time.time()
    buff    = serialport.read(128)
    while (time.time() - tic) < timeout:
        buff += serialport.read(128)
        if matcher.search(buff):
            return True

    return False


def download_file(portnum, baudrate, filepath):
    # open serial port first
    serialport = serial.Serial()
    serialport.port = portnum
    serialport.baudrate = baudrate
    serialport.parity = "N"
    serialport.bytesize = 8
    serialport.stopbits = 1
    serialport.timeout = 0.05

    try:
        serialport.open()
    except Exception as e:
        raise Exception("Failed to open serial port: %s!" % portnum)

    # send handshark world for check amp boot mode
    mylist = [0xA5]
    checkstatuslist = [0x5A]
    bmatched = False
    shakehand = False
    count = 0
    reboot_count = 0;

    # step 1: check system status
    for i in range(300):
        serialport.write(serial.to_bytes(checkstatuslist))
        time.sleep(0.1)
        buff = serialport.read(2)
        print(buff)
        # case 1: input == output is cli or repl mode
        if((buff) == b'Z'):
            # print('Read data OK');
            reboot_count +=1
        else:
        # not cli or repl mode is running mode
            print("Please reboot the board manually.")
            break;

        if(reboot_count >= 4):
            # need reboot system
            print("Please reboot the board manually.")
            break;

    # step 2: wait reboot and hand shakend cmd
    time.sleep(1)
    bmatched = send_check_recv_data(serialport, b'amp shakehand begin...', 10)
    # print(buff)

    if bmatched:
        print('amp shakehand begin...')

        for i in range(300):
            serialport.write(serial.to_bytes(mylist))
            time.sleep(0.1)
            buff = serialport.read(2)
            print(buff)

            if((buff) == b'Z'):
                # print('Read data OK');
                count +=1

            if(count >= 4):
                shakehand = True

            if shakehand:
                break

            if i > 5:
                print("Please reboot the board manually.")
                break

    else:
        print("Please reboot the board manually, and try it again.")
        serialport.close()
        return

    # start send amp boot cmd
    time.sleep(0.1)
    print("start to send amp_boot cmd")
    cmd = 'amp_boot'
    serialport.write(cmd.encode())
    # serialport.write(b'amp_boot')

    # send file transfer cmd
    time.sleep(0.1)
    print("start to send file cmd")
    cmd = 'cmd_file_transfer\n'
    serialport.write(cmd.encode())
    bmatched = send_check_recv_data(serialport, b'amp shakehand success', 2)
    # serialport.write(b'cmd_flash_js\n')
    
    # send file
    if bmatched:
        print('amp shakehand success')
        time.sleep(0.1)
        ymodemTrans(serialport, filepath)
        print("Ymodem transfer file finish")

        # send file transfer cmd
        time.sleep(0.1)
        print("send cmd exit")
        cmd = 'cmd_exit\n'
        serialport.write(cmd.encode())
    else:
        print('amp shakehand failed, please reboot the boaard manually')

    # close serialport
    serialport.close()


def get_downloadconfig():
    """ get configuration from .config_burn file, if it is not existed, 
        generate default configuration of chip_haas1000 """ 
    configs = {}
    configs['chip_haas1000'] = {}
    configs['chip_haas1000']['serialport'] = ''
    configs['chip_haas1000']['baudrate'] = ''
    configs['chip_haas1000']['filepath'] = ''

    return configs['chip_haas1000']


def main2():
    cmd_parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description='''Run and transfer file to system.''',)

    cmd_parser.add_argument('-d', '--device', default='', help='the serial device or the IP address of the pyboard')
    cmd_parser.add_argument('-b', '--baudrate', default=115200, help='the baud rate of the serial device')
    cmd_parser.add_argument('files', nargs='*', help='input transfer files')

    args = cmd_parser.parse_args()

    print(args)
    # download file
    # step 1: set config
    downloadconfig = get_downloadconfig()
    # step 2: get serial port
    if not downloadconfig["serialport"]:
        downloadconfig["serialport"] = args.device
        if not downloadconfig["serialport"]:
            downloadconfig["serialport"] = miniterm.ask_for_port()
            if not downloadconfig["serialport"]:
                print("no specified serial port")
                return
        else:
            needsave = True

    # step 3: get baudrate
    if not downloadconfig["baudrate"]:
        downloadconfig["baudrate"] = args.baudrate
        if not downloadconfig["baudrate"]:
            downloadconfig["baudrate"] = "115200"

    # step 4: get transfer file
    if not downloadconfig["filepath"]:
        downloadconfig["filepath"] = args.files
        if not downloadconfig["filepath"]:
            print('no file wait to transfer')
            return
    
    if os.path.isabs("".join(downloadconfig["filepath"])):
        filepath = "".join(downloadconfig["filepath"])
        print('the filepath is abs path')
    else:
        basepath = os.path.abspath('.')
        filepath = basepath + '/' + "".join(downloadconfig["filepath"])
        print('the filepath is not abs path')


    print("serial port is %s" % downloadconfig["serialport"])
    print("transfer baudrate is %s" % downloadconfig["baudrate"])
    # print(base_path(downloadconfig["filepath"]))
    print("filepath is %s" % filepath)
    # print("the settings were restored in the file %s" % os.path.join(os.getcwd(), '.config_burn'))
    # step 3: download file
    download_file(downloadconfig["serialport"], downloadconfig['baudrate'], filepath)



if __name__ == "__main__":
    main2()
