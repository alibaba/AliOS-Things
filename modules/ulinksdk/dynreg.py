#from _uhttp.client import HTTPConnection
import urequests as requests
import _thread
import urandom as random
import json
import hmac
from hashlib._sha256 import sha256
import http

class DYNREG:
    STATE_USER_INPUT_NULL_POINTER = (-0x0101)
    STATE_USER_INPUT_MISSING_HOST = (-0x0108)
    STATE_USER_INPUT_MISSING_PRODUCT_KEY = (-0x0104)
    STATE_USER_INPUT_MISSING_PRODUCT_SECRET = (-0x0107)
    STATE_USER_INPUT_MISSING_DEVICE_NAME = (-0x0105)
    STATE_USER_INPUT_EXEC_DISABLED = (-0x0109)


    def __init__(self, lk_handle = None, deviceinfo = None, cb = None):
        # param check
        print("params check")
        if 'region' not in deviceinfo:
            raise ValueError("region wrong!")
        if 'productKey' not in deviceinfo:
            raise ValueError("product key wrong!")
        if 'deviceName' not in deviceinfo:
            raise ValueError("device name wrong!")
        if 'productSecret' not in deviceinfo:
            raise ValueError("device secret wrong!")
        print("params done")
        self.lk_handle = lk_handle

        self.region = deviceinfo['region']
        self.host = "iot-auth.{}.aliyuncs.com".format(self.region)
        self.port = 443

        self.product_key = deviceinfo['productKey']
        self.product_secret = deviceinfo['productSecret']
        self.device_name = deviceinfo['deviceName']

        self.callback = cb

        self.recv_handler = self.dynreg_recv_handler
        self.http_conn = None

    def dynreg_recv_handler(self):
        pass

    def dynreg_send_request(self):
        
        ### test 

        '''data_patch = {
            'url': 'http://httpbin.org/post',
            'method': 'POST',
            'headers': {
                'Content-Type':'application/x-www-form-urlencoded',
                'Accept': 'text/xml,text/javascript,text/html,application/json',
                'Content-Length': '145',
            },
            'timeout': 6000,
            'params': ''
        }

        http.request(data_patch, self.callback)'''

        ### test
        
        print('start to send http request...')
        '''res = 0
        #self.http_conn = HTTPConnection(self.host, self.port)

        req_method = "POST"
        req_path = "/auth/register/device"

        random_str = str(int(random.random()*10000))
        print("random_str:", random_str)

        hmac_msg = "deviceName{}productKey{}random{}".format(self.device_name, self.product_key, random_str)


        sign_str = hmac.new(bytes(self.product_secret, "utf8"), msg=bytes(hmac_msg, "utf8"), digestmod=sha256).hexdigest()
        print("sign_str:", sign_str)
        
        req_content = "productKey={}&deviceName={}&random={}&sign={}&signMethod=hmacsha256".format(self.product_key, self.device_name, random_str, sign_str)
        
        
        req_header_dic= dict()
        #req_header_dic['Host'] = self.host
        req_header_dic['Accept'] = "text/xml,text/javascript,text/html,application/json"
        req_header_dic['Content-Type'] = "application/x-www-form-urlencoded"
        req_header_dic['Content-Length'] = str(len(req_content))

        request_url = self.host + req_path
        
        print("\n\nreq_method:",req_method)
        print("request_url:",request_url)
        print("req_content:", req_content)
        print("req_header:", req_header_dic)
        print('\n\n')

        print('send http request start...')

        res = requests.post(request_url, headers=req_header_dic, data = req_content)
        print("post done")
        print(res)'''


        # requests.request(method=req_method, url=request_url, data= req_content.encode('utf-8'), headers=req_header_dic)

        ######### test use
        request_url = "http://httpbin.org/post"
        req_content = json.dumps({ "cid": 1, "arg": 2})
        req_header_dic = { 'Content-Type':'application/x-www-form-urlencoded', 'Accept': 'text/xml,text/javascript,text/html,application/json'}
        ######### test use
        res = requests.post(request_url, headers=req_header_dic, data = req_content)
        print('send http request finished...')
        print(res)

        
        '''combine_header = "{} {} HTTP/1.1\r\nHost: {}\r\n{}Content-Length: {}\r\n\r\n".format(req_method, req_path, self.host, \
                                                                                            json.dumps(req_header_dic), str(len(req_content)))'''


    def dynreg_recv(self):
        pass
        '''print('start to receive message ...')
        if self.http_conn == None:
            return -1
        res = 0

        resp = self.http_conn.getresponse()
        print(resp)
        print(resp.read())'''



        
        