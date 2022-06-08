import ujson as json
import utime
import _thread
import hmac
import uos
from _umqtt.simple2 import MQTTClient
from hashlib._sha256 import sha256
import urandom as random
import time
from dynreg import DYNREG

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
        "get_ota_firmware": "/sys/{}/{}/thing/ota/firmware/get", #.format(productKey, DeviceName),

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

    def global_alink_id_next(self):
        self.g_global_lock.acquire()
        self.alink_id = self.alink_id+1
        if (self.alink_id < 0):
            self.alink_id = 0
        self.g_global_lock.release()
        return self.alink_id

    def lk_register(self, deviceinfo, callback):
        print("in lk_register..")
        self.dynreg_handle = DYNREG(lk_handle = self, deviceinfo = deviceinfo, cb = callback)
        print("in lk_register001..")

        self.dynreg_handle.dynreg_send_request()

        self.dynreg_handle.dynreg_recv()

        pass

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
        #self.__connect_main()

        _thread.start_new_thread(self.__connect_main, ())
        _thread.start_new_thread(self.__listen, ())
        #self.__start()

    def __subsribe_list_insert(self, listnode):
        self.sub_list_lock.acquire()
        self.subscribe_list.append(listnode)
        self.sub_list_lock.release()

    def lk_rundom(self):
        msg = ""
        for i in range(0,5):
            num = random.randint(1, 10)
            msg += str(num)
        return msg

    def lk_check_secret(self, deviceName):
        try:
            with open("/data/pyamp/secret.json", "r", encoding="utf-8") as f:
                secret_data = json.load(f)
        except Exception as e:
            print("[ERROR] File Open failed : %s" % str(e))
        device_secret = secret_data.get(deviceName, None)
        if device_secret != None:
            return device_secret
        return False

    def lk_save_Secret(self, data):
        secret_data = {}
        if "secret.json" in uos.listdir("data/pyamp"):
            with open("/data/pyamp/secret.json", "r", encoding="utf-8") as f:
                secret_data = json.load(f)
                print(secret_data)
        try:
            with open("/data/pyamp/secret.json", "w+", encoding="utf-8") as w:
                secret_data.update(data)
                json.dump(secret_data, w)
        except Exception as e:
            print("[ERROR] File write failed : %s" % str(e))

    def __formatConnectInfo(self, secret, randomNum=None):
        secret = secret
        if randomNum != None:
            mqt_id = "{}|securemode=2,authType=register,random={},signmethod=hmacsha256|".format(self.clientID, randomNum)
            hmac_msg = "deviceName{}productKey{}random{}".format(self.deviceName, self.productKey, randomNum)
        else:
            mqt_id = "{}|securemode=3,signmethod=hmacsha256|".format(self.clientID)
            hmac_msg = "clientId{}deviceName{}productKey{}".format(self.clientID, self.deviceName, self.productKey)
        return mqt_id, secret, hmac_msg

    def __default_recv_handler(self, topic, msg, retained, status):
        print("Subscribe Recv: Topic={},Msg={}".format(topic.decode(), msg.decode()))
        if str(topic, "utf-8") == "/ext/register":
            data = json.loads(msg)
            self.deviceSecret = data.get("deviceSecret")
            self.productSecret = None
            data = {self.deviceName: self.deviceSecret}
            self.lk_save_Secret(data)                    # save DeviceSecret
            #self.__connect_main()
        else:
            self.sub_list_lock.acquire()
            for node in self.subscribe_list:
                if node['topic'] == topic.decode():
                    if node['handler'] != None:
                        msg_data = json.loads(msg.decode())
                        node['handler'](msg_data)
            self.sub_list_lock.release()
                
    def __connect_main(self):
        if self.productSecret == None:
            ssl = False
            mqt_id, secret, hmac_msg = self.__formatConnectInfo(self.deviceSecret)
            print(secret)
            print(hmac_msg)
        else:
            if "secret.json" in uos.listdir("usr/"):
                msg = self.lk_check_secret(self.deviceName)
                if msg:
                    self.deviceSecret = msg
                    mqt_id, secret, hmac_msg = self.__formatConnectInfo(self.deviceSecret)
                    self.__mqtt_connect(mqt_id, secret, hmac_msg, self.keepAlive, self.clean_session, ssl=False)
                    print("[INFO] The MQTT connection was successful")
                    return 0
            print("[INFO] MQTT dynamic registration")
            ssl = True
            randomNum = self.lk_rundom()
            mqt_id, secret, hmac_msg = self.__formatConnectInfo(self.productSecret, randomNum=randomNum)
            try:
                mqtts_cl = self.__mqtt_connect(mqt_id, secret, hmac_msg, self.keepAlive, self.clean_session, ssl)
            except:
                return -1         
            utime.sleep(2)
            mqtts_cl.wait_msg()
            utime.sleep(1)
            mqtts_cl.disconnect()
            return 0
        try:
            self.__mqtt_connect(mqt_id, secret, hmac_msg, self.keepAlive, self.clean_session, ssl)
            return 0
        except:
            return -1

    def __listen(self):
        print('start listen...')
        while self.connected == False:                 # wait for mqtt connect success
            pass
        while self.connected == True:
            try:
                self.mqtt_client.wait_msg()
            except OSError as e:
                return -1

    def __mqtt_connect(self, mqt_id, secret, hmac_msg, keepAlive, clean_session, ssl):
        print('start mqtt connect...')
        mqt_server = "{}.iot-as-mqtt.{}.aliyuncs.com".format(self.productKey, self.region)
        self.password = hmac.new(bytes(secret, "utf8"), msg=bytes(hmac_msg, "utf8"), digestmod=sha256).hexdigest()
        #self.password = "52b84e97a779bf69b2cf57178d8c58980edbb79f8c568af8e1c9d1df38f17520"
        mqtt_client = MQTTClient(mqt_id, mqt_server, self.port, self.username, self.password,
                                      keepAlive, ssl=ssl)
        mqtt_client.set_callback(self.__default_recv_handler)
        mqtt_client.connect(clean_session=clean_session)
        self.mqtt_client = mqtt_client       # this must be set before setting self.connected
        self.connected = True
        print('mqtt connect success...')
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
        postProps_node = self.__create_subscribe_node(self.TOPIC["prop_set"].format(self.productKey, self.deviceName), 
                                                    self.oncallback[self.ON_PROPS])
        self.__subsribe_list_insert(postProps_node)
        post_payload = post_data['params']

        msg_dic = dict()
        msg_dic['id'] = str(self.global_alink_id_next())
        msg_dic['version'] = "1.0"
        msg_dic['params'] = post_payload
        msg_dic['sys'] = {"ack": "1"}
        msg = json.dumps(msg_dic)
        res = self.lk_publish(self.TOPIC["prop_post"].format(self.productKey, self.deviceName), msg)
        return res

    def lk_postEvent(self, event_data):
        event_id = str(event_data['id'])
        event_payload = event_data['params']

        msg_dic =dict()
        msg_dic['id'] = str(self.global_alink_id_next())
        msg_dic['version'] = "1.0"
        msg_dic['params'] = event_payload
        msg_dic['sys'] = {"ack": "1"}
        msg = json.dumps(msg_dic)

        postEvent_node = self.__create_subscribe_node(self.TOPIC["event_post_reply"].format(self.productKey, self.deviceName, event_id), 
                                                    self.oncallback[self.ON_EVENT])
        self.__subsribe_list_insert(postEvent_node)

        res = self.lk_publish(self.TOPIC["event_post"].format(self.productKey, self.deviceName, event_id), msg)

        return res
    
    def lk_postRaw(self, rawdata):
        msg = rawdata['param']
        
        postRaw_node = self.__create_subscribe_node(self.TOPIC['model_up_reply'].format(self.productKey, self.deviceName), 
                                                    self.oncallback[self.ON_RAWDATA])
        self.__subsribe_list_insert(postRaw_node)

        return self.lk_publish(self.TOPIC["model_up"].format(self.productKey, self.deviceName), msg)

    def lk_dis_connect(self):
        self.mqtt_client.disconnect()
        self.connected = False        
        if (self.oncallback[self.ON_DISCONNECT] != None):
            self.oncallback[self.ON_DISCONNECT]()
        pass

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
        
        from upload_file import UPload
        self.up_handle = UPload(lk_handle= self)

        self.up_handle.upload_subsribe()

        if data_len == 0:
            self.up_handle.g_data = None
            self.up_handle.g_data_len = 0
            self.up_handle.g_file_path = data
            file_len = self.up_handle.get_file_size(data)
        else:
            self.up_handle.g_data = data
            self.up_handle.g_data_len = data_len
            file_len = data_len

        # print("file size is", file_len)    #test point

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
            result = self.up_handle.upload_process()
            if (result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FINISHED):
                print( "MQTT Upload file({}) ID({}) success".format(result['file_name'], result['uploadid']))
                break

            elif result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FAILED or \
                 result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FAILED_TIMEOUT or \
                 result['status'] == self.up_handle.STATE_MQTT_UPLOAD_CANCEL_FAILED :
                 
                print("MQTT Upload file({}) failed,res:-0x{}".format(result['file_name'], str(0-result['status'])) )
                break

            elif result['status'] == self.up_handle.STATE_MQTT_UPLOAD_CANCEL_SUCCESS:
                print("MQTT Upload file({}) cancel success,res:-0x{}".format(result['file_name'], str(0-result['status'])))
                break

            elif result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FAILED_WHOLE_CHECK:
                print("MQTT Upload file({}) whole file md5 failed,res:-0x{}".format(result['file_name'], str(0-result['status'])))
                break
        
        if result['status'] == self.up_handle.STATE_MQTT_UPLOAD_FINISHED:
            return result['uploadid']
        else:
            return None
        