#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Add by HaaS
# version = 1.1.1

import os, sys, re, codecs, time, json
import argparse
import inspect
from ymodemfile import YModemfile

import logging

logging.basicConfig(level=logging.INFO,
                    format='%(filename)s[line:%(lineno)d]: %(message)s')

try:
    import serial
    from serial.tools import miniterm
    from serial.tools.list_ports import comports
except:
    logging.debug("\n\nNot found pyserial, please install:\nsudo pip install pyserial")
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


def received_data_check(pattern, buff):
    matcher = re.compile(pattern)
    if matcher.search(buff):
        return True

    return False

def send_check_recv_data_count_down(serialport, pattern, timeout, countUnit):
    """ receive serial data, and check it with pattern """
    matcher = re.compile(pattern)
    tic     = time.time()
    cnt     = 0
    unit    = countUnit
    buff    = serialport.read(128)
    currentTic = time.time()
    while (currentTic - tic) < timeout:
        # show count down
        if((currentTic - tic) > unit):
            cnt = unit
            unit = countUnit + unit
            print("*****", (timeout-cnt), "*****")

        buff += serialport.read(128)
        if matcher.search(buff):
            print("")
            return True
        # reset currentTic
        currentTic = time.time()

    # give more 5S to read buffer
    while (time.time() - tic) < (timeout + 10):
        buff += serialport.read(128)
        if matcher.search(buff):
            print("")
            return True
    print("")
    return False

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

def file_transfer_handshake_function(serialport):
    count = 0
    handshake_data = [0xA5]
    shakehand_result = False
    print("***** handsharking *****")
    print("")

    for i in range(300):
        serialport.write(serial.to_bytes(handshake_data))
        time.sleep(0.1)
        buff = serialport.read(2)
        # logging.debug(buff)

        if((buff) == b'Z'):
            # logging.debug('Read data OK');
            count +=1

        if(count >= 4):
            shakehand_result = True

        if shakehand_result:
            break

        if i > 5:
            logging.debug("Please reboot the board manually.")
            break

    return shakehand_result


def download_file(portnum, baudrate, filepath, devpath):
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
    checkstatuslist = [0x5A]
    bmatched = False
    shakehand = False
    reboot_cmd_cli = 'reboot()\n'
    reboot_cmd = 'reboot\n'
    reboot_count = 0;
    reboot_method = 0;

    # step 1: check system status
    for i in range(300):
        serialport.write(serial.to_bytes(checkstatuslist))
        time.sleep(0.1)
        buff = serialport.read(2)
        # logging.debug(buff)
        # case 1: input == output is cli or repl mode
        if((buff) == b'Z'):
            # logging.debug('Read data OK');
            reboot_count +=1
        else:
        # not cli or repl mode is running mode
            # reboot_method = 1;
            time.sleep(0.2)
            serialport.write(reboot_cmd_cli.encode())
            time.sleep(0.2)
            serialport.write(reboot_cmd.encode())
            break;

        if(reboot_count >= 4):
            # need reboot system
            # send Enter CMD
            # print("send enter")
            time.sleep(0.2)
            # send a cmd
            serialport.write(b'a\r\n')

            time.sleep(0.2)
            enter_buff = serialport.read(100)
            # print(enter_buff)
            repl_mode = received_data_check(b'Traceback', enter_buff)
            if repl_mode:
                # exit python repl mode
                # print('repl mode')
                serialport.write(reboot_cmd_cli.encode())
            else:
                # cli mode
                cli_mode = received_data_check(b'cmd not found', enter_buff)
                if cli_mode:
                    # print('cli mode')
                    serialport.write(reboot_cmd.encode())
                else:
                    # print('abnomarl')
                    reboot_method = 1;
                    break;

            # print(enter_buff)
            print("***** Device is rebooting *****")
            print("")
            break;


    # step 2: wait reboot and hand shakend cmd
    # judge reset dev
    if(reboot_method == 1):
        # need reset dev
        bmatched = False
    else:
        time.sleep(1)
        bmatched = send_check_recv_data(serialport, b'amp shakehand begin...', 10)
        if bmatched:
            print("***** Device reboot OK *****")
        else:
            print("***** Device reboot Failed *****")

        # logging.debug(buff)

    if bmatched:
        shakehand = file_transfer_handshake_function(serialport)
        if shakehand:
            print("***** recived handshark data *****")
        else:
            print("***** can not recived handshark data *****")
            print("")
            print("***** Please reboot the board manually, and try it again *****")
            print("")
            serialport.close()
            return
    else:
        print("***** Please reboot the board within 10 seconds *****")
        print("")
        bmatched_retry = send_check_recv_data_count_down(serialport, b'amp shakehand begin...', 10, 1)

        if bmatched_retry:
            shakehand = file_transfer_handshake_function(serialport)
            if shakehand:
                print("***** recived handshark data *****")
            else:
                print("***** can not recived handshark data *****")
                print("")
                print("***** Please reboot the board manually, and try it again *****")
                print("")
                serialport.close()
                return
        else:
            print("")
            print("***** Please reboot the board manually, and try it again *****")
            print("")
            serialport.close()
            return

    # start send amp boot cmd
    time.sleep(0.1)
    logging.debug("start to send amp_boot cmd")
    save_path = devpath
    cmd = 'amp_boot'

    if((save_path == 'NULL') or (save_path == 'DATA')):
        logging.debug('save file in /data')
        cmd = 'amp_boot'
    elif (save_path == 'SDCARD'):
        logging.debug('save file in /sdcard')
        cmd = 'amp_boot_sd'

    serialport.write(cmd.encode())
    # serialport.write(b'amp_boot')

    # send file transfer cmd
    time.sleep(0.1)
    # logging.debug("start to send file cmd")
    # cmd = 'cmd_file_transfer\n'
    # serialport.write(cmd.encode())
    bmatched = send_check_recv_data(serialport, b'amp shakehand success', 5)
    # serialport.write(b'cmd_flash_js\n')
    # send file
    if bmatched:
        print("***** Handshark success *****")
        print("")
        print("***** Start file transfer *****")
        print("")
        cmd = 'cmd_file_transfer\n'
        serialport.write(cmd.encode())

        time.sleep(0.1)
        serialport.timeout = 0.5
        ymodemTrans(serialport, filepath)
        # logging.debug("Ymodem transfer file finish")

        # send file transfer cmd
        time.sleep(0.1)
        print("***** File transfer End *****")
        print("")
        cmd = 'cmd_exit\n'
        serialport.write(cmd.encode())
    else:
        print("***** Handshark failed *****")
        print("***** Please reboot the board manually, and try it again *****")
    # close serialport
    serialport.close()


def get_downloadconfig():
    """ get configuration from .config_burn file, if it is not existed, 
        generate default configuration of chip_haas1000 """ 
    configs = {}
    configs['chip_haas1000'] = {}
    configs['chip_haas1000']['serialport'] = ''
    configs['chip_haas1000']['baudrate'] = ''
    configs['chip_haas1000']['savepath'] = ''
    configs['chip_haas1000']['filepath'] = ''

    return configs['chip_haas1000']


def main2():
    cmd_parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description='''Run and transfer file to system.''',)

    cmd_parser.add_argument('-d', '--device', default='', help='the serial device or the IP address of the pyboard')
    cmd_parser.add_argument('-b', '--baudrate', default=115200, help='the baud rate of the serial device')
    cmd_parser.add_argument('-p', '--path', default='NULL', help='the save file path: SDCARD or DATA or NULL')
    cmd_parser.add_argument('files', nargs='*', help='input transfer files')

    args = cmd_parser.parse_args()

    logging.debug(args)
    # download file
    # step 1: set config
    downloadconfig = get_downloadconfig()
    # step 2: get serial port
    if not downloadconfig["serialport"]:
        downloadconfig["serialport"] = args.device
        if not downloadconfig["serialport"]:
            downloadconfig["serialport"] = miniterm.ask_for_port()
            if not downloadconfig["serialport"]:
                logging.debug("no specified serial port")
                return
        else:
            needsave = True

    # step 3: get baudrate
    if not downloadconfig["baudrate"]:
        downloadconfig["baudrate"] = args.baudrate
        if not downloadconfig["baudrate"]:
            downloadconfig["baudrate"] = "115200"

    # step 4: get saved file path of devices
    if not downloadconfig["savepath"]:
        downloadconfig["savepath"] = args.path
        if not downloadconfig["savepath"]:
            logging.debug('use default path')
            return

    # step 4: get transfer file
    if not downloadconfig["filepath"]:
        downloadconfig["filepath"] = args.files
        if not downloadconfig["filepath"]:
            logging.debug('no file wait to transfer')
            return

    if os.path.isabs("".join(downloadconfig["filepath"])):
        filepath = "".join(downloadconfig["filepath"])
        logging.debug('the filepath is abs path')
    else:
        basepath = os.path.abspath('.')
        filepath = basepath + '/' + "".join(downloadconfig["filepath"])
        logging.debug('the filepath is not abs path')

    print("")
    print("***** Params setting *****")
    print("serial port is %s" % downloadconfig["serialport"])
    print("transfer baudrate is %s" % downloadconfig["baudrate"])
    print("savepath is %s" % downloadconfig["savepath"])
    print("filepath is %s" % filepath)
    print("***** Params setting end *****")
    print("")

    # step 3: download file
    download_file(downloadconfig["serialport"], downloadconfig['baudrate'], filepath, downloadconfig['savepath'])



if __name__ == "__main__":
    main2()
