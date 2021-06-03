# * coding: UTF8 *
"""
这里所有的的接口仅需要调用一次即可，具体接口和参数如下所示。

=================================================================================================

"""

def install_codec_driver():
   """
   声卡安装，仅需要调用一次。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def uninstall_codec_driver():
   """
   声卡卸载，仅需要调用一次。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def init():
   """
   初始化uVoice功能组件，仅需要调用一次。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def deinit():
   """
   取消初始化uVoice功能组件，仅需要调用一次。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

