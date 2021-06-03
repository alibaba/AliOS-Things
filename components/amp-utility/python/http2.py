# -*- coding: UTF-8 -*-
"""

模块功能：HTTP客户端的相关功能函数，包括连接，下载等功能

"""

AF_INET = 0 


class Client(object):
    """

    初始化client类，获取httpclient实例
      
    :param 空:
    :returns: client对象

    """
    def __init__(self):
        pass


    def set_header(self,header):
        """
        设置请求header参数

        :param header(str): 设置请求的header参数
        :returns: 0: 成功，其他: 失败

        """
        pass

    def get(self,url):
        """
        执行http get 请求

        :param url(str): data是一个str类型的 header
        :returns: 0: 成功，其他: 失败

        """
        pass

    def download(self,url,path):
        """
        设置请求header参数


        :param url(str): 请求的url
        :param path(str): 保存的文件路径
        :returns: 0: 成功，其他: 失败

        """
        pass



    