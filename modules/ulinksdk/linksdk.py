import ujson as json
import utime
import _thread
import hmac
import uos
from umqtt.simple2 import MQTTClient
import urandom as random
import time

class LinkSDK:
    # topics 汇总
    TOPIC = {
        # porp
        "prop_post": "/sys/{}/{}/thing/event/property/post", #.format(productKey, DeviceName),
        "prop_post_reply": "/sys/{}/{}/thing/event/property/post_reply", #.format(productKey, DeviceName),
        "prop_set": "/sys/{}/{}/thing/service/property/set", #.format(productKey, DeviceName),

        # event
        "event_post": "/sys/{}/{}/thing/event/{}/post", #.format(productKey, DeviceName, eventID),
        "event_post_reply": "/sys/{}/{}/thing/event/{}/post_reply", #.format(productKey, DeviceName, eventID),

        # Thing Model

        # Pub 用于设备发布属性、事件和扩展信息的数据
        "model_up": "/sys/{}/{}/thing/model/up_raw", #.format(productKey, DeviceName),
        # Sub 云端响应 up_raw
        "model_up_reply": "/sys/{}/{}/thing/model/up_raw_reply", #.format(productKey, DeviceName),

        # Sub 用于设备订阅获取云端设置的属性和云端调用服务的指令
        "model_down": "/sys/{}/{}/thing/model/down_raw", #.format(productKey, DeviceName),
        # Pub 设备端响应 down_raw
        "model_down_reply": "/sys/{}/{}/thing/model/down_raw_reply", #.format(productKey, DeviceName),

        # OTA
        "ota_inform": "/ota/device/inform/{}/{}", #.format(productKey, DeviceName),
        "ota_upgrade": "/ota/device/upgrade/{}/${}", #.format(productKey, DeviceName),
        "ota_progress": "/ota/device/progress/{}/${}", #.format(productKey, DeviceName),
        "get_ota_firmware": "/sys/{}/{}/thing/ota/firmware/get", #.format(productKey, DeviceName),  /sys/+/+/thing/model/down_raw

        # Device Info
        "dev_info_update": "/sys/{}/{}/thing/deviceinfo/update", #.format(productKey, DeviceName),
        "dev_info_update_reply": "/sys/{}/{}/thing/deviceinfo/update_reply", #.format(productKey, DeviceName),
        "dev_info_delete": "/sys/{}/{}/thing/deviceinfo/delete", #.format(productKey, DeviceName),
        "dev_info_delete_reply": "/sys/{}/{}/thing/deviceinfo/delete_reply", #.format(productKey, DeviceName),

        # NTP clock
        "ntp_req": "/ext/ntp/{}/{}/request", #.format(productKey, DeviceName),
        "ntp_res": "/ext/ntp/{}/{}/response", #.format(productKey, DeviceName),

        "shadow_update": "/shadow/update/{}/{}", #.format(productKey, DeviceName),
        "shadow_get": "/shadow/get/{}/{}", #.format(productKey, DeviceName),

        "config_push": "/sys/{}/{}/thing/config/push", #.format(productKey, DeviceName),
        "config_get": "/sys/{}/{}/thing/config/get", #.format(productKey, DeviceName),
        "config_get_reply": "/sys/{}/{}/thing/config/get_reply", #.format(productKey, DeviceName),
        # broadcast
        "broadcast": "/broadcast/{}/{}", #.format(productKey, DeviceName),

        # MQTT upload file topic
        "upload_init": "/sys/{}/{}/thing/file/upload/mqtt/init",         #.format(productKey, DeviceName)
        "upload_file": "/sys/{}/{}/thing/file/upload/mqtt/send",         #.format(productKey, DeviceName)
        "upload_cancel": "/sys/{}/{}/thing/file/upload/mqtt/cancel",     #.format(productKey, DeviceName)
        "upload_init_reply": "/sys/{}/{}/thing/file/upload/mqtt/init_reply",      #.format(productKey, DeviceName)
        "upload_file_reply": "/sys/{}/{}/thing/file/upload/mqtt/send_reply",      #.format(productKey, DeviceName)
        "upload_cancel_reply": "/sys/{}/{}/thing/file/upload/mqtt/cancel_reply"    #.format(productKey, DeviceName)
    }
     # 事件类型(eventType)
    ON_CONNECT = 1
    ON_CLOSE = 2
    ON_DISCONNECT = 3
    ON_EVENT = 4
    ON_SERVICE = 5
    ON_SUBCRIBE = 6
    ON_PROPS = 7
    ON_ERROR = 8
    ON_REPLY = 9
    ON_RAWDATA = 10
    ON_REGISTER = 11
    ON_CB_MAX = 12
    g_global_lock = _thread.allocate_lock()
    alink_id = 0
    def __init__(self):
        self.clean_session = False

        self.clientID = '13225554098'        # 自定义字符（不超过64）
        self.mqtt_client = None
        self.password = None
        self.port = 1883

        self.ntpcallback = None

        self.NTPtime = dict()

        self.NTPtime_Received = False

        self.oncallback = None
        self.connected = False

        self.subscribe_list = list()
        self.up_handle = None
        self.dynreg_handle = None

        self.sub_list_lock = _thread.allocate_lock()

        self.exit = False

    def global_alink_id_next(self):
        self.g_global_lock.acquire()
        self.alink_id = self.alink_id+1
        if (self.alink_id < 0):
            self.alink_id = 0
        self.g_global_lock.release()
        return self.alink_id

    def __check_secret(self, deviceName):
        try:
            with open("/secret.json", "r", encoding="utf-8") as f:
                secret_data = json.load(f)
        except Exception as e:
            print("[ERROR] Device Secret File Open failed : %s" % str(e))
        device_secret = secret_data.get(deviceName, None)
        if device_secret != None:
            return True
        return False

    def lk_register(self, deviceinfo, callback):
        if "secret.json" in uos.listdir("/"):
            print("found secret file")
            if self.__check_secret(deviceinfo['deviceName']):
                callback("[INFO] device is already activated")
                return 0
        if self.dynreg_handle == None:
            from ulinksdk.dynreg import DYNREG
            self.dynreg_handle = DYNREG()
        return self.dynreg_handle.register(deviceinfo,callback)


    def lk_connect(self,key_info):
        # param check
        if 'region' not in key_info:
            raise ValueError("region wrong!")
        if 'productKey' not in key_info:
            raise ValueError("product key wrong!")
        if 'deviceName' not in key_info:
            raise ValueError("device name wrong!")
        if ('deviceSecret' not in key_info) and ('productSecret' not in key_info):
            raise ValueError("device secret & product secret are both empty!")
        if 'keepaliveSec' not in key_info:
            raise ValueError("keep alive wrong!")

        self.region = key_info['region']
        self.productKey = key_info['productKey']
        self.deviceName = key_info['deviceName']
        self.deviceSecret = key_info['deviceSecret']
        self.productSecret = None
        if 'productSecret' in key_info:
            self.productSecret = key_info['productSecret']
        self.keepAlive = 300
        #self.keepAlive = key_info['keepaliveSec']
        self.username = "{}&{}".format(self.deviceName, self.productKey)
        self.__connect_main()
        if self.connected == True:
            postProps_node = self.__create_subscribe_node(self.TOPIC["prop_set"].format(self.productKey, self.deviceName),
                                                    self.oncallback[self.ON_PROPS])
            self.__subsribe_list_insert(postProps_node)

        #_thread.start_new_thread(self.__connect_main, ())
        _thread.start_new_thread(self.__listen, ())
        return 0


    def __subsribe_list_insert(self, listnode):
        self.sub_list_lock.acquire()
        for index in range(len(self.subscribe_list)):
            if self.subscribe_list[index]['topic'] == listnode['topic']:
                self.subscribe_list[index]['handler'] = listnode['handler']
                self.sub_list_lock.release()
                return
        self.subscribe_list.append(listnode)
        self.sub_list_lock.release()
        return

    def __formatConnectInfo(self, deviceSecret):
        secret = deviceSecret
        mqt_id = "{}|securemode=3,signmethod=hmacsha256|".format(self.clientID)
        hmac_msg = "clientId{}deviceName{}productKey{}".format(self.clientID, self.deviceName, self.productKey)
        return mqt_id, secret, hmac_msg

    def __default_recv_handler(self, topic, msg, retained, status):
        # print("Subscribe Recv: Topic={},Msg={}".format(topic.decode(), msg.decode()))
        try:
            msg_data = json.loads(msg.decode())
        except:
            msg_data = msg
        self.sub_list_lock.acquire()
        for node in self.subscribe_list:
            if node['topic'] == topic.decode():
                if (node['handler'] != None):
                    node['handler'](msg_data)
                    break
        self.sub_list_lock.release()
        service_topic = "/sys/+/+/thing/service/+"
        if (self.__compare_topic(service_topic, topic.decode())):
            if (self.oncallback[self.ON_SERVICE] != None):
                self.oncallback[self.ON_SERVICE](msg_data)

    def __compare_topic(self, topic01, topic02):
        topic01_split = topic01.split('/')
        topic02_split = topic02.split('/')
        topic01_len = len(topic01_split)
        topic02_len = len(topic02_split)
        if (topic01_len == topic02_len):
            for i in range(topic01_len):
                if (topic01_split[i] != topic02_split[i])  and (topic01_split[i] != '+'):
                    return False
            return True
        else:
            return False

    def __connect_main(self):
        ssl = False
        mqt_id, secret, hmac_msg = self.__formatConnectInfo(self.deviceSecret)
        self.__mqtt_connect(mqt_id, secret, hmac_msg, self.keepAlive, self.clean_session, ssl)
        '''try:
            self.__mqtt_connect(mqt_id, secret, hmac_msg, self.keepAlive, self.clean_session, ssl)
        except:
            print("lk mqtt connect error")'''

    def __listen(self):
        print('listen thread start')
        while self.connected == False:                 # wait for mqtt connect success
            utime.sleep(0.5)
        while True:
            if self.exit == True:
                print('listen thread exit')
                _thread.exit()
            try:
                self.mqtt_client.wait_msg_timeout()
            except:
                pass

    def __mqtt_connect(self, mqt_id, secret, hmac_msg, keepAlive, clean_session, ssl):
        #print('start mqtt connect...')
        mqt_server = "{}.iot-as-mqtt.{}.aliyuncs.com".format(self.productKey, self.region)
        #self.password = _hmac.new(bytes(secret, "utf8"), msg=bytes(hmac_msg, "utf8"), digestmod=sha256).hexdigest()
        self.password = hmac.sha256(secret, hmac_msg)
        '''print("secret:", secret)
        print("hmac_msg:", hmac_msg)
        old_pwd = "c8418b54579b0c25cb6aff55359076ab6ca789eb8b855c35f16fe37ca30abc01"
        print("old pwd is:", old_pwd)
        print("new pwd is:",self.password)'''
        mqtt_client = MQTTClient(mqt_id, mqt_server, self.port, self.username, self.password,
                                      keepAlive, ssl=ssl)
        mqtt_client.set_callback(self.__default_recv_handler)
        mqtt_client.connect(clean_session=clean_session)
        self.mqtt_client = mqtt_client       # this must be set before setting self.connected
        self.connected = True
        #print('mqtt connect success...')
        if (self.oncallback[self.ON_CONNECT] != None):
            self.oncallback[self.ON_CONNECT](self.connected)
        return self.mqtt_client

    def __rev_NTPtime_handler(self, data):
        dst = int(data['deviceSendTime'])
        srt = int(data['serverRecvTime'])
        sst = int(data['serverSendTime'])
        utc = int((srt + sst + int(time.time()*1000) - dst) / 2)
        NTPtime = dict()
        NTPtime['msecond'] = utc % 1000
        local_time = time.localtime(int(utc/1000))
        NTPtime['year'] = str(local_time[0])
        NTPtime['month'] = str(local_time[1])
        NTPtime['day'] = str(local_time[2])
        NTPtime['hour'] = str(local_time[3])
        NTPtime['minute'] = str(local_time[4])
        NTPtime['second'] = str(local_time[5])
        NTPtime['timestamp'] = utc
        self.NTPtime = NTPtime
        if (self.ntpcallback != None):
            self.ntpcallback(NTPtime)
        self.NTPtime_Received = True

    def __create_subscribe_node(self, topic, handler):
        node = dict()
        node['topic'] = topic
        node['handler'] = handler
        return node

    def lk_getDeviceInfo(self):
        devinfo = dict()
        devinfo['region'] = self.region
        devinfo['deviceName'] = self.deviceName
        devinfo['deviceSecret'] = self.deviceSecret
        devinfo['productKey'] = self.productKey
        devinfo['productSecret'] = self.productSecret
        return devinfo

    def lk_subscribe(self, topic, qos=0):

        subscribe_node = self.__create_subscribe_node(topic, self.oncallback[self.ON_SUBCRIBE])
        self.__subsribe_list_insert(subscribe_node)

        try:
            self.mqtt_client.subscribe(topic, qos)
            return 0
        except OSError as e:
            print("[WARNING] subscribe failed. Try to reconnect : %s" % str(e))
            return -1

    def lk_unsubscribe(self, topic, qos=0):
        try:
            self.mqtt_client.unsubscribe(topic, qos)
            return 0
        except OSError as e:
            print("[WARNING] unsubscribe failed. Try to reconnect : %s" % str(e))
            return -1

    def lk_publish(self, topic, msg, retain=False, qos=0):
        try:
            self.mqtt_client.publish(topic, msg, retain, qos)
            return 0
        except OSError as e:
            print("[WARNING] Publish failed. Try to reconnect : %s" % str(e))
            return -1

    def lk_postProps(self, post_data):
        post_payload = post_data['params']
        msg_dic = dict()
        msg_dic['id'] = str(self.global_alink_id_next())
        msg_dic['version'] = "1.0"
        msg_dic['params'] = json.loads(post_payload)
        msg_dic['sys'] = {"ack": "1"}
        msg = json.dumps(msg_dic)
        res = self.lk_publish(self.TOPIC["prop_post"].format(self.productKey, self.deviceName), msg.encode('utf-8'))
        return res

    def lk_postEvent(self, event_data):
        event_id = str(event_data['id'])
        event_payload = json.loads(event_data['params'])

        msg_dic =dict()
        msg_dic['id'] = str(self.global_alink_id_next())
        msg_dic['version'] = "1.0"
        msg_dic['params'] = event_payload
        msg_dic['sys'] = {"ack": "1"}
        msg = json.dumps(msg_dic)

        postEvent_node = self.__create_subscribe_node(self.TOPIC["event_post_reply"].format(self.productKey, self.deviceName, event_id),
                                                    self.oncallback[self.ON_EVENT])
        self.__subsribe_list_insert(postEvent_node)

        res = self.lk_publish(self.TOPIC["event_post"].format(self.productKey, self.deviceName, event_id), msg.encode('utf-8'))

        return res

    def lk_postRaw(self, rawdata):
        msg = rawdata['param']

        postRaw_node = self.__create_subscribe_node(self.TOPIC['model_down'].format(self.productKey, self.deviceName),
                                                    self.oncallback[self.ON_RAWDATA])
        self.__subsribe_list_insert(postRaw_node)

        return self.lk_publish(self.TOPIC["model_up"].format(self.productKey, self.deviceName), msg)

    def lk_disconnect(self):
        if (self.connected == True):
            self.mqtt_client.disconnect()
            self.connected = False
        if (self.oncallback[self.ON_DISCONNECT] != None):
            self.oncallback[self.ON_DISCONNECT](0)
        utime.sleep(1)

    def lk_set_oncallback(self, oncb):
        self.oncallback= oncb

    def lk_getNTPtime(self, cb):
        self.ntpcallback = cb
        ntptime_node = self.__create_subscribe_node(self.TOPIC['ntp_res'].format(self.productKey, self.deviceName),
                                                self.__rev_NTPtime_handler
                                                )
        self.__subsribe_list_insert(ntptime_node)

        time_str = str(int(time.time() * 1000))
        data = dict()
        data['deviceSendTime'] = time_str
        msg = json.dumps(data)
        res= self.lk_publish(self.TOPIC["ntp_req"].format(self.productKey, self.deviceName), msg)
        return res

    def get_rand(self):
        return int(random.random()*100)

    def lk_upload(self, filename, data, data_len, cb):
        if filename == None:
            raise ValueError("filename wrong!")
        from ulinksdk.fileupload import UPload
        if self.up_handle == None:
            self.up_handle = UPload(lk_handle = self)
            self.up_handle.upload_subsribe()

        file_len = self.up_handle.file_data_desc_insert(filename, data, data_len)

        #print("file size is", file_len)    #test point
        file_option = dict()
        file_option['file_name'] = filename
        file_option['file_size'] = file_len
        file_option['mode'] = "overwrite"
        file_option['digest'] = None
        file_option['read_data_handler'] = self.up_handle.upload_read_data_handler
        file_option['userdata'] = None

        found = 0
        # print('found == 0')    #test point
        self.up_handle.uptask_lock.acquire()
        # print('after found == 0')  #test point
        for up_task in self.up_handle.up_task_list:
            if up_task['file_name'] == file_option['file_name']:
                found = 1
                self.up_handle.uptask_lock.release()

        # print('found == 0??', found) #test point
        if found == 0:
            crc64_str = '\0' * 32        # without CRC
            uuid = (str(int(time.time() * 1000))[0:13] + str(self.get_rand()))[0:15]
            # print('uuid:',uuid)  #test point
            up_task_new = self.up_handle.upload_file_task_init(file_option['file_name'], file_option['file_size'], \
                                file_option['mode'] ,crc64_str, uuid, file_option['read_data_handler'], file_option['userdata'])

            self.up_handle.upload_file_task_insert(up_task_new)
            self.up_handle.uptask_lock.release()

        self.up_handle.upload_open_stream(filename, None)

        result = dict()
        while True:
            result = self.up_handle.upload_process(filename)
            if result['file_name'] == filename:
                if (result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FINISHED) :
                    #print( "MQTT Upload file({}) ID({}) success".format(result['file_name'], result['uploadid']))
                    break
                elif result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FAILED or \
                    result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FAILED_TIMEOUT or \
                    result['status'] == self.up_handle.STATE_MQTT_UPLOAD_CANCEL_FAILED :
                    print("MQTT Upload file({}) failed,res:-{}".format(result['file_name'], hex(0-result['status'])) )
                    break
                elif result['status'] == self.up_handle.STATE_MQTT_UPLOAD_CANCEL_SUCCESS:
                    #print("MQTT Upload file({}) cancel success,res:-{}".format(result['file_name'], hex(0-result['status'])))
                    break
                elif result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK:
                    print("MQTT Upload file({}) whole file md5 failed,res:-{}".format(result['file_name'], hex(0-result['status'])))
                    break

        if (result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FINISHED):
            return result['uploadid']
        else:
            return None
