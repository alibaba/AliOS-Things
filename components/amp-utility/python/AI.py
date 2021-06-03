# * coding: UTF8 *
"""

=================================================================================================

"""

def open(MLEngineType_t):
   """
   打开推理引擎功能模块

   :param MLEngineType_t: 引擎类型
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def config(key，secret，region_id，endpoint，url):
   """
   配置推理引擎需要的参数

   :param key: "Your-Access-Key"
   :param secret: "Your-Access-Secret"
   :param region_id: "Your-OSS-Endpoint"
   :param endpoint: "Your-OSS-Bucket"
   :param url: "compare URL"
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def setInputData(mFileName):
   """
   设置输入数据

   :param mFileName: 文件名
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def loadNet(mFileName):
   """
   设置输入数据

   :param mFileName: 文件名
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def predict():
   """
   开始推理

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def getPredictResponses():
   """
   获取推理结果

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def unLoadNet():
   """
   卸载AI模型文件

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

