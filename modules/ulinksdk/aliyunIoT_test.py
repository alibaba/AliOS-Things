import linksdk

class Device:
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

    def __init__(self):
        self.linksdk = linksdk.LinkSDK()
        self.callback = [None] * (self.ON_CB_MAX)
        self.deviceHandle = [self.callback, self.linksdk]
        
    def register(self, deviceinfo, callback):
        if (self.linksdk == None):
            print("device not connected!")
            return (-1)
        try:
            return self.linksdk.lk_register(deviceinfo, callback)
        except:
            pass

    def on(self, Type, cb):
        self.callback[Type] = cb

    def connect(self, key_info):
        self.linksdk.lk_set_oncallback(self.callback)
        self.linksdk.lk_connect(key_info)

    def getDeviceInfo(self):
        if (self.linksdk == None):
            print("device not connected!")
            return (-1)
        try:
            return self.linksdk.lk_getDeviceInfo()
        except:
            pass
    def getDeviceHandle(self):
        if (self.deviceHandle):
            return self.deviceHandle
        else:
            return None

    def getNtpTime(self, cb):
        if (self.linksdk == None):
            print("device not connected!")
            return (-1) 
        try:
            return self.linksdk.lk_getNTPtime(cb)
        except:
            pass

    def postProps(self, post_data): 
        if (self.linksdk == None):
            print("device not connected!")
            return (-1) 
        try:
            self.linksdk.lk_postProps(post_data)
            return 0
        except:
            pass

    def postEvent(self, event_data):
        if (self.linksdk == None):
            print("device not connected!")
            return (-1) 
        try:
            self.linksdk.lk_postEvent(event_data)
            return 0
        except:
            pass
        
    def postRaw(self, raw_data):
        if (self.linksdk == None):
            print("device not connected!")
            return (-1) 
        try:
            self.linksdk.lk_postRaw(raw_data)
            return 0
        except:
            pass

    def uploadFile(self, filename, filepath, cb):
        if (self.linksdk == None):
            print("device not connected!")
            return None
        try:
            return self.linksdk.lk_upload(filename, filepath, 0, cb)
        except:
            return None

    def uploadContent(self, filename, content, cb):
        if (self.linksdk == None):
            print("device not connected!")
            return None
        try:
            if type(content) == type('str'):
                content = content.encode('utf-8')
            data_len = len(content)
            return self.linksdk.lk_upload(filename, content, data_len, cb)
        except:
            return None

    def subscribe(self, topicinfo):
        if (self.linksdk == None):
            print("device not connected!")
            return (-1)
        return self.linksdk.lk_subscribe(
            topic = topicinfo['topic'],
            qos = topicinfo['qos']
        )

    def publish(self, topicinfo):
        if (self.linksdk == None):
            print("device not connected!")
            return (-1)

        return self.linksdk.lk_publish(
            topic = topicinfo['topic'],
            msg = topicinfo['payload'],
            qos = topicinfo['qos']
        )

    def unsubscribe(self,topicinfo):
        if (self.linksdk == None):
            print("device not connected!")
            return (-1)
            
        return self.linksdk.lk_unsubscribe(
            topic = topicinfo['topic'],
            qos = topicinfo['qos']
        )


    def end(self):
        self.linksdk.mqtt_client.disconnect()
