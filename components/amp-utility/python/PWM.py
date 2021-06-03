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

def setConfig(freq, duty):
   """
   设置PWM参数，同时会启动PWM输出

   :param freq: 频率
   :param duty: 占空比
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def getDuty():
   """
   获取PWM duty参数

   :param 空: 
   :returns: >0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def getFreq():
   """
   获取PWM频率参数

   :param 空: 
   :returns: >0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def setDuty(duty):
   """
   设置PWM duty参数

   :param duty :占空比值 
   :returns: >0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def setFreq(freq):
   """
   设置PWM频率参数

   :param freq :频率值 
   :returns: >0: 成功，其他: 失败
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

