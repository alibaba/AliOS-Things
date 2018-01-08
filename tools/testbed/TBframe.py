import hashlib

DEBUG = False

CLIENT_DEV     = 'CDEV'
ALL_DEV        = 'ADEV'
DEVICE_LOG     = 'DLOG'
DEVICE_STATUS  = 'DSTU'
DEVICE_CMD     = 'DCMD'
DEVICE_ERASE   = 'DERS'
DEVICE_PROGRAM = 'DPRG'
DEVICE_RESET   = 'DRST'
DEVICE_START   = 'DSTR'
DEVICE_STOP    = 'DSTP'
DEVICE_ALLOC   = 'DALC'
LOG_SUB        = 'LGSB'
LOG_UNSUB      = 'LGUS'
STATUS_SUB     = 'STSB'
STATUS_UNSUB   = 'STUS'
LOG_DOWNLOAD   = 'LGDL'
FILE_BEGIN     = 'FBGN'
FILE_DATA      = 'FDTA'
FILE_END       = 'FEND'
CMD_DONE       = 'CMDD'
CMD_ERROR      = 'CMDE'
HEARTBEAT      = 'HTBT'
TYPE_NONE      = 'NONE'
CLIENT_UUID    = 'CUID'
CLIENT_TAG     = 'CTAG'
REQUEST_LOGIN  = 'RLGI'
CLIENT_LOGIN   = 'CLGI'
SERVER_LOGIN   = 'SLGI'
TERMINAL_LOGIN = 'TLGI'

def is_valid_type(type):
    #frequently used commands
    if type == DEVICE_LOG:
        return True
    if type == DEVICE_STATUS:
        return True
    if type == HEARTBEAT:
        return True
    if type == DEVICE_CMD:
        return True
    if type == CMD_DONE:
        return True
    if type == CMD_ERROR:
        return True
    if type == DEVICE_ERASE:
        return True
    if type == DEVICE_PROGRAM:
        return True
    if type == DEVICE_RESET:
        return True
    if type == DEVICE_START:
        return True
    if type == DEVICE_STOP:
        return True
    #less frequently used commands
    if type == CLIENT_DEV:
        return True
    if type == ALL_DEV:
        return True
    if type == LOG_SUB:
        return True
    if type == LOG_UNSUB:
        return True
    if type == STATUS_SUB:
        return True
    if type == STATUS_UNSUB:
        return True
    if type == LOG_DOWNLOAD:
        return True
    if type == FILE_BEGIN:
        return True
    if type == FILE_DATA:
        return True
    if type == FILE_END:
        return True
    if type == DEVICE_ALLOC:
        return True
    if type == CLIENT_UUID:
        return True
    if type == CLIENT_TAG:
        return True
    if type == REQUEST_LOGIN:
        return True
    if type == CLIENT_LOGIN:
        return True
    if type == SERVER_LOGIN:
        return True
    if type == TERMINAL_LOGIN:
        return True
    return False

def construct(type, value):
    if is_valid_type(type) == False:
        return ''
    frame = '{' + type + ',' + '{0:04d}'.format(len(value)) + ',' + value + '}'
    return frame

def parse(msg):
    sync = False
    type = TYPE_NONE
    length = 0
    value = ''
    while msg != '':
        if len(msg) < 12:
            type = TYPE_NONE
            length = 0
            value = ''
            break;
        #   print(msg)
        for i in range(len(msg)):
            if msg[i] != '{':
                continue
            if (i + 12) > len(msg):
                break;
            if is_valid_type(msg[i+1: i+5]) == False:
                continue
            if msg[i + 5] != ',':
                continue
            if msg[i+6 : i+10].isdigit() == False:
                continue
            if msg[i+10] != ',':
                continue
            sync = True
            if DEBUG and i > 0:
                print("msg:{0}".format(msg))
                print("discard:{0}".format(msg[0:i]))
            msg = msg[i:]
            break
        if sync == False:
            break

        type = msg[1:5]
        length = int(msg[6:10])
        if len(msg) < length + 12:
            type = TYPE_NONE
            length = 0
            value = ''
            break
        if msg[length + 11] != '}':
            sync = False
            if DEBUG: print(msg[0:11] + " Lose sync because of FOOTER error")
            msg = msg[1:]
            continue
        value = msg[11:length+11]
        msg = msg[length+12:]
        break;
    return type, length, value, msg

def hash_of_file(filename):
    h = hashlib.sha1()
    with open(filename, 'rb', buffering=0) as f:
        for b in iter(lambda : f.read(1024), b''):
            h.update(b)
    return h.hexdigest()
