#!/usr/bin/env python

import sys, os, serial, time, platform, logging
from functools import partial

# MODEM Protocol bytes
NUL = b'\x00'
SOH = b'\x01'
STX = b'\x02'
EOT = b'\x04'
ACK = b'\x06'
DLE = b'\x10'
NAK = b'\x15'
CAN = b'\x18'
CRC = b'C'

LOG_LEVEL_ERROR=0
LOG_LEVEL_WARN =1
LOG_LEVEL_INFO =2
LOG_LEVEL_DEBUG=3

class XMODEM(object):
    '''
    XMODEM Protocol handler, expects an object to read from and an object to
    write to.

    >>> def getc(size, timeout=1):
    ...     return data or None
    ...
    >>> def putc(data, timeout=1):
    ...     return size or None
    ...
    >>> modem = XMODEM(getc, putc)


    :param getc: Function to retrieve bytes from a stream
    :type getc: callable
    :param putc: Function to transmit bytes to a stream
    :type putc: callable
    :param mode: XMODEM protocol mode
    :type mode: string
    :param pad: Padding character to make the packets match the packet size
    :type pad: char

    '''

    # crctab calculated by Mark G. Mendel, Network Systems Corporation
    crctable = [
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
    ]

    def __init__(self, getc, putc, mode='xmodem', pad=b'\x1a'):
        self.getc = getc
        self.putc = putc
        self.mode = mode
        self.pad = pad
        self.log_level = LOG_LEVEL_WARN

    def log(self, log_level, logstr):
        if log_level <= self.log_level:
            print logstr


    def abort(self, count=2, timeout=60):
        '''
        Send an abort sequence using CAN bytes.
        '''
        for _ in range(count):
            self.putc(CAN, timeout)

    def send(self, stream, retry=16, timeout=60, quiet=False, callback=None):
        '''
        Send a stream via the XMODEM protocol.

            >>> stream = open('/etc/issue', 'rb')
            >>> print(modem.send(stream))
            True

        Returns ``True`` upon successful transmission or ``False`` in case of
        failure.

        :param stream: The stream object to send data from.
        :type stream: stream (file, etc.)
        :param retry: The maximum number of times to try to resend a failed
                      packet before failing.
        :type retry: int
        :param timeout: The number of seconds to wait for a response before
                        timing out.
        :type timeout: int
        :param quiet: If True, write transfer information to stderr.
        :type quiet: bool
        :param callback: Reference to a callback function that has the
                         following signature.  This is useful for
                         getting status updates while a xmodem
                         transfer is underway.
                         Expected callback signature:
                         def callback(total_packets, success_count, error_count)
        :type callback: callable
        '''

        # initialize protocol
        try:
            packet_size = dict(
                xmodem    = 128,
                xmodem1k  = 1024,
                ymodem    = 1024,
            )[self.mode]
        except KeyError:
            raise ValueError("Invalid mode specified: {self.mode!r}"
                             .format(self=self))

            self.log(LOG_LEVEL_DEBUG, 'Begin start sequence, packet_size={0:d}'.format(packet_size))
        error_count = 0
        crc_mode = 0
        cancel = 0
        while True:
            char = self.getc(1)
            if char:
                if char == NAK:
                    self.log(LOG_LEVEL_DEBUG, 'standard checksum requested (NAK).')
                    crc_mode = 0
                    break
                elif char == CRC:
                    self.log(LOG_LEVEL_DEBUG, '16-bit CRC requested (CRC).')
                    crc_mode = 1
                    break
                elif char == CAN:
                    if not quiet:
                        sys.stderr.write('received CAN\n')
                    if cancel:
                        self.log(LOG_LEVEL_INFO, 'Transmission canceled: received 2xCAN at start-sequence')
                        return False
                    else:
                        self.log(LOG_LEVEL_DEBUG, 'cancellation at start sequence.')
                        cancel = 1
                else:
                    self.log(LOG_LEVEL_ERROR, 'send error: expected NAK, CRC, or CAN; got {0}'.format(char))

            error_count += 1
            if error_count > retry:
                self.log(LOG_LEVEL_INFO, 'send error: error_count reached {0}, aborting.'.format(retry))
                self.abort(timeout=timeout)
                return False

        # send data
        error_count = 0
        success_count = 0
        total_packets = 0
        if self.mode == 'ymodem':
            sequence = 0
            filenames = stream
        else:
            sequence = 1
        while True:
            # build packet
            if self.mode == 'ymodem' and success_count == 0:
                # send packet sequence 0 containing:
                #  Filename Length [Modification-Date [Mode [Serial-Number]]]
                # 'stream' is actually the filename
                import os
                if len(filenames):
                    filename = filenames.pop()
                    stream = open(filename, 'rb')
                    stat = os.stat(filename)
                    data = os.path.basename(filename) + NUL + str(stat.st_size)
                    self.log(LOG_LEVEL_DEBUG, 'ymodem sending : {0} len:{1}'.format(filename, stat.st_size))
                else:
                    # empty file name packet terminates transmission
                    filename = ''
                    data = ''
                    stream = None
                    self.log(LOG_LEVEL_DEBUG, 'ymodem done, sending empty header.')
                if len(data) <= 128:
                    header_size = 128
                else:
                    header_size = 1024

                header = self._make_send_header(header_size, sequence)
                data = data.ljust(header_size, NUL)
                checksum = self._make_send_checksum(crc_mode, data)
            else:
                # happens after sending ymodem empty filename
                if not stream:
                    return True
                # normal data packet
                data = stream.read(packet_size)
                if not data:
                    # end of stream
                    self.log(LOG_LEVEL_DEBUG, 'send: at EOF')
                    break
                total_packets += 1

                header = self._make_send_header(packet_size, sequence)
                data = data.ljust(packet_size, self.pad)
                checksum = self._make_send_checksum(crc_mode, data)

            # emit packet & get ACK
            while True:
                self.log(LOG_LEVEL_DEBUG, 'send: block {0}'.format(sequence))
                self.putc(header + data + checksum)
                char = self.getc(1, timeout)
                if char == ACK:
                    success_count += 1
                    if callable(callback):
                        callback(total_packets, success_count, error_count)
                    error_count = 0
                    if self.mode == 'ymodem' and success_count == 1 and len(filename):
                        char = self.getc(1, timeout)
                        if char == DLE: # dunno why
                            char = self.getc(1, timeout)
                        if char == CRC:
                            break
                        self.log(LOG_LEVEL_ERROR, 'send error: ymodem expected CRC; got {0} for block {1}'.format(char, sequence))
                    else:
                        break

                self.log(LOG_LEVEL_ERROR, 'send error: expected ACK; got {0} for block {1}'.format(char, sequence))
                error_count += 1
                if callable(callback):
                    callback(total_packets, success_count, error_count)
                if error_count > retry:
                    # excessive amounts of retransmissions requested,
                    # abort transfer
                    self.log(LOG_LEVEL_ERROR, 'send error: NAK received {0} times, aborting.'.format(error_count))
                    self.abort(timeout=timeout)
                    return False

            # keep track of sequence
            sequence = (sequence + 1) % 0x100

        # emit EOT and get corresponding ACK
        while True:
            self.log(LOG_LEVEL_DEBUG, 'sending EOT, awaiting ACK')
            # end of transmission
            self.putc(EOT)

            # An ACK should be returned
            char = self.getc(1, timeout)
            if char == ACK:
                break
            else:
                self.log(LOG_LEVEL_ERROR, 'send error: expected ACK; got {0}'.format(char))
                error_count += 1
                if error_count > retry:
                    self.log(LOG_LEVEL_WARN, 'EOT was not ACKd, aborting transfer')
                    self.abort(timeout=timeout)
                    return False

        self.log(LOG_LEVEL_INFO, 'Transmission successful (ACK received).')
        if self.mode == 'ymodem':
            # YMODEM - recursively send next file
            # or empty filename header to end the xfer batch.
            stream.close()
            return self.send(filenames, retry, timeout, quiet, callback)
        return True

    def _make_send_header(self, packet_size, sequence):
        assert packet_size in (128, 1024), packet_size
        _bytes = []
        if packet_size == 128:
            _bytes.append(ord(SOH))
        elif packet_size == 1024:
            _bytes.append(ord(STX))
        _bytes.extend([sequence, 0xff - sequence])
        return bytearray(_bytes)

    def _make_send_checksum(self, crc_mode, data):
        _bytes = []
        if crc_mode:
            crc = self.calc_crc(data)
            _bytes.extend([crc >> 8, crc & 0xff])
        else:
            crc = self.calc_checksum(data)
            _bytes.append(crc)
        return bytearray(_bytes)

    def recv(self, stream, crc_mode=1, retry=16, timeout=60, delay=1, quiet=0):
        '''
        Receive a stream via the XMODEM protocol.

            >>> stream = open('/etc/issue', 'wb')
            >>> print(modem.recv(stream))
            2342

        Returns the number of bytes received on success or ``None`` in case of
        failure.
        '''

        # initiate protocol
        error_count = 0
        char = 0
        cancel = 0
        while True:
            # first try CRC mode, if this fails,
            # fall back to checksum mode
            if error_count >= retry:
                self.log(LOG_LEVEL_INFO, 'error_count reached {0}, aborting.'.format(retry))
                self.abort(timeout=timeout)
                return None
            elif crc_mode and error_count < (retry // 2):
                if not self.putc(CRC):
                    self.log(LOG_LEVEL_DEBUG, 'recv error: putc failed, sleeping for {0}'.format(delay))
                    time.sleep(delay)
                    error_count += 1
            else:
                crc_mode = 0
                if not self.putc(NAK):
                    self.log(LOG_LEVEL_DEBUG, 'recv error: putc failed, sleeping for {0}'.format(delay))
                    time.sleep(delay)
                    error_count += 1

            char = self.getc(1, timeout)
            if char is None:
                self.log(LOG_LEVEL_WARN, 'recv error: getc timeout in start sequence')
                error_count += 1
                continue
            elif char == SOH:
                self.log(LOG_LEVEL_DEBUG, 'recv: SOH')
                break
            elif char == STX:
                self.log(LOG_LEVEL_DEBUG, 'recv: STX')
                break
            elif char == CAN:
                if cancel:
                    self.log(LOG_LEVEL_INFO, 'Transmission canceled: received 2xCAN at start-sequence')
                    return None
                else:
                    self.log(LOG_LEVEL_DEBUG, 'cancellation at start sequence.')
                    cancel = 1
            else:
                error_count += 1

        # read data
        error_count = 0
        income_size = 0
        packet_size = 128
        sequence = 1
        cancel = 0
        while True:
            while True:
                if char == SOH:
                    if packet_size != 128:
                        self.log(LOG_LEVEL_DEBUG, 'recv: SOH, using 128b packet_size')
                        packet_size = 128
                    break
                elif char == STX:
                    if packet_size != 1024:
                        self.log(LOG_LEVEL_DEBUG, 'recv: SOH, using 1k packet_size')
                        packet_size = 1024
                    break
                elif char == EOT:
                    # We received an EOT, so send an ACK and return the
                    # received data length.
                    self.putc(ACK)
                    self.log(LOG_LEVEL_INFO, "Transmission complete, {0} bytes".format(income_size))
                    return income_size
                elif char == CAN:
                    # cancel at two consecutive cancels
                    if cancel:
                        self.log(LOG_LEVEL_INFO, 'Transmission canceled: received 2xCAN at block {0}'.format(sequence))
                        return None
                    else:
                        self.log(LOG_LEVEL_DEBUG, 'cancellation at block {0}'.format(sequence))
                        cancel = 1
                else:
                    err_msg = ('recv error: expected SOH, EOT; got {0}'.format(char))
                    if not quiet:
                        sys.stderr.write(err_msg+"\n")
                    self.log(LOG_LEVEL_WARN, err_msg)
                    error_count += 1
                    if error_count > retry:
                        self.log(LOG_LEVEL_INFO, 'error_count reached {0}, aborting.'.format(retry))
                        self.abort()
                        return None

            # read sequence
            error_count = 0
            cancel = 0
            self.log(LOG_LEVEL_DEBUG, 'recv: data block {0}'.format(sequence))
            seq1 = self.getc(1, timeout)
            if seq1 is None:
                self.log(LOG_LEVEL_WARN, 'getc failed to get first sequence byte')
                seq2 = None
            else:
                seq1 = ord(seq1)
                seq2 = self.getc(1, timeout)
                if seq2 is None:
                    self.log(LOG_LEVEL_WARN, 'getc failed to get second sequence byte')
                else:
                    # second byte is the same as first as 1's complement
                    seq2 = 0xff - ord(seq2)

            if not (seq1 == seq2 == sequence):
                # consume data anyway ... even though we will discard it,
                # it is not the sequence we expected!
                err_msg = 'expected sequence {0}, got (seq1={1}, seq2={2}), \
                          receiving next block, will NAK.'.format(sequence, seq1, seq2)
                self.log(LOG_LEVEL_ERROR, err_msg)
                self.getc(packet_size + 1 + crc_mode)
            else:
                # sequence is ok, read packet
                # packet_size + checksum
                data = self.getc(packet_size + 1 + crc_mode, timeout)
                valid, data = self._verify_recv_checksum(crc_mode, data)

                # valid data, append chunk
                if valid:
                    income_size += len(data)
                    stream.write(data)
                    self.putc(ACK)
                    sequence = (sequence + 1) % 0x100
                    # get next start-of-header byte
                    char = self.getc(1, timeout)
                    continue

            # something went wrong, request retransmission
            self.log(LOG_LEVEL_WARN, 'recv error: purge, requesting retransmission (NAK)')
            while True:
                # When the receiver wishes to <nak>, it should call a "PURGE"
                # subroutine, to wait for the line to clear. Recall the sender
                # tosses any characters in its UART buffer immediately upon
                # completing sending a block, to ensure no glitches were mis-
                # interpreted.  The most common technique is for "PURGE" to
                # call the character receive subroutine, specifying a 1-second
                # timeout, and looping back to PURGE until a timeout occurs.
                # The <nak> is then sent, ensuring the other end will see it.
                data = self.getc(1, timeout=1)
                if data is None:
                    break
                assert False, data
            self.putc(NAK)
            # get next start-of-header byte
            char = self.getc(1, timeout)
            continue

    def _verify_recv_checksum(self, crc_mode, data):
        if crc_mode:
            _checksum = bytearray(data[-2:])
            their_sum = (_checksum[0] << 8) + _checksum[1]
            data = data[:-2]

            our_sum = self.calc_crc(data)
            valid = bool(their_sum == our_sum)
            if not valid:
                self.log(LOG_LEVEL_WARN, 'recv error: checksum fail (theirs={0:04x}, ours={1:04x}), '.format(their_sum, our_sum))
        else:
            _checksum = bytearray([data[-1]])
            their_sum = _checksum[0]
            data = data[:-1]

            our_sum = self.calc_checksum(data)
            valid = their_sum == our_sum
            if not valid:
                self.log(LOG_LEVEL_WARN, 'recv error: checksum fail (theirs={0:02x}, ours={1:02x})'.forma(their_sum, our_sum))
        return valid, data

    def calc_checksum(self, data, checksum=0):
        '''
        Calculate the checksum for a given block of data, can also be used to
        update a checksum.

            >>> csum = modem.calc_checksum('hello')
            >>> csum = modem.calc_checksum('world', csum)
            >>> hex(csum)
            '0x3c'

        '''
        if platform.python_version_tuple() >= ('3', '0', '0'):
            return (sum(data) + checksum) % 256
        else:
            return (sum(map(ord, data)) + checksum) % 256

    def calc_crc(self, data, crc=0):
        '''
        Calculate the Cyclic Redundancy Check for a given block of data, can
        also be used to update a CRC.

            >>> crc = modem.calc_crc('hello')
            >>> crc = modem.calc_crc('world', crc)
            >>> hex(crc)
            '0xd5e3'

        '''
        for char in bytearray(data):
            crctbl_idx = ((crc >> 8) ^ char) & 0xff
            crc = ((crc << 8) ^ self.crctable[crctbl_idx]) & 0xffff
        return crc & 0xffff

port = 0
prcss_debug = False
modem_debug = False

def getc(size, timeout=1):
    port.timeout = timeout
    ret = port.read(size)
    if modem_debug:
        print "read:",
        print ret,
        print ", {0} bytes".format(len(ret))
    return ret

def putc(data, timeout=1):
    port.timeout = timeout
    len = port.write(data)
    if modem_debug:
        print "{0} bytes writen".format(len)
    return len

def send_file(filename):
    modem = XMODEM(getc, putc, mode="ymodem")
    return modem.send([filename])

def assert_response(patterns, timeout):
    port.timeout = 0.01
    timeout_tick = time.time() + timeout
    while True:
        line = port.readline()
        if prcss_debug and len(line):
            print line
        for pattern in patterns:
            if pattern in line:
                return True
        if time.time() > timeout_tick:
            return False

def print_usage():
    print "Usage: {0} port [-a app.bin] [-b bootloader.bin] [-d driver.bin] [--bootloader-baudrate 921600] [--application-baudrate 115200] [--noboot]\n".format(sys.argv[0])
    print "  examples: python {0} /dev/ttyUSB0 -a app.bin, to update app only".format(sys.argv[0])
    print "          : python {0} /dev/ttyUSB1 -b bootloader.bin -a app.bin, to update bootloader and app".format(sys.argv[0])
    print "          : python {0} /dev/ttyUSB0 -a app.bin -d driver.bin, to update app and driver".format(sys.argv[0])

if len(sys.argv) < 4:
    print_usage()
    sys.exit(1)

device = sys.argv[1]

updates=[]
bootloader_baudrate=921600
application_baudrate=921600
bootapp = True
hardreboot = False

i = 2
update = 0
while i < len(sys.argv):
    if sys.argv[i].startswith("0x") and (i + 1) < len(sys.argv):
        if os.path.isfile(sys.argv[i+1]) == False:
            sys.stderr.write("error: file {0} does not exist\n".format(sys.argv[i+1]))
            sys.exit(1)
        updates.append([sys.argv[i], sys.argv[i+1]])
        update += 1
        i += 1
    elif sys.argv[i] == "--bootloader-baudrate" and (i + 1) < len(sys.argv):
        try:
            bootloader_baudrate = int(sys.argv[i+1])
        except:
            sys.stderr.write("error: invalid bootload baudrate value {0}\n".format(sys.argv[i+1]))
            sys.exit(1)
        i += 1
    elif sys.argv[i] == "--application-baudrate" and (i + 1) < len(sys.argv):
        try:
            application_baudrate = int(sys.argv[i+1])
        except:
            sys.stderr.write("error: invalid bootload baudrate value {0}\n".format(sys.argv[i+1]))
            sys.exit(1)
        i += 1
    elif sys.argv[i] == "--hardreset":
        hardreboot = True
    elif sys.argv[i] == "--noappboot":
        bootapp = False
    i += 1

if update <= 0:
    sys.exit(0)

try:
    port = serial.Serial(device, bootloader_baudrate, timeout = 0.05)
    port.setRTS(False)
except:
    sys.stderr.write("error: unable to open {0}\n".format(device))
    sys.exit(1)

if hardreboot == False:
    port.write("a\r\n") #abort potential ongoing YMODEM transfer
    port.flushInput()
    port.write("help\r\n")
    if assert_response(["bootloader", "read [address] [size]"], 1) == False:
        if application_baudrate != bootloader_baudrate:
            port.baudrate = application_baudrate
            port.flushInput()
            port.write("dummycmd_for_flushing_purpose\r\n")
            time.sleep(0.1)
        port.write("reboot\r\n")
        if assert_response(["reboot"] , 1) == False:
            sys.stderr.write("error: failed to reboot the board, it did not respond to \"reboot\" command\n")
            sys.exit(1)
        if application_baudrate != bootloader_baudrate:
            port.baudrate = bootloader_baudrate
        time.sleep(0.11); port.write("   ") #0.11s
        time.sleep(0.03); port.write("   ") #0.14s
        time.sleep(0.03); port.write("   ") #0.17s
        time.sleep(0.03); port.write("   ") #0.20s
        time.sleep(0.03); port.write("   ") #0.23s
        time.sleep(0.03); port.write("   \r\n") #0.26s
        if assert_response(["ootloader", "BOOTLODER"], 1) == False:
            sys.stderr.write("error: failed to enter bootloader\n")
            sys.exit(1)
else:
    port.setRTS(True)
    time.sleep(0.1)
    port.setRTS(False)
    time.sleep(0.11); port.write("   ") #0.11s
    time.sleep(0.03); port.write("   ") #0.14s
    time.sleep(0.03); port.write("   ") #0.17s
    time.sleep(0.03); port.write("   ") #0.20s
    time.sleep(0.03); port.write("   ") #0.23s
    time.sleep(0.03); port.write("   \r\n") #0.26s
    if assert_response(["ootloader"], 1) == False:
        sys.stderr.write("error: failed to enter bootloader\n")
        sys.exit(1)
port.flushInput()

failed_num = 0
for [addr, image] in updates:
    status_str = "updating {0} with {1} @ address {2} ...".format(device, image, addr)
    print status_str
    port.write("write {0}\r\n".format(addr))
    if assert_response(["Waiting for the file to be sent"], 1) == False:
        sys.stderr.write("error: waiting for target to enter into YMODEM recived mode failed\n")
        sys.exit(1)
    result = send_file(image)
    if result == True:
        status_str = status_str + " succeed"
    else:
        status_str = status_str + " failed"
        failed_num += 1
    print status_str

if bootapp and failed_num == 0:
    port.write("boot\r\n")
    assert_response(["Booting......"], 1)
port.close()
sys.exit(failed_num)

