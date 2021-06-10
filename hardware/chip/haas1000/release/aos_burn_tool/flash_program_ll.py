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
    buff    = b''
    while (time.time() - start) < timeout:
        #line = serialport.readline()
        #timeout dont work for 'readline', so using 'read_until' instead
        line = serialport.read_until(b'\n')
        if len(line) == 0:
            continue
        buff += line
        match = pattern.search(buff)
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
    pattern = re.compile(pattern)
    start   = time.time()
    buff    = b''
    while (time.time() - start) < timeout:
        #line = serialport.readline()
        #timeout dont work for 'readline', so using 'read_until' instead
        line = serialport.read_until(b'\n')
        if len(line) == 0:
            continue
        buff += line
        print(line.decode('UTF-8',errors='ignore'))
        sys.stdout.flush()
        match = pattern.search(buff)
        if match:
            return match
    return None

#burn file in 2_boot
def burn_bin_file(serialport, filename, address):
    if not os.path.exists(filename):
        print("file \"%s\" is not existed." % filename)
        return False

    #get address
    if address == "0" or address == "0x0" or address == "0x00":
        # get flash address
        match = send_and_match(serialport, b'1', b'Backup part addr:([0-9a-fxA-F]*)', 5)
        if not match:
            print("Can not get flash address")
            return False
        address = match.group(1)
    else:
        serialport.write(b'1')
        address = address.encode()
        
    # set flash address
    match = send_and_match(serialport, address + b'\r\n', b'CCCC', 30)
    if not match:
        print("Can not enter into ymodem mode")
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
        raise Exception("Failed to open serial port: %s!" % portnum)

    for i in range(3):
        #reset in 2_boot or CLI
        serialport.write(b'\n2\n')
        serialport.write(b'reboot\n')
        match = match_and_send(serialport, b'2ndboot cli menu', b'w', 5)
        if match:
            print('Reset success!!')
            sys.stdout.flush()
            break
        #reset in Ymodem
        print('Reset Ymodem!!')
        sys.stdout.flush()
        serialport.write(b'\x13')
        serialport.write(b'\x04')
        serialport.write(b'\x03')

    if i == 3:
        print("Please reset the HaaS100 manually.")
        serialport.close()
        return False

    for bin_file in bin_files:
        if not burn_bin_file(serialport, bin_file[0], bin_file[1]):
            print("Download file %s failed." % bin_file[0])
            serialport.close()
            return False

    # switch partition
    print("Swap AB partition")
    serialport.write(b'3')
    time.sleep(0.5)
    serialport.write(b'4')
    time.sleep(0.5)
    serialport.write(b'3')
    time.sleep(0.5)
    serialport.write(b'2')
    time.sleep(0.5)
    # workaround retry issue in 2nd boot
    serialport.write(b'\n' * 16)
    match = send_and_match(serialport, b'', b'2ndboot cli menu', 5)
    if match:
        print("Burn \"%s\" success." % bin_files)

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
