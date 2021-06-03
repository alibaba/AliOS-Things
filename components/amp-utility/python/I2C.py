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
   发送数据，该函数为阻塞函数，I2C发送完成后才会返

   :param dataBuffer: 待写入的数据
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def read(dataBuffer):
   """
   主动读取指定bytes的I2C数据。该函数为阻塞函数，会一直等待读取指定bytes的数据

   :param dataBuffer: 读出来数据的存储空间
   :returns: >0: 成功读取数据的字节数，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def writeReg(addr, dataBuffer):
   """
   向寄存器地址memaddr发送数据，该函数为阻塞函数，I2C发送完成后才会返回

   :param addr: 设备地址
   :param dataBuffer: 待写入的数据
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def readReg(addr, dataBuffer):
   """

    主动向寄存器地址memaddr读取指定bytes的I2C数据。该函数为阻塞函数，会一直等待读取指定bytes的数据
   
   :param addr: 设备地址
   :param dataBuffer: 读出来数据的存储空间
   :returns: >0: 成功读取数据的字节数，其他: 失败
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

