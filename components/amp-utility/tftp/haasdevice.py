
#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    haasdevice.py
@Description:    file description   
@Date       :    2021/06/03 10:03:12
@Author     :    guoliang.wgl
@version    :    1.0
'''

# here put the import lib

import time
import os
import logging
import serial
import logging
from subprocess import *
import threading
import time
import sys
import argparse
import socket
import json

logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s - %(filename)s[line:%(lineno)d] - %(levelname)s: %(message)s')


class HaaSboard:
    def __init__(self, device, baudrate=115200, user="micro", password="python", wait=0):
        self.serial = serial.Serial()
        self.serial.port = device
        self.serial.baudrate = baudrate
        self.serial.parity = "N"
        self.serial.bytesize = 8
        self.serial.stopbits = 1
        self.serial.timeout = 0.05

        try:
            self.serial.open()
        except Exception as e:
            raise Exception("Failed to open serial port: %s!" % device)



    def close(self):
        self.serial.close()


    def run_cmd(self,cmd):
        self.serial.write(cmd)

    def run_cmd_follow(self,cmd,ending,timeout = 10):
        self.serial.write(cmd)
        return self.read_until(1,ending,timeout)



    def read_until(self, min_num_bytes, ending, timeout=10, data_consumer=None):
        # if data_consumer is used then data is not accumulated and the ending must be 1 byte long
        assert data_consumer is None or len(ending) == 1

        data = self.serial.read(min_num_bytes)
        if data_consumer:
            data_consumer(data)
        timeout_count = 0
        while True:
            if data.endswith(ending):
                break
            elif self.serial.inWaiting() > 0:
                new_data = self.serial.read(1)
                if data_consumer:
                    data_consumer(new_data)
                    data = new_data
                else:
                    data = data + new_data
                timeout_count = 0
            else:
                timeout_count += 1
                if timeout is not None and timeout_count >= 100 * timeout:
                    logging.warning("Waiting for  %s  timeout" %(ending.decode('utf-8')))
                    break
                time.sleep(0.01)
        return data


class HaaSDevice(object):
    """
    all the HaaSDevice  functions can be found int this class
    """

    CMD_REBOOT = 'reboot \n'
    FLAG_REBOOT = 'register python command succeed'
    CMD_UNZIP = 'unzip'
    FLAG_UNZIP = 'register python command succeed'
    CMD_WIFI = "python /data/python-apps/wifi/main.py "
    FLAG_WIFI = "DeviceIP:"

    CMD_TFTP = "tftp get "
    FLAG_TFTP = "tftp received"




    def __init__(self):
        self._baudrate = 115200
        self._dev_node = None
        self._board = None
        self._ip = None
        self._ssid = None
        self._pwd = None

    def is_str_empty(self,value):
        if value is None or value == "" :
            return True
        else:
            return False

    @property
    def baudrate(self):
        return self._baudrate

    @baudrate.setter
    def baudrate(self,value):
        self._baudrate = value

    
    @property
    def dev_node(self):
        return self._dev_node

    @dev_node.setter
    def dev_node(self,value):
        self._dev_node = value


    @property
    def ip(self):
        return self._ip

    @property
    def ssid(self):
        return self._ssid

    @ssid.setter
    def ssid(self,value):
        self._ssid = value    


    @property
    def pwd(self):
        return self._pwd

    @ssid.setter
    def pwd(self,value):
        self._pwd = value      


    def connect_board(self):
         self._board = HaaSboard(self._dev_node,self._baudrate,wait=5)

    def reboot(self):
        if self._board is None:
            raise OSError("Please connect the board  first")
        logging.info("Try to reboot device...")
    
        ret = self._board.run_cmd_follow(self.CMD_REBOOT.encode('utf-8'),self.FLAG_REBOOT.encode('utf-8'))
        if ret :
            logging.info("reboot succeed ")
        else :
            raise OSError("reboot failed ")

    def connect_wifi(self):
        """
        todo : verify if the wifi is connected
        """
        if self._board is None:
            raise OSError("Please connect the board  first")
    
        if self.is_str_empty(self._ssid):
            raise ValueError("Error:ssid is null")

        if self.is_str_empty(self._pwd):
            raise ValueError("Error:password is null")


        cmd =  self.CMD_WIFI+ ' ' + self._ssid + ' ' + self._pwd + ' ' + '\r\n'
        logging.info("connect_wifi:" +cmd)
        ret = self._board.run_cmd_follow(cmd.encode('utf-8'),self.FLAG_WIFI.encode('utf-8'),10)

        if ret :
            pass
        else:
            raise("Connect wifi failed")

        self._ip = self._board.read_until(1,b"\n").decode('UTF-8').strip()
        self._board.read_until(1,b"ConnectWifi finished",3)
        return self._ip


    def tftp_get(self,src_ip,port,src_file,dest = None):
        if self._board is None:
            raise OSError("Please connect the board  first")
        
        if self.is_str_empty(src_ip):
            raise ValueError("src IP can't be None")

        if self.is_str_empty(src_file):
            raise ValueError("src file can't be None")
        
        if not os.access(src_file,os.R_OK):
            raise PermissionError("src file can't be read")

        if self.is_str_empty(dest):
            dest = '/data/' + src_file
        else:
            dest = dest + src_file

        if src_file.endswith('.zip'):
            tftp_get_cmd = self.CMD_TFTP + ' ' + src_ip + ' ' + port + ' ' + src_file  + ' ' +  dest +  ' ' + 'binary' +  '\n'
        else:
            tftp_get_cmd = self.CMD_TFTP + ' ' + src_ip + ' ' + port + ' ' + src_file  + ' ' +  dest +   ' ' +  'text' +  '\n'


        self._board.run_cmd_follow(tftp_get_cmd.encode('utf-8'),b"tftp received")
        ret = self._board.read_until(1,b"\n").decode('UTF-8').strip()
        if 'failed' in ret:
            logging.error('run '+ tftp_get_cmd + ' failed:tftp received ' + ret )
            return None
        else:
            return dest

    def unzip(self,src,dest = None):
        if self._board is None:
            raise OSError("Please connect the board  first")

        if self.is_str_empty(dest):
            dest = '/data/'

        dest = dest + 'app' + '/'

        # 创建目录
        mk_dir = 'mkdir ' + dest + ' \n'
        self._board.run_cmd(mk_dir.encode('utf-8'))
        time.sleep(1)
        unzip_cmd =  'unzip '   + src + ' ' + dest  +  '\n'
        logging.info("unzip cmd:" +unzip_cmd)
        ret = self._board.run_cmd_follow(unzip_cmd.encode('utf-8'),b"unzip succeed")
        return dest+'main.py'


    def run_app(self,main_entry):
        if self._board is None:
            raise OSError("Please connect the board  first")

        if self.is_str_empty(main_entry):
            raise ValueError("app entry:" + main_entry + "cat't be null")

        run_cmd  = 'python ' + main_entry + '\r\n'
        logging.info("Try to run app:" + main_entry)

        self._board.run_cmd_follow(run_cmd.encode('utf-8'), '\r\n'.encode('utf-8'))


    def close(self):

        if self._board is None:
            raise OSError("Please connect the board  first")
        else:
            self._board.close()

def start_client_and_run(dev_node,baudrate,ssid,pwd,host,port,srcfile,dest_dir):
    """
    @description  :
    ---------
    @param        :
    -------
    @Returns      :
    -------
    """
    device = HaaSDevice()


    device.baudrate = baudrate
    device.dev_node = dev_node
    device.ssid = ssid
    device.pwd = pwd

    device.connect_board()
    #device.reboot()

    if device.ip is None:
        try:
            logging.info("Try to connect wifi for device first time")
            device.connect_wifi()
        except SystemError:
            print('')

    if device.ip is None:
        logging.info("Try to connect wifi for device second time")
        device.connect_wifi()
    
    if device.ip is None:
        raise OSError("Connect wifi failed ,no ip found")

    logging.info("GetDevice IP succeed: " + device.ip)
    #todo get pc ip
    #ping pc and device 
    pc_ip = host
    src_file = srcfile
    tftp_port = port
    if 'sdcard' in dest_dir:
        dest_dir = '/sdcard/'
    else:
        dest_dir = '/data/'

    ret = device.tftp_get(pc_ip,tftp_port,src_file,dest_dir)
    if ret:
        logging.info("Push files to device succeed: " + ret)
        if src_file.endswith('.zip'):
            py_main = device.unzip(ret,dest = dest_dir)
            logging.info("unzip files to device succeed: " + py_main)
        elif src_file.endswith('.py'):
            py_main = dest_dir + src_file
        else:
            py_main = None

    else:
        raise OSError("tftp_get   failed" )

    if not is_str_empty(py_main):
        device.run_app(py_main)
        logging.info("run "  + py_main + " succeed" )
    
    device.close()
    return True


def start_tftp_server(host,post):
    global event
    global on_server_started
    logging.info("Trying to start tftp server...")
    tftp_info = Popen(["py3tftp", "--host", host,"-p",port],stdout = PIPE,stderr = STDOUT)

    retry = 0 
    while tftp_info.poll() is None:
        retry = retry + 1
        info = tftp_info.stdout.readline().decode('utf-8')
        if "Listening..." in info:
            on_server_started = True
            break
        if (retry > 10 ):
            logging.error("Start tftp server failed")
            break
        
        logging.info(info)

    if on_server_started:
        event.wait()
        tftp_info.kill()
    else:
        raise SystemError("tftp server start failed,by retry 10 times")




    

def stop_tftp_server():
    global event
    event.set()


def is_str_empty(value):
    if value is None or value == "" :
        return True
    else:
        return False

if __name__ == '__main__':
    on_server_started = False
    logging.info("main function start ")
    
    if(len(sys.argv) == 2):
        config = sys.argv[1]
        logging.info("tftp config  is : %s " % config )
        if not os.path.isfile(config):
            logging.error("tftp config must be a file")
            sys.exit(1)
        
        with open(config,'r') as f :
            json_str = json.load(f)
            logging.info(json_str)


            # 以下是必选项
            host = json_str['ip']
            port = '6069'
            baudrate = json_str['baudrate']
            dev_node = json_str['serialPort']
            ssid = json_str['ssid']
            pwd = json_str['pwd']
            dest_dir = json_str['dstDir']
            if 'file' in json_str:
                target_file = json_str['file']
            else:
                target_file = 'app.zip'
    else:

        cmd_parser = argparse.ArgumentParser()
        cmd_parser.add_argument('-d', '--device', default='', help='the serial device ')
        cmd_parser.add_argument('-b', '--baudrate', default='', help='the baud rate of the serial device')
        cmd_parser.add_argument('-s', '--ssid', default='', help='the wifi ssid')
        cmd_parser.add_argument('-p', '--password', default='', help='the wifi password')
        cmd_parser.add_argument('-ip', '--ip', default='', help='the PC ip address')
        cmd_parser.add_argument('-f', '--file', default='', help='the file to be send')
        cmd_parser.add_argument('--dest', default='', help='the dest dir file to be send') 
        args = cmd_parser.parse_args()
        host = args.ip
        port = '6069'

        baudrate = args.baudrate
        dev_node = args.device
        ssid = args.ssid
        pwd = args.password

        if is_str_empty(args.file):
            targetfile =  'app.zip'
        else:
            targetfile = args.file


    # host = '192.168.3.241'
    # port = '6069'

    # baudrate = 1500000
    # dev_node = "/dev/cu.SLAB_USBtoUART"
    # ssid = "KIDS"
    # pwd = "12345678"


    # host = socket.gethostbyname(socket.gethostname())


    if is_str_empty(host) or is_str_empty(baudrate) or is_str_empty(dev_node) or is_str_empty(ssid) or is_str_empty(pwd):
        #logging.error("Usage1:python  haasdevice.py -d /dev/cu.SLAB_USBtoUART -b 1500000 -s KIDS -p 12345678 -ip 192.168.3.241")
        logging.error("Usage2:python  haasdevice.py tftpcfg.json")
        sys.exit(1)



    event = threading.Event()
    event.clear()

    server_t = threading.Thread(target=start_tftp_server,args=((host,port)))
    server_t.start()

    while ( not on_server_started):
        time.sleep(1)
        logging.info("Waiting for tftp server started....")
    else:
        logging.info("tftp server start succeed")
        start_client_and_run(dev_node,baudrate,ssid,pwd,host,port,target_file,dest_dir)
        stop_tftp_server()
