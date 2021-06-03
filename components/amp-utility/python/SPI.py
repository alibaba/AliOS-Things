# * coding: UTF8 *
"""

=================================================================================================

"""

def open(type):
   """
   打开并根据board.json配置实例

   :param type: 对象类型
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def write(dataBuffer):
   """
   通过SPI发送数据，该函数为阻塞函数，SPI发送完成后才会返回

   :param dataBuffer: 待写入的数据
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def read(dataBuffer):
   """
   读取指定bytes的SPI数据，该函数为阻塞函数，读取到指定bytes的数据后才会返回

   :param dataBuffer: 读出来数据的存储空间
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def sendRecv(writeDataBuffer, readDataBuffer):
   """
   通过SPI发送数据阻塞发送数据，发送完后，再阻塞读取指定bytes的数据

   :param writeDataBuffer: 待写入的数据
   :param readDataBuffer: 读出来数据的存储空间
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def close():
   """
   关闭实例

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

