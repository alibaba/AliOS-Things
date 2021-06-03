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

def write(value):
   """
   设置GPIO电平值，GPIO引脚为输出模式时可用

   :param value: 待写入的数据
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def read():
   """
   读取GPIO电平值，输入模式和输出模式时均可用

   :param 空: 
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

