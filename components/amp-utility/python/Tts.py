# * coding: UTF8 *
"""
Tts模块具体接口和参数如下所示。

========================================================================================================

"""

def create():
   """
   创建TTS引擎。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: 空
   """
   pass

def release():
   """
   释放TTS引擎。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def callback(type, cbfun):
   """
   设置TTS的回调类型及回调函数。示例::

      def cb_url(test):
         print("receive info with url %s" %(test))
      
      t0.callback(t0.CB_ENUM_URL, cb_url)

   :param type(int): 回调类型，通过设置回调类型接收关心的TTS引擎信息，取值为CB_ENUM_URL, CB_ENUM_DATA或者CB_ENUM_EVENT
   :param cbfun(function): 回调函数
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def init(aicloud_type, config):
   """
   初始化Tts引擎配置，详细信息参考 `语音合成接口说明 <https://help.aliyun.com/document_detail/84435.html?spm=a2c4g.11186623.6.581.16a75275MJHPrH>`_ 。示例::

      t0.init(0, {'app_key': 'xxxxx', 
                  'token': 'xxxxxxxx', 
                  'format': 3, 
                  'sample_rate': 16000, 
                  'voice': 'siqi', 
                  'volume': 80, 
                  'speech_rate': 0, 
                  'pitch_rate':0, 
                  'text_encode_type': 1})

   :param aicloud_type(int): 目前可选值仅为0;
   :param config(dictionary): Tts引擎的配置信息;
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def request(text, recv_type):
   """
   发起Tts语音合成请求。

   :param text(str): 需要语音合成的文本信息
   :param recv_type(int): 语音合成后返回到端上的格式类型： 0表示返回URL，1表示返回数据
   :returns: 0: 成功，其他: 失败
   :raises OSError: ENXIO
   """
   pass

def stop():
   """
   停止Tts语音合成请求。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

