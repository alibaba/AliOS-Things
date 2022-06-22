import ujson as json
import time
import _thread
import urandom as random
import struct

crc_ibm_table = [
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
]
def crc_ibm (data):
    global crc_ibm_table
    crc = 0x0000
    lut = 0
    for byte in data:
        lut = (crc ^ (byte)) & 0xFF
        crc = (crc >> 8) ^ crc_ibm_table[lut]
    return crc

class UPload:

    STATE_MQTT_UPLOAD_NONE = -0x2115
    STATE_MQTT_UPLOAD_REQUEST_INIT = -0x2116
    STATE_MQTT_UPLOAD_IS_UPLOADING = -0x2117
    STATE_MQTT_UPLOAD_REQUEST_CANCEL = -0x2118
    STATE_MQTT_UPLOAD_FINISHED = -0x2125
    STATE_MQTT_UPLOAD_FAILED = -0x2121
    STATE_MQTT_UPLOAD_FAILED_TIMEOUT = -0x2122
    STATE_MQTT_UPLOAD_CANCEL_FAILED = -0x2120
    STATE_MQTT_UPLOAD_CANCEL_SUCCESS = -0x2119
    STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK = -0x2124
    STATE_MQTT_UPLOAD_UPTASK_IS_NULL = -0x210D
    AIOT_MQTT_UPLOADRECV_INIT_REPLY = 0
    AIOT_MQTT_UPLOADRECV_UPLOAD_REPLY = 1
    AIOT_MQTT_UPLOADRECV_CANCEL_REPLY = 2
    UPLOAD_FILE_FAILED_BLOCK_CRC = 78121
    UPLOAD_FILE_FAILED_WHOLE_CHECK = 78125
    UPLOAD_FILE_HAVE_FINISHED = 78124
    STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL = -0x2101
    MQTT_UPLOAD_DEFAULT_SEND_MAX_SIZE = 128 * 1024
    MQTT_UPLOAD_DEFAULT_SEND_MIN_SIZE = 256

    UPLOAD_FILE_OK = 200
    UPLOAD_FILE_PARAMETER_ERROR =460
    UPLOAD_FILE_FILE_BLOCK_DUPLICATION = 78126


    default_block_size = ( 2 * 1024)
    retry_max_value = 5
    rsp_timeout = 2000

    def __init__(self, lk_handle = None):
        if lk_handle == None:
            raise ValueError("lk_handle wrong")

        self.conflictStrategy = "overwrite"

        self.up_task_list = list()
        self.lk_handle = lk_handle

        self.uptask_lock = _thread.allocate_lock()
        self.open_lock = _thread.allocate_lock()
        self.file_data_desc_lock = _thread.allocate_lock()

        self.result = dict()

        self.file_data_desc_list = list()

    def upload_subsribe(self):
        # print("start to subsribe") #test point
        topic01 = self.lk_handle.TOPIC["upload_init_reply"].format(self.lk_handle.productKey, self.lk_handle.deviceName)
        topic02 = self.lk_handle.TOPIC["upload_file_reply"].format(self.lk_handle.productKey, self.lk_handle.deviceName)
        topic03 = self.lk_handle.TOPIC["upload_cancel_reply"].format(self.lk_handle.productKey, self.lk_handle.deviceName)
        # print("subsribe:", topic01) #test point
        self.lk_handle.lk_subscribe(topic01)
        # print("subsribe:", topic02) #test point
        self.lk_handle.lk_subscribe(topic02)
        # print("subsribe:", topic03) #test point
        self.lk_handle.lk_subscribe(topic03)

        sub_node01 = self.lk_handle.__create_subscribe_node(topic01,
                                self._upload_receive_init_response_handler)

        self.lk_handle.__subsribe_list_insert(sub_node01)

        sub_node02 = self.lk_handle.__create_subscribe_node(topic02,
                                self._upload_receive_upload_response_handler)
        self.lk_handle.__subsribe_list_insert(sub_node02)

        sub_node03 = self.lk_handle.__create_subscribe_node(topic03,
                                self._upload_receive_cancel_response_handler)
        self.lk_handle.__subsribe_list_insert(sub_node03)


    def upload_file_task_init(self, filename, filesize, mode, digest, uuid, read_data_handler, userdata):
        up_task = dict()
        up_task['file_size'] = filesize
        up_task['failed_count'] = 0
        up_task['file_offset'] = 0
        up_task['status'] = self.STATE_MQTT_UPLOAD_REQUEST_INIT
        up_task['read_data_handler'] = read_data_handler              # to be designed
        up_task['userdata'] = userdata                                # no crc
        up_task['is_rsp'] = 0
        up_task['is_destory'] = 1
        up_task['send_last_time'] = 0
        up_task['file_name'] = filename
        up_task['mode'] = mode
        up_task['crc64'] = digest   # no crc
        up_task['uuid'] = uuid
        up_task['upload_id'] = None
        up_task['rsp_code'] = None
        up_task['packet'] = dict()
        up_task['packet']['type'] = None
        up_task['packet']['desc'] = dict()
        up_task['packet']['desc']['file_name'] = None
        up_task['packet']['desc']['upload_id'] = None
        up_task['data'] = None
        up_task['block_size'] = 0
        return up_task

    def upload_file_task_insert(self, up_task):
        self.up_task_list.append(up_task)

    def file_data_desc_insert(self, filename, data, data_len):
        #print("file name:", filename)  # test point
        self.file_data_desc_lock.acquire()
        found = 0
        for index in range(len(self.file_data_desc_list)):
            if (self.file_data_desc_list[index]['file_name'] == filename):
                found = 1
                break
        file_data_desc = dict()
        if data_len == 0:
            file_data_desc['file_name'] = filename
            file_data_desc['data'] = None
            file_data_desc['data_len'] = 0
            file_data_desc['file_path'] = data
            #print("file path:", data) # test point
            file_data_desc['file_len'] = self.get_file_size(data)
            #print("file path file size:", file_data_desc['file_len']) # test point

        else:
            file_data_desc['file_name'] = filename
            file_data_desc['data'] = data
            file_data_desc['data_len'] = data_len
            file_data_desc['file_path'] = None
            file_data_desc['file_len'] = data_len

        if (found == 1):   # file already exits in the list
            self.file_data_desc_list[index] = file_data_desc
        else:     # file not exits in the list
            self.file_data_desc_list.append(file_data_desc)

        self.file_data_desc_lock.release()

        return file_data_desc['file_len']

    def upload_block_size_is_valid(self, block_size, is_final):
        if block_size == 0:
            return 0
        if (is_final == 1):
            if block_size > self.MQTT_UPLOAD_DEFAULT_SEND_MAX_SIZE:
                return 0
            else:
                return 1
        else:
            if ((block_size < self.MQTT_UPLOAD_DEFAULT_SEND_MIN_SIZE) or (block_size > self.MQTT_UPLOAD_DEFAULT_SEND_MAX_SIZE)):
                return 0
            else:
                return 1

    def upload_read_data_handler(self, packet=None, up_task=None, size=0, userdata=None):
        read_len = 0
        self.file_data_desc_lock.acquire()
        found = 0
        if up_task != None:
            for index in range(len(self.file_data_desc_list)):
                file_data_desc = self.file_data_desc_list[index]
                if file_data_desc['file_name'] == up_task['file_name']:
                    found = 1
                    if (packet['desc']['code'] == self.UPLOAD_FILE_OK):
                        if (up_task!=None  and size != 0) :
                            read_size = size
                            file_name = packet['desc']['file_name']
                            if (file_data_desc['data'] != None):
                                offset = packet['desc']['file_offset']
                                if (read_size > file_data_desc['data_len'] - offset):
                                    read_len = file_data_desc['data_len'] - offset
                                else:
                                    read_len = read_size
                                up_task['data'] = file_data_desc['data'][offset: offset+read_len]    # probably is not ok
                            else:
                                with open(file_data_desc['file_path'], 'rb') as f:
                                    offset  = packet['desc']['file_offset']
                                    f.seek(offset,0)
                                    # print("read_size:", read_size)
                                    up_task['data'] = f.read(read_size)
                                    read_len = len(up_task['data'])
                                    # print("Open {} read at: {},Read_len: {}".format(file_name, offset, read_len)) # test point
                    break
        self.file_data_desc_lock.release()
        #if found == 0:
        #    raise ValueError("file not found from read data handler!")
        return read_len

    def upload_recv_response_process(self, type, msg, up_task):
        # print("type:", type) #test point
        if (type == self.AIOT_MQTT_UPLOADRECV_CANCEL_REPLY):
            if msg['desc']['code'] == self.UPLOAD_FILE_OK:
                up_task['status'] = self.STATE_MQTT_UPLOAD_CANCEL_SUCCESS
                up_task['failed_count'] = 0
            elif msg['desc']['code'] == self.UPLOAD_FILE_FILE_BLOCK_DUPLICATION:
                # print("UPLOAD_FILE_FILE_BLOCK_DUPLICATION resend")
                res = self.upload_resend_pub(up_task)
                if res != 0:
                    up_task['status'] = self.STATE_MQTT_UPLOAD_CANCEL_FAILED
            else:
                up_task['status'] = self.STATE_MQTT_UPLOAD_CANCEL_FAILED

            if ( (up_task['read_data_handler'] != None) and \
                (up_task['status'] == self.STATE_MQTT_UPLOAD_CANCEL_FAILED or up_task['status'] == self.STATE_MQTT_UPLOAD_CANCEL_SUCCESS )  ):
                up_task['read_data_handler'](msg, None, 0, up_task['userdata'])

            else:
                return

        elif type == self.AIOT_MQTT_UPLOADRECV_INIT_REPLY or type == self.AIOT_MQTT_UPLOADRECV_UPLOAD_REPLY:
            if (msg['desc']['code'] == self.UPLOAD_FILE_OK):
                up_task['is_rsp'] = 1
                up_task['failed_count'] = 0
                up_task['file_offset'] = msg['desc']['file_offset'] + msg['desc']['block_size']
                if (msg['desc']['complete'] == 1):
                    up_task['status'] = self.STATE_MQTT_UPLOAD_FINISHED
                    # print("Complete upload, total size:{}".format(up_task['file_offset'])) #test point
                    return
                up_task['status'] = self.STATE_MQTT_UPLOAD_IS_UPLOADING
                msg['desc']['file_offset'] = msg['desc']['file_offset'] + msg['desc']['block_size']
                # print("status:", up_task['status'])  #test point
                # print("UPLOAD_FILE_OK") #test point

                data_len = 0
                if (up_task['read_data_handler'] != None):
                    data_len = up_task['read_data_handler'](msg, up_task, self.default_block_size, up_task['userdata'])
                else:
                    return

                # print("Read data_len:{}".format(data_len)) #test point
                # print("data to upload:", up_task['data']) #test point

                is_final = 0
                if (up_task['file_offset'] + data_len) >= up_task['file_size']:
                    is_final = 1
                # print('data_len:', data_len, 'up_task file_offset:', up_task['file_offset'], \
                #        'up_task file_size:', up_task['file_size'], "is final:", is_final)                #test point

                block_size_is_valid = self.upload_block_size_is_valid(data_len, is_final)
                if ( (data_len != 0) and (up_task['data'] != None) and (block_size_is_valid == 1) ):
                    #print('upload_recv_response_process send_block_file:','block_size:', up_task['block_size'],\
                    #    'packet:', up_task['packet'])
                    # print('\nupload_recv_response_process send_block_file:') # test point
                    res = self.upload_send_block_file(up_task, up_task['data'], data_len, up_task['file_offset'], 0)
                    if res == 0:
                        up_task['block_size'] = data_len
                    else:
                        print("STATE_MQTT_UPLOAD_FAILED! upload_send_block_file res: ", res)
                        up_task['status'] = self.STATE_MQTT_UPLOAD_FAILED
                else:
                    print("STATE_MQTT_UPLOAD_FAILED! data_len %s, up_task['data']: %s, block_size_is_valid: %s" %\
                                                                        (data_len, up_task['data'], block_size_is_valid))
                    up_task['status'] = self.STATE_MQTT_UPLOAD_FAILED
                return

            elif msg['desc']['code'] == self.UPLOAD_FILE_FAILED_BLOCK_CRC:
                # print("UPLOAD_FILE_FAILED_BLOCK_CRC resend") # test point
                self.upload_resend_pub(up_task)

            else:
                if msg['desc']['code'] == self.UPLOAD_FILE_FAILED_WHOLE_CHECK:
                    up_task['status'] = self.STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK
                elif  msg['desc']['code'] == self.UPLOAD_FILE_HAVE_FINISHED:
                    up_task['status'] = self.STATE_MQTT_UPLOAD_FINISHED
                else:
                    # print("upload fail!!, code:", msg['desc']['code']) # test point
                    print("STATE_MQTT_UPLOAD_FAILED! msg desc code:", msg['desc']['code'])
                    up_task['status'] = self.STATE_MQTT_UPLOAD_FAILED

                if up_task['read_data_handler'] != None:
                    up_task['read_data_handler'](msg, None, 0, up_task['userdata'])

        else:
            pass

    def _upload_receive_init_response_handler(self, msg_data):
        # print("file up init reply received:", msg_data)  #test point
        res = 0
        msg = dict()
        msg['type'] = self.AIOT_MQTT_UPLOADRECV_INIT_REPLY
        msg['desc'] = dict()
        msg['desc']['file_name'] = msg_data['data']['fileName']
        msg['desc']['upload_id'] = msg_data['data']['uploadId']
        msg['desc']['message'] = msg_data['message']
        msg['desc']['code'] = msg_data['code']
        msg['desc']['file_size'] = msg_data['data']['fileSize']
        msg['desc']['file_offset'] = msg_data['data']['offset']
        msg['desc']['complete'] = 0
        self.uptask_lock.acquire()
        for index in range(len(self.up_task_list)):
            if (msg['desc']['file_name'] != None) and  (msg['desc']['file_name'] == self.up_task_list[index]['file_name']):
                if msg['desc']['upload_id'] == None:
                    print("[ERROR] Receive uploadID is NULL")
                else:
                    self.up_task_list[index]['upload_id'] = msg['desc']['upload_id']
                    msg['desc']['block_size'] = 0

                self.up_task_list[index]['is_rsp'] = 1
                self.up_task_list[index]['rsp_code'] = msg['desc']['code']

                self.up_task_list[index]['packet']['desc']['file_name'] = msg['desc']['file_name']
                self.up_task_list[index]['packet']['desc']['upload_id'] = msg['desc']['upload_id']
                self.up_task_list[index]['packet']['desc']['file_size'] = msg['desc']['file_size']
                self.up_task_list[index]['packet']['desc']['file_offset'] = msg['desc']['file_offset']
                self.up_task_list[index]['packet']['desc']['block_size'] = msg['desc']['block_size']
                self.up_task_list[index]['packet']['desc']['code'] = msg['desc']['code']

                # print("file up init reply msg matched!")  #test point
                self.upload_recv_response_process(self.AIOT_MQTT_UPLOADRECV_INIT_REPLY, msg, self.up_task_list[index])
                self.uptask_lock.release()
                return
        self.uptask_lock.release()
        return
        # print("Recv file name{} ".format(msg['desc']['file_name']))  #test point

    def _upload_receive_upload_response_handler(self, msg_data):
        # print("file up upload reply received:", msg_data)  #test point
        res = 0
        msg = dict()
        msg['type'] = self.AIOT_MQTT_UPLOADRECV_UPLOAD_REPLY

        msg['desc'] = dict()
        msg['desc']['upload_id'] = msg_data['data']['uploadId']
        msg['desc']['message'] = msg_data['message']
        msg['desc']['code'] = msg_data['code']
        #msg['desc']['file_size'] = msg_data['data']['fileSize']
        msg['desc']['file_offset'] = msg_data['data']['offset']
        msg['desc']['block_size'] = msg_data['data']['bSize']
        try:
            complete = msg_data['data']['complete']
            if complete == True:
                msg['desc']['complete'] = 1
            else:
                msg['desc']['complete'] = 0
        except:
            msg['desc']['complete'] = 0xFF

        self.uptask_lock.acquire()
        for index in range(len(self.up_task_list)):
            if (msg['desc']['upload_id'] != None) and (self.up_task_list[index]['upload_id'] != None) and \
                (msg['desc']['upload_id'] == self.up_task_list[index]['upload_id']):
                # print("upload id matched!") #test point
                # print("complete:", msg['desc']['complete']) #test point
                if (self.up_task_list[index]['status'] != self.STATE_MQTT_UPLOAD_IS_UPLOADING):
                    # print("not in uploading!!")  #test point
                    # print("status:", self.up_task_list[index]['status'])  #test point
                    break
                if (msg['desc']['file_offset'] != self.up_task_list[index]['file_offset']) or (msg['desc']['block_size'] != self.up_task_list[index]['block_size']):
                    #print("Receive upload ACK error,file_offset:{}, block_szie:{}".format(msg['desc']['file_offset'], msg['desc']['block_size']))
                    pass
                else:
                    msg['desc']['file_name'] = self.up_task_list[index]['file_name']
                    self.up_task_list[index]['is_rsp'] = 1
                    self.up_task_list[index]['rsp_code'] = msg['desc']['code']
                    self.upload_recv_response_process(self.AIOT_MQTT_UPLOADRECV_UPLOAD_REPLY, msg, self.up_task_list[index])
                break
        self.uptask_lock.release()


    def _upload_receive_cancel_response_handler(self, msg_data):
        # print("file up cancel reply received:", msg_data) #test point
        res = 0
        msg = dict()
        msg['type'] = self.AIOT_MQTT_UPLOADRECV_CANCEL_REPLY
        msg['desc'] = dict()
        msg['desc']['upload_id'] = msg_data['data']['uploadId']
        msg['desc']['message'] = msg_data['message']
        msg['desc']['code'] = msg_data['code']

        self.uptask_lock.acquire()
        for index in range(len(self.up_task_list)):
            if (msg['desc']['upload_id'] != None) and (self.up_task_list[index]['upload_id'] != None) and \
                (msg['desc']['upload_id'] == self.up_task_list[index]['upload_id']):

                if self.up_task_list[index]['status'] == self.STATE_MQTT_UPLOAD_NONE:
                    break
                self.up_task_list[index]['is_rsp'] = 1
                self.up_task_list[index]['rsp_code'] = msg['desc']['code']
                self.upload_recv_response_process(self.AIOT_MQTT_UPLOADRECV_CANCEL_REPLY, msg, self.up_task_list[index])
                break
        self.uptask_lock.release()

    def upload_send_request_init(self, filename, filesize, mode, digest, uuid):
        init_topic = self.lk_handle.TOPIC['upload_init'].format(self.lk_handle.productKey, self.lk_handle.deviceName)

        param_dic = dict()
        payload_dic = dict()
        param_dic['fileName'] = str(filename)
        param_dic['fileSize'] = filesize
        param_dic['conflictStrategy'] = str(mode)  # self.conflictStrategy
        param_dic['initUid'] = uuid

        id = self.lk_handle.global_alink_id_next()
        payload_dic['id'] = str(id)
        payload_dic['params'] = param_dic

        payload = json.dumps(payload_dic)
        payload = payload.replace('\\','')

        payload = '{\"id\":\"%s\",\"params\":{\"fileName\":\"%s\",\"fileSize\":%s,\"conflictStrategy\":\"%s\",\"initUid\":\"%s\"}}' % \
            (id, filename, filesize, mode, uuid)
        payload = payload.replace(' ', '')

        # print("publish", json.dumps(payload)) #test point
        return self.lk_handle.lk_publish(init_topic, payload)

    def get_file_size(self, filepath):
        size = 0
        try:
            with open(filepath, 'rb') as f:
                size = f.seek(0,2)
        except:
            return -1
        return size

    def upload_open_stream(self, filename, packet):
        res = 0
        file_flag = 0
        self.open_lock.acquire()
        self.uptask_lock.acquire()
        index = 0
        for index in range(len(self.up_task_list)):
            if (self.up_task_list[index]['file_name'] == filename):
                self.up_task_list[index]['is_rsp'] = 0
                res = self.upload_send_request_init(self.up_task_list[index]['file_name'], self.up_task_list[index]['file_size'], \
                                                 self.up_task_list[index]['mode'],   self.up_task_list[index]['crc64'], self.up_task_list[index]['uuid'])
                self.up_task_list[index]['send_last_time'] = int(time.time() * 1000)
                self.up_task_list[index]['failed_count'] = 0
                self.up_task_list[index]['is_destory'] = 0

                file_flag = 1
                break
        self.uptask_lock.release()
        if file_flag != 1:
            self.open_lock.release()
            return self.STATE_MQTT_UPLOAD_UPTASK_IS_NULL

        if (res != 0):
            self.up_task_list[index]['is_destory']  = 1
            self.open_lock.release()
            return res

        i = 0
        while self.up_task_list[index]['is_rsp'] == 0:
            # print('waiting cloud response...') #test point
            time.sleep(0.02)
            i = i+1
            if i == 1000:
                print('Wait init response timeout.')
                break

        if self.up_task_list[index]['is_rsp'] == 0:
            print("STATE_MQTT_UPLOAD_FAILED! request init fail")
            res = self.STATE_MQTT_UPLOAD_FAILED
            self.up_task_list[index]['is_destory'] =1
            self.open_lock.release()
            return res

        if (self.up_task_list[index]['rsp_code'] != self.UPLOAD_FILE_OK ):
            print("STATE_MQTT_UPLOAD_FAILED! request init fail")
            res = self.STATE_MQTT_UPLOAD_FAILED
            self.up_task_list[index]['is_destory'] =1
            self.open_lock.release()
            return res

        self.up_task_list[index]['is_destory'] =1
        self.open_lock.release()
        return res


    def upload_send_block_file(self, up_task, data, block_size, file_offset, iscomplete=0):
        res = 0
        if (up_task == None):
            return self.STATE_MQTT_UPLOAD_UPTASK_IS_NULL

        if (data == None):
            return self.STATE_MQTT_UPLOAD_MQTT_HANDLE_IS_NULL

        if iscomplete == 1:
            str_isComplete ="true"
        else:
            str_isComplete ="false"

        id = self.lk_handle.global_alink_id_next()

        head_payload = '{\"id\":"%s\",\"params\":{\"uploadId\":\"%s\",\"offset\":%s,\"bSize\":%s,\"isComplete\":%s}}' % \
                        (id, up_task['upload_id'], file_offset, block_size, str_isComplete )
        head_payload = head_payload.replace(' ', '')
        #print("head payload:", head_payload)
        topic = self.lk_handle.TOPIC['upload_file'].format(self.lk_handle.productKey, self.lk_handle.deviceName)
        head_len = len(head_payload.encode('utf-8'))
        payload_len = up_task['block_size'] + head_len + 4
        if type(data)  == type('str') :
            data = data.encode('utf-8')

        crc16 = crc_ibm(data)
        # print("head_payload:", head_payload.encode('utf-8')) #test point
        # print('crc16:', crc16) #test point
        # print("head_len", head_len)  #test point
        head_len_net = ((head_len>>8) & 0x00FF) + ((head_len << 8) & 0xff00)

        payload = struct.pack('<h{}s{}sh'.format(head_len,len(data)), head_len_net, head_payload.encode('utf-8'), data, crc16)
        # print("payload is:", payload) #test point
        #print("file block len:", len(data))
        #print("crc16:", crc16)
        #print('\n')
        res = self.lk_handle.lk_publish(topic, payload)
        return res


    def upload_send_request_cancel(self, up_task):
        res = 0
        params_dic = dict()
        params_dic['uploadId'] = up_task['upload_id']
        params = json.dumps(params_dic)
        payload_dic = dict()
        payload_dic['id'] = str(self.lk_handle.global_alink_id_next())
        payload_dic['params'] = params
        payload = json.dumps(payload_dic)
        topic = self.lk_handle.TOPIC['upload_cancel'].format(self.lk_handle.productKey, self.lk_handle.deviceName)
        res = self.lk_handle.lk_publish(topic, payload)
        return res

    def upload_resend_pub(self, up_task):
        res = 0
        if ((up_task['send_last_time'] == 0) or (up_task['file_size'] == -1) or \
            (up_task['status'] == self.STATE_MQTT_UPLOAD_REQUEST_INIT)):
            return res

        # print("Resend pub", up_task['file_name'])
        up_task['failed_count'] = up_task['failed_count']+1
        if (up_task['failed_count'] > self.retry_max_value):
            up_task['status'] = self.STATE_MQTT_UPLOAD_FAILED_TIMEOUT
            up_task['failed_count'] = 0
            return self.STATE_MQTT_UPLOAD_FAILED_TIMEOUT

        if ((up_task['status'] == self.STATE_MQTT_UPLOAD_IS_UPLOADING) and (up_task["block_size"] != 0)):
            # print("blocksize input:", up_task["block_size"])  #test point
            res = self.upload_send_block_file(up_task, up_task['data'], up_task['block_size'], up_task['file_offset'], 0)
        elif up_task['status'] == self.STATE_MQTT_UPLOAD_REQUEST_CANCEL :
            res = self.upload_send_request_cancel(up_task)
        return res


    def upload_process(self, filename):
        self.result['status'] = self.STATE_MQTT_UPLOAD_NONE
        self.result['err_code'] = 0
        self.uptask_lock.acquire()
        for index in range(len(self.up_task_list)):
            self.result['status'] = self.up_task_list[index]['status']
            self.result['file_name'] = self.up_task_list[index]['file_name']
            self.result['uploadid'] = self.up_task_list[index]['upload_id']
            if self.result['status'] == self.STATE_MQTT_UPLOAD_NONE:
                pass
            elif self.result['status'] == self.STATE_MQTT_UPLOAD_REQUEST_INIT:
                now = int(time.time() * 1000)
                if (now - self.up_task_list[index]['send_last_time']) > 20000:
                    print("STATE_MQTT_UPLOAD_REQUEST_INIT resend")
                    self.up_task_list[index]['send_last_time'] = now
                    self.upload_resend_pub(self.up_task_list[index])

            elif self.result['status'] == self.STATE_MQTT_UPLOAD_IS_UPLOADING:
                now = int(time.time() * 1000)
                if (now - self.up_task_list[index]['send_last_time']) > 20000:
                    print("STATE_MQTT_UPLOAD_IS_UPLOADING resend")
                    self.up_task_list[index]['send_last_time'] = now
                    self.upload_resend_pub(self.up_task_list[index])

            elif self.result['status'] == self.STATE_MQTT_UPLOAD_REQUEST_CANCEL:
                now = int(time.time() * 1000)
                if (now - self.up_task_list[index]['send_last_time']) > 2000:
                    # print("STATE_MQTT_UPLOAD_REQUEST_CANCEL resend")
                    self.up_task_list[index]['send_last_time'] = now
                    self.upload_resend_pub(self.up_task_list[index])

            elif (self.result['status'] == self.STATE_MQTT_UPLOAD_FINISHED) or \
                    self.result['status'] == self.STATE_MQTT_UPLOAD_CANCEL_SUCCESS:

                self.up_task_list[index]['status'] = self.STATE_MQTT_UPLOAD_NONE
                self.result['err_code'] = self.up_task_list[index]['rsp_code']
                # print("Finish upload,Deleted upload task")
                if self.result['file_name'] == filename:
                    while (self.up_task_list[index]['is_destory'] == 0):
                        time.sleep(0.02)
                    del self.up_task_list[index]
                    self.uptask_lock.release()
                    return self.result

            elif (self.result['status'] == self.STATE_MQTT_UPLOAD_CANCEL_FAILED) or \
                    (self.result['status'] == self.STATE_MQTT_UPLOAD_FAILED) or \
                        (self.result['status'] == self.STATE_MQTT_UPLOAD_FAILED_TIMEOUT) or \
                            (self.result['status'] == self.STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK):

                self.up_task_list[index]['status'] = self.STATE_MQTT_UPLOAD_NONE
                self.result['err_code'] = self.up_task_list[index]['rsp_code']

                if self.result['file_name'] == filename:
                    while (self.up_task_list[index]['is_destory'] == 0):
                        time.sleep(0.02)
                    del self.up_task_list[index]
                    self.uptask_lock.release()
                    return self.result
            else:
                pass

        self.uptask_lock.release()
        return self.result
