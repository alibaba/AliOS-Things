import usocket
import _thread
import urandom as random
import json
import hmac
import uos
import utime

class DYNREG:
    STATE_USER_INPUT_NULL_POINTER = (-0x0101)
    STATE_USER_INPUT_MISSING_HOST = (-0x0108)
    STATE_USER_INPUT_MISSING_PRODUCT_KEY = (-0x0104)
    STATE_USER_INPUT_MISSING_PRODUCT_SECRET = (-0x0107)
    STATE_USER_INPUT_MISSING_DEVICE_NAME = (-0x0105)
    STATE_USER_INPUT_EXEC_DISABLED = (-0x0109)
    CORE_HTTP_DEFAULT_HEADER_LINE_MAX_LEN = 128

    def __init__(self):
        self.callback = None
        self.keepAlive = 300
        self.region = None
        self.product_key = None
        self.product_secret = None
        self.device_name = None
        self.device_Secret = None
        self.register_success = False
        self.clean_session = False
        self.clientID = '13225554099'        # 自定义字符（不超过64）
        self.connected = False
        self.http_client = None
        self.msg_data = None
        self.port = 443

    def __save_Secret(self, data):
        secret_data = {}
        if "secret.json" in uos.listdir("/"):
            with open("/secret.json", "r", encoding="utf-8") as f:
                secret_data = json.load(f)
        try:
            with open("/secret.json", "w+", encoding="utf-8") as w:
                secret_data.update(data)
                json.dump(secret_data, w)
        except Exception as e:
            print("[ERROR] File write failed : %s" % str(e))

    def dynreg_rundom(self):
        msg = ""
        for i in range(0,5):
            num = random.randint(1, 10)
            msg += str(num)
        return msg

    def register(self, deviceinfo, cb):
        # param check
        if 'productKey' not in deviceinfo:
            raise ValueError("product key wrong!")
        if 'deviceName' not in deviceinfo:
            raise ValueError("device name wrong!")
        if 'productSecret' not in deviceinfo:
            raise ValueError("device secret wrong!")
        self.region = "cn-shanghai"
        if 'region' in deviceinfo:
            self.region = deviceinfo['region']
        self.product_key = deviceinfo['productKey']
        self.product_secret = deviceinfo['productSecret']
        self.device_name = deviceinfo['deviceName']
        self.host = "iot-auth.{}.aliyuncs.com".format(self.region)
        self.port = 443
        self.callback = cb
        self.username = "{}&{}".format(self.device_name, self.product_key)
        # print("self.username",self.username)
        # print("[INFO] dynamic registration")

        self.dynreg_connect()
        self.dynreg_send_req()
        status_code, content =  self.dynreg_receive()
        # print("status_code, content", status_code, content)
        # print("msg['code']",msg['code'])
        if (status_code != 200):
            self.register_success = False
        else:
            self.msg_data = msg = json.loads(content)
            if (msg['code'] == 200):
                self.register_success = True
                data = {msg['data']['deviceName']: msg['data']['deviceSecret']}
                self.__save_Secret(data)
            elif (msg['code'] == 6289):
                self.register_success = True
            else:
                self.register_success = False

        if (self.register_success == True):
            #print('register success!')
            self.callback(self.msg_data)
            return 0
        else:
            return -1

    def dynreg_sign (self,randomNum):
        plain_text = "deviceName%sproductKey%srandom%s" % (self.device_name, self.product_key, randomNum)
        #sign_hex = _hmac.new(bytes(self.product_secret, "utf8"), msg=bytes(plain_text, "utf8"), digestmod=sha256).hexdigest()
        sign_hex = hmac.sha256(self.product_secret, plain_text)
        return sign_hex

    def dynreg_connect(self):
        self.sock = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
        sockaddr = usocket.getaddrinfo(self.host, self.port)[0][-1]
        #sockaddr = usocket.getaddrinfo("www.baidu.com", self.port)[0][-1]
        self.sock.connect(sockaddr)
        import ussl
        #self.sock = ussl.wrap_socket(self.sock, server_side = False, server_hostname = self.host, key = None, cert = cred['x509_server_cert'].encode('utf-8'))
        self.sock = ussl.wrap_socket(self.sock)
    def dynreg_send_req(self):
        randomNum = self.dynreg_rundom()
        sign_str = self.dynreg_sign(randomNum)
        content = "productKey=%s&deviceName=%s&random=%s&sign=%s&signMethod=hmacsha256" % \
                    (self.product_key, self.device_name, randomNum, sign_str)
        request = {
            "method": "POST",
            "path": "/auth/register/device",
            "header" : "Accept: text/xml,text/javascript,text/html,application/json\r\nContent-Type: application/x-www-form-urlencoded\r\n",
            "content" : content,
            "content_len" : len(content)
        }
        combine_header = "%s %s HTTP/1.1\r\nHost: %s\r\n%sContent-Length: %s\r\n\r\n" % \
                            (request["method"], request["path"], self.host, request["header"], request["content_len"])

        # send header
        #print("combine_header:\n", combine_header) # test point
        self.sock.write(combine_header)

        #print("content:\n", content) # test point
        # send body
        self.sock.write(content)


    def dynreg_receive(self):
        # receive header
        status_code = 0
        content_length = 0
        line = str()
        while True:
            char = self.sock.read(1).decode()
            if (char != '\r'):
                line = line + char
            else:
                #print(line)
                if (line[0:8] == 'HTTP/1.1'):
                    status_code = int(line.split(' ')[1])
                if (line[0:14] == 'Content-Length'):
                    content_length = int(line.split(' ')[1])

                self.sock.read(1).decode()   # '\n'

                line = str()
                char = self.sock.read(1).decode()
                if(char == '\r'):
                    self.sock.read(1).decode()   # '\n'
                    break
                line = line + char

        #print('status_code:', status_code)
        #print('content_length:', content_length)
        content = self.sock.read(content_length).decode()
        #print(content)
        return status_code, content
