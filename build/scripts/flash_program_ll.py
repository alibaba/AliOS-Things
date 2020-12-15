#!/usr/bin/env python
import os, sys, re, time
from ymodem import YModem

try:
    import serial
    from serial.tools import miniterm
except:
    print("\nNot found pyserial, please install it by: \nsudo python -m pip install pyserial")
    sys.exit(0)


def send_cmd_check_recv_data(serialport, command, pattern, timeout):
    """ receive serial data, and check it with pattern """
    if command:
        serialport.write(command)
    matcher = re.compile(pattern)
    tic     = time.time()
    buff    = serialport.read(128)
    while (time.time() - tic) < timeout:
        buff += serialport.read(128)
        if matcher.search(buff):
            return True, buff
        else:
            if command:
                serialport.write(command)
    return False, buff

def burn_bin_file(serialport, filename, address):
    if not os.path.exists(filename):
        print("file \"%s\" is not existed." % filename)
        return False
    for i in range(3):
        # ymodem update
        serialport.write(b'1')
        time.sleep(0.1)

        # get flash address
        bmatched, buff = send_cmd_check_recv_data(serialport, b'', b'Please input flash addr:', 2)
        print(buff)
        if bmatched:
            break
    if address == "0":
        if bmatched:
            pattern = re.compile(b'Backup part addr:([0-9a-fxA-F]*)')
            match = pattern.search(buff)
            if match:
                address = match.group(1)
            else:
                print("can not get flash address")
                return False
        else:
            print("can not get flash address")
            return False
    else:
        address = address.encode()

    # set flash address
    serialport.write(address)
    serialport.write(b'\r\n')
    time.sleep(0.1)
    bmatched, buff = send_cmd_check_recv_data(serialport, b'', b'CCCCC', 5)
    print(buff)
    if not bmatched:
        print("can not enter into ymodem mode")
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
    serialport.parity = "N"
    serialport.bytesize = 8
    serialport.stopbits = 1
    serialport.timeout = 0.05

    try:
        serialport.open()
    except Exception as e:
        raise Exception("Failed to open serial port: %s!" % portnum)

    # reboot the board, and enter into 2nd boot mode
    bmatched = False
    for i in range(300):
        serialport.write(b'\nreboot\n\n')
        time.sleep(0.1)
        bmatched, buff = send_cmd_check_recv_data(serialport, b'w', b'aos boot#', 2)
        print(buff)
        if bmatched:
            break
        if i > 3:
            print("Please reboot the board manually.")

    if not bmatched:
        print("Please reboot the board manually, and try it again.")
        serialport.close()
        return 1

    for bin_file in bin_files:
        if not burn_bin_file(serialport, bin_file[0], bin_file[1]):
            print("Download file %s failed." % bin_file[0])
            serialport.close()
            return 1

    # switch partition
    print("Swap AB partition");
    serialport.write(b'3')
    time.sleep(0.1)
    serialport.write(b'4')
    time.sleep(0.1)
    serialport.write(b'3')
    time.sleep(0.1)
    serialport.write(b'2')
    time.sleep(0.1)
    bmatched, buff = send_cmd_check_recv_data(serialport, b'', b'2ndboot cli menu in [1-9]00ms', 2)
    print(buff)
    if bmatched:
        print("Burn \"%s\" success." % bin_files)

    # close serial port
    serialport.close()

    if bmatched:
        return 0
    else:
        return 1

def main():
    length = len(sys.argv)
    if (length < 5) or (length % 2 == 0):
        print("Usage: ./flash_program_ll.py COM6 1500000 sendfile flash_addr\n")
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
