# -*- coding: UTF-8 -*-


class NetWorkClient(object):

    """
    该模块实现网络管理相关的功能，包括初始化，联网，状态信息等.
    """
    def __init__(self):
        nm.init()

    def __str_is_empty(self,value):
        if value is None or value == "":
            return True
        else:
            return False

    def connect(self,data):
        """
        连接网络

        **入参**
        
        data类型是字典，字典的key信息如下

            - ssid : 必选，值是字符串
            - password : 必选，值是字符串


        使用示例::

            # -*- coding: UTF-8 -*-
            import network
            net =  network.NetWorkClient()
            net.connect({
                    'ssid' : 'KIDS' ,
                    'password' : '12345678'
                }
            )
        """
        if isinstance(data, dict):
            pass
        else:
            raise ValueError("connect func param must be dict")

        if not 'ssid' in data:
            raise ValueError('connect : param must have key "ssid"')
        elif self.__str_is_empty(data['ssid']):
            raise ValueError("ssid wrong")

        if not 'password' in data:
            raise ValueError('connect : param must have key "password"')
        elif self.__str_is_empty(data['password']):
            raise ValueError("password wrong")
        nm.connect(data['ssid'],data['password'])

    def disconnect(self):
        """
        断开网络
        """
        nm.disconnect

    def getType(self):
        """
        获取当前网络类型:

          - ``0``  :     NETWORK_WIFI
          - ``1``  :     NETWORK_CELLULAR
          - ``2``  :     NETWORK_ETHERNET
          - ``3``  :     NETWORK_UNKNOW

        """
        return nm.getType()

    def getStatus(self):
        """

        获取当前网络状态

          - ``True``  已连接
          - ``False`` 未连接

        """
        return nm.getStatus
