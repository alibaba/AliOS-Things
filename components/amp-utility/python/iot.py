# -*- coding: UTF-8 -*-
"""



阿里云物联网平台提供安全可靠的设备连接通信能力，支持设备数据采集上云，规则引擎流转数据和云端数据下发设备端。此外，也提供方便快捷的设备管理能力，支持物模型定义，数据结构化存储，和远程调试、监控、运维。 基于iot模块可以简单快速的使用阿里云物联网平台能力。

iot模块的主要实现是一个Device 类，通过它的构造函数获取device实例，就可以轻松设置相关回调函数和调用相关方法。阿里云物联网平台中涉及三个基本概念如下:

- 属性(prop)
    云端/设备端数据互通的通道，云端通过改变prop值，反馈到设备端，设备端修改prop值，保存到云端

- 事件(event)
    设备端到云端的事件通知

- 服务(service)
    云端到设备端的服务调用

详细使用示例请参考主页”参考案例”中的云端连接/控制部分


"""

class Device(object):
    """

    初始化物联网平台Device类，获取device实例
      
    **入参**

    data类型是字典，字典的key信息如下

        - deviceName : 物联网平台上注册的设备名称，必选
        - deviceSecret : 物联网平台上注册的设备秘钥，必选
        - productKey : 物联网平台上注册的productKey，必选
        - productSecret : 物联网平台上注册的productSecret，可选
         - region : 默认值是cn-shanghai，可选



    使用示例::

         import iot

         productKey = "a1uTFk4xjko"
         productSecret = "xxxxxxx"
         deviceName  =    "mpy_001"
         deviceSecret  =   "xxxxxxxxxxxxxxx"
         key_info = {
            'region' : 'cn-shanghai' ,
            'productKey': productKey ,
            'deviceName': deviceName ,
            'deviceSecret': deviceSecret ,
            'productSecret': productSecret
         }
         device = iot.Device(key_info) 



    """

    def __init__(self,data):

        def __str_is_empty(value):
            if value is None or value == "":
                return True
            else:
                return False

        if not isinstance(data,dict):
            raise ValueError("Class Device init param must be dict")

        if not 'productKey' in data:
            raise ValueError('Device init : param must have key "productKey"')
        elif __str_is_empty(data['productKey']):
            raise ValueError("productKey wrong")

        if not 'deviceName' in data:
            raise ValueError('Device init : param must have key "deviceName"')
        elif __str_is_empty(data['deviceName']):
            raise ValueError("deviceName wrong")

        if not 'deviceSecret' in data:
            raise ValueError('Device init : param must have key "deviceSecret"')
        elif __str_is_empty(data['deviceSecret']):
            raise ValueError("deviceSecret wrong")

        if 'productSecret' in data:
            productSecret = data['productSecret']
        else:
            productSecret = ""

        if 'region' in data:
            region = data['region']
        else:
            region = "cn-shanghai"    
        
        _lk.init(region,data['productKey'],data['deviceName'],data['deviceSecret'],productSecret)

        _lk.register_dyn_dev()





    # def publish(self):
    #     pass

    # def subscribe(self):
    #     pass

    # def unsubscribe(self):
    #     pass



    def on(self,event,callback):

        """
        通过这个函数，可以设置物联网平台各种事件的处理函数，函数接收两个参数分别是事件名称和事件处理函数

        - 事件: 'connect'
   
        当iot设备连接到物联网平台的时候触发connect 事件
            
        事件处理函数示例::

            def on_connect():
                print('linkkit is connected')

        - 事件: 'disconnect'

        当连接断开时，触发'disconnect'事件

        事件处理函数示例::

            def on_disconnect():
                print('linkkit is disconnected')

        - 事件: 'props'

        当iot云端下发属性设置时，触发props事件

        事件处理函数示例::

            def on_props(request):
                print('clound req data is %s' %(request))
        """
        # - 事件: 'service'

        # 当iot云端调用设备service时，触发service事件

        # 事件处理函数示例::

        #     def on_service(id,request):
        #         print('clound req id  is %d , req is %s' %(id,request))

        # - 事件: 'error'

        # 当设备跟iot平台通信过程中遇到错误时，触发error事件

        # 事件处理函数示例::

        #     def on_error(err):
        #         print('err msg is %s '%(err))

        if not callable(callback):
            raise ValueError("the 2nd param of function on must be function")

        if (event == 'connect'):
            _lk.register_call_back(_lk.ON_CONNECT,callback)
        elif (event == 'disconnect'):
            _lk.register_call_back(_lk.ON_DISCONNECT,callback)
        elif (event == 'close'):
            _lk.register_call_back(_lk.ON_CLOSE,callback)
        elif (event == 'error'):
            _lk.register_call_back(_lk.ON_ERROR,callback)
        elif (event == 'props'):
            _lk.register_call_back(_lk.ON_PROPS,callback)
        elif (event == 'service'):
            _lk.register_call_back(_lk.ON_SERVICE,callback)


    def connect(self):
        '''
        连接物联网平台连接函数，该函数是异步调用
        '''
        _lk.connect()

    def postProps(self,data):
        """
        上报设备属性

        **入参**

        data类型是字典，字典的key信息如下

            - params : 必选，值是字典

        使用示例::


            data = {
                'params': {
                'test_prop' : 100
                }
            }
            device.postProps(data)


        """
        if not isinstance(data, dict):
            raise ValueError("postProps func param must be dict")

        if not 'params' in data:
            raise ValueError('data must have key: "params"')
        return _lk.post_property(ujson.dumps(data['params']))



    def postEvent(self,data):
        """
        上报设备的事件

        **入参**

        data类型是字典，字典的key信息如下

            - params : 必选，值是字典
            - id : 必选，事件名称，请参考物模型上定义的事件id

        使用示例::

            data = {
                'id': 'EventTest' ,
                'params': {
                'test_event' : 100
                }
            }
            device.postEvent(data)        
  

        """
        if isinstance(data, dict):
            if not 'id' in data:
                raise ValueError('data must have key:  "id"')
            if not 'params' in data:
                raise ValueError('data must have key: "params"')

        else:
            raise ValueError("postEvent func param must be dict")
        return _lk.post_event(data['id'],ujson.dumps(data['params']))


    def close(self):
        '''
        关闭物联网设备节点，断开连接
        '''
        return _lk.close()

    def do_yield(self,time):
        """
        激活物联网平台接收云端消息，并设置接收超时时间为:timeout, 单位是 ms.为了保证云端消息被接收到，执行了异步命令以后，需要循环执行这个方法，直到拿云端的返回
        """
        _lk.do_yield(time)


    # def __register_callback(self):
    #     _lk.register_call_back(1,self.__on_connect)
    #     _lk.register_call_back(3,self.__on_disconnect)
    #     _lk.register_call_back(5,self.__on_service_request)
    #     _lk.register_call_back(7,self.__on_prop_set)
    #     _lk.register_call_back(9,self.__on_thing_prop_post)
    #     _lk.register_call_back(10,self.__on_thing_event_post)


# class GateWay(object):

#     def addTopo(self):
#         pass

#     def getTopo(self):
#         pass

#     def removeTopo(self):
#         pass

#     def login(self):
#         pass

#     def logout(self):
#         pass

#     def regiestSubDevice(self):
#         pass

# def register():
    # pass









