# -*- coding: UTF-8 -*-

import netmgr as nm

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

        :param data(dict): data的key信息如下

            .. list-table::

                * - 属性
                  - 类型
                  - 必填
                  - 说明
                * - ssid
                  - 字符串
                  - 必填
                  - 需要连接的wifi热点名称
                * - password
                  - 字符串
                  - 必填
                  - 需要连接的wifi密码


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

        :param 空:
        :returns: 

            .. list-table::

                * - 返回值
                  - 网络类型
                * - 0
                  - WIFI
                * - 1
                  - 蜂窝网络
                * - 2
                  - 以太网
                * - 3
                  - 未知网络   




        """
        return nm.getType()

    def getStatus(self):
        """

        获取当前网络状态

        :param 空:
        :returns: 

          - ``True``  已连接
          - ``False`` 未连接

        """
        return nm.getStatus()

    def getInfo(self):
        """

        获取当前网络信息

        :param 空:
        :returns: 返回一个字典，字典信息如下

            .. list-table::

                * - key名称
                  - value类型
                * - SSID
                  - 字符串
                * - IP
                  - 字符串
                * - MAC
                  - 字符串
                * - RSSI
                  - Int 

        """
        return nm.getInfo()


