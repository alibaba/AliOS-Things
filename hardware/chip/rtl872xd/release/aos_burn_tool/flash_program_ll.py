#!/usr/bin/env python
import os, sys, re, time
from ymodem import YModem

try:
    import serial
    from serial.tools import miniterm
except:
    print("\nNot found pyserial, please install it by: \nsudo python%d -m pip install pyserial" % (sys.version_info.major))
    sys.exit(-1)

def match_and_send(serialport, pattern, command, timeout):
    """ receive serial data, and check it with pattern """
    pattern = re.compile(pattern)
    start   = time.time()
    while (time.time() - start) < timeout:
        #line = serialport.readline()
        #timeout dont work for 'readline', so using 'read_until' instead
        line = serialport.read_until(b'\n')
        if len(line) == 0:
            continue
        match = pattern.search(line)
        if match:
            if command:
                serialport.write(command)
            print(line.decode('UTF-8',errors='ignore'))
            sys.stdout.flush()
            return match
        print(line.decode('UTF-8',errors='ignore'))
        sys.stdout.flush()
    return None

def send_and_match(serialport, command, pattern, timeout):
    """ receive serial data, and check it with pattern """
    if command:
        serialport.write(command)
    if pattern == b'':
        #only send
        sys.stdout.flush()
        return None
    pattern = re.compile(pattern)
    start   = time.time()
    while (time.time() - start) < timeout:
        #line = serialport.readline()
        #timeout dont work for 'readline', so using 'read_until' instead
        line = serialport.read_until(b'\n')
        if len(line) == 0:
            continue
        print(line.decode('UTF-8',errors='ignore'))
        sys.stdout.flush()
        match = pattern.search(line)
        if match:
            return match
    return None

#burn file in 2_boot
def burn_bin_file(serialport, filename, address):
    if not os.path.exists(filename):
        print("[ScriptPrint] File \"%s\" is not existed." % filename)
        return False

    #get address
    if address == "0" or address == "0x0" or address == "0x00":
        # get flash address
        match = send_and_match(serialport, b'5', b'Backup part addr:([0-9a-fxA-F]*)', 5)
        if not match:
            print("[ScriptPrint] Can not get flash address")
            return False
        address = match.group(1)
    else:
        send_and_match(serialport, b'6', b'', 0)
        address = address.encode()

    # set flash address
    match = send_and_match(serialport, address + b'\r\n', b'CCCC', 30)
    if not match:
        print("[ScriptPrint] Can not enter ymodem mode")
        return False

    # send binary file
    def sender_getc(size):
        return serialport.read(size) or None

    def sender_putc(data, timeout=15):
        return serialport.write(data)

    sender = YModem(sender_getc, sender_putc)
    sent = sender.send_file(filename)
    return True

def burn_bin_files(portnum, baudrate, bin_files):
    # open serial port
    serialport = serial.Serial()
    serialport.port = portnum
    serialport.baudrate = baudrate
    serialport.parity   = "N"
    serialport.bytesize = 8
    serialport.stopbits = 1
    serialport.timeout  = 1

    try:
        serialport.open()
    except Exception as e:
        raise Exception("[ScriptPrint] Failed to open serial port: %s!" % portnum)

    print("[ScriptPrint] Try to reboot...")

    # 重启单板并确保进入2nd-boot
    for i in range(10):
        match = send_and_match(serialport, b'\n', b'\(ash', 2)
        if match:
            # 如果在cli模式，通过reboot重启系统
            print("[ScriptPrint] Reboot from CLI")
            send_and_match(serialport, b'reboot\n', b'', 0)
            match = match_and_send(serialport, b'2ndboot cli menu', b'w', 5)
            if match:
                print("[ScriptPrint] check if in boot")
                match = send_and_match(serialport, b'\n', b'aos boot', 2)
                if match:
                    # 进入boot模式，退出
                    break

        match = send_and_match(serialport, b'\n', b'aos boot', 2)
        if match:
            # 如果在boot模式，通过2重启系统
            print("[ScriptPrint] Reboot from 2nd-boot")
            send_and_match(serialport, b'2\n', b'', 0)
            match = match_and_send(serialport, b'2ndboot cli menu', b'w', 5)
            if match:
                print("[ScriptPrint] check if in boot")
                match = send_and_match(serialport, b'\n', b'aos boot', 2)
                if match:
                    # 进入boot模式，退出
                    break

        match = match_and_send(serialport, b'2ndboot cli menu', b'w', 2)
        if match:
            print("[ScriptPrint] check if in boot")
            match = send_and_match(serialport, b'\n', b'aos boot', 2)
            if match:
                # 进入boot模式，退出
                break

        else:
            # 一些solution需要先退出命令行模式回到CLI
            print("[ScriptPrint] change to CLI mode")
            send_and_match(serialport, b'\n\x03', b'', 0) #ctrl-C, ETX, 本文结束
            send_and_match(serialport, b'\n\x04', b'', 0) #ctrl-D, EOT, 传输结束

        time.sleep(1)

    if i >= 9 :
        print("[ScriptPrint] reboot fail")
        print("[ScriptPrint] Please connect the serial port of the board to the PC, then reset the board");
        # close serial port
        serialport.close()
        return False

    # boot中下载文件
    print("[ScriptPrint] Downloading files...")
    for bin_file in bin_files:
        if not burn_bin_file(serialport, bin_file[0], bin_file[1]):
            print("[ScriptPrint] Download file %s failed." % bin_file[0])
            serialport.close()
            return False

    # switch partition
    print("[ScriptPrint] Swap AB partition")
    send_and_match(serialport, b'3\n', b'', 0)
    time.sleep(0.5)
    send_and_match(serialport, b'4\n', b'', 0)
    time.sleep(0.5)
    send_and_match(serialport, b'3\n', b'', 0)
    time.sleep(0.5)
    send_and_match(serialport, b'2\n', b'', 0)
    time.sleep(0.1)
    # workaround retry issue in 2nd boot
    match = send_and_match(serialport, b'\n'*16, b'ota update complete',100)
    if match:
        print("[ScriptPrint] Burn \"%s\" success." % bin_files)

    # close serial port
    serialport.close()

    if match:
        return True
    else:
        return False

def main():
    length = len(sys.argv)
    if (length < 5) or (length % 2 == 0):
        print("Usage demo: ./flash_program_ll.py COM6 1500000 sendfile flash_addr\n")
        return 1

    serialport = sys.argv[1]
    baudrate = sys.argv[2]
    bin_files = []
    for i in range(3, length, 2):
        filename = sys.argv[i]
        address = sys.argv[i + 1]
        bin_files.append((filename, address))

    ret = burn_bin_files(serialport, baudrate, bin_files)
    sys.exit(ret)

if __name__ == "__main__":
    main()
