# -*- coding: UTF-8 -*-
"""


模块功能：提供socket server和client相关的接口


"""

AF_INET = 0 

AF_INET6 = 1

SOCK_STREAM = 2

SOCK_DGRAM = 3

SOCK_RAW = 4

class socket(object):
    """

    初始化socket类，获取socket实例
      
    :param 空:
    :returns: client对象

    """
    def __init__(self,af=AF_INET):

        """
        param af(int): 地址族，可选类型如下

          usocket.AF_INET ：IPV4
          usocket.AF_INET6 ：IPV6

        type - socket类型

        usocket.SOCK_STREAM ：对应TCP的流式套接字

        usocket.SOCK_DGRAM ：对应UDP的数据包套接字

        usocket.SOCK_RAW ：原始套接字  


        """

        pass


    def bind(self,header):
        """
        设置请求header参数

        :param header(str): 设置请求的header参数
        :returns: 0: 成功，其他: 失败

        """
        pass

    def listen(self,url):
        """
        执行http get 请求

        :param url(str): data是一个str类型的 header
        :returns: 0: 成功，其他: 失败

        """
        pass

    def accept(self,url,path):
        """
        设置请求header参数


        :param url(str): 请求的url
        :param path(str): 保存的文件路径
        :returns: 0: 成功，其他: 失败

        """
        pass



    