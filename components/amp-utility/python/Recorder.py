# * coding: UTF8 *
"""
Recorder模块具体接口和参数。

==========================================================================================

"""

def set_sink(format, rate, channels, bits, frames, bitrate, sink):
   """
   请求录音资源并设置录音参数。

   :param format(int): 录音数据编码格式, 可以选择的类型为:

      .. c:enum:: media_format_t

         MEDIA_FMT_UNKNOWN = 0,

         MEDIA_FMT_PCM,

         MEDIA_FMT_WAV,

         MEDIA_FMT_MP3,

         MEDIA_FMT_AAC,

         MEDIA_FMT_M4A,

         MEDIA_FMT_OGG,

         MEDIA_FMT_OPS,

         MEDIA_FMT_SPX,

         MEDIA_FMT_WMA,

         MEDIA_FMT_AMR,

         MEDIA_FMT_AMRWB,

         MEDIA_FMT_FLAC,
   :param rate(int): 采样率，支持采样率包括但不限于：8K, 16K，24K
   :param channels(int): 声道数，取值为1或者2
   :param bits(int): 数据宽度，取值为16，24，32, 通常取值为16
   :param frame_samples(int): 帧长度, 每次录音期待读取的数据
   :param sink(str): 保存路径，对于录音到缓存中的场景，sink设置NULL即可。

   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def clr_sink():
   """
   释放录音资源。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def start():
   """
   开始录音。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def stop():
   """
   停止录音。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def get_stream(buffer, nbytes):
   """
   读取指定长度的录音数据并保存到buffer中。

   :param buffer(bytearray): 音频存放的位置
   :param nbytes(int): 期待读取的音频字节数
   :returns: 实际读取到的音频数据字节数，不大于nbytes；负数代表读取失败。
   :raises OSError: EINVAL
   """
   pass

def get_state():
   """
   获取录音引擎状态。

   :param 空:
   :returns: 录音音频状态，可以选择的类型为:

      .. c:enum::  recorder_state_t

            RECORDER_STAT_IDLE = 0,

            RECORDER_STAT_READY,

            RECORDER_STAT_RUNNING,

            RECORDER_STAT_STOP,

            RECORDER_STAT_ERROR,
   :raises OSError: EINVAL
   """
   pass

def get_position():
   """
   获取录音进度，单位为秒。

   :param 空:
   :returns: int类型，当前录音进度
   :raises OSError: EINVAL
   """
   pass

def ns_enable(enable):
   """
   是否使能音频降噪NS(Noise Suppression)功能。

   :param enable(int): 1为允许使能，0为禁止使能
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def ec_enable(enable):
   """
   是否使能回声消除EC(Echo Cancle)功能。

   :param enable(int): 1为允许使能，0为禁止使能
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def agc_enable(enable):
   """
   是否使能自动增益控制AGC(Auto Gain Control)功能。

   :param enable(int): 1为允许使能，0为禁止使能
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def vad_enable(enable):
   """
   是否使能语音活性检测VAD(Voice Activity Detection)功能。

   :param enable(int): 1为允许使能，0为禁止使能
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def format_support(format):
   """
   设置播放器支持的音源类型。

   :param format(int): 音源类型，具体内容参考 ``set_sink``
   :returns: 0: 成功，其他: 失败。
   :raises OSError: EINVAL
   """
   pass
