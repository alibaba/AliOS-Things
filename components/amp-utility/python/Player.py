# * coding: UTF8 *
"""
Player模块具体接口和参数如下所示。

==========================================================================================

"""

def create():
   """
   创建播放器。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: ENXIO
   """
   pass

def release():
   """
   释放由create函数创建的播放器。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def start():
   """
   开始音频播放，该接口为同步接口。程序会阻塞在该函数调用处，直到播放器正常启动后才返回。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def stop():
   """
   停止音频播放，该接口为同步接口。程序会阻塞在该函数调用处，直到播放器正常停止后才返回。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def pause():
   """
   暂停音频播放，该接口为同步接口。程序会阻塞在该函数调用处，直到播放器正常暂停后才返回。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def resume():
   """
   恢复音频播放，该接口为同步接口。程序会阻塞在该函数调用处，直到播放器正常恢复后才返回。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def complete():
   """
   等待当前播放完成，player进入standby状态。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def stop_async():
   """
   停止音频播放，该接口为异步接口。程序调用该函数后会立即返回，而不会阻塞在该函数调用处。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def pause_async():
   """
   暂停音频播放，该接口为异步接口。程序调用该函数后会立即返回，而不会阻塞在该函数调用处。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def resume_async():
   """
   恢复音频播放，该接口为异步接口。程序调用该函数后会立即返回，而不会阻塞在该函数调用处。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def pause_async():
   """
   暂停音频播放，该接口为异步接口。程序调用该函数后会立即返回，而不会阻塞在该函数调用处。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def resume_async():
   """
   恢复音频播放，该接口为异步接口。程序调用该函数后会立即返回，而不会阻塞在该函数调用处。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def set_source(source):
   """
   设置音频播放的音源，支持本地音源（data、sdcard）和URL音源。

   :param source(str): 音频播放的音源;
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def clr_source():
   """
   清除音频播放的音源。

   :param 空: 
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def set_stream(format, cache_enable, cache_size):
   """
   设置音频播放信息。

   :param format(int): 音频格式, 可以选择的类型为:

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
   :param cache_enable(int): 是否允许使能缓存。0：禁止， 其他：允许
   :param cache_size(int): 允许使能缓存情况下缓存的大小。
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def put_stream(buffer,  nbytes):
   """
   填充音频数据到音频播放器, 播放内存流。示例::

      p0.put_stream(bytearray([0,1,2,3,4,5,6,7,8,9]), 10)

   :param buffer(bytearray): 待播放的的音频数据
   :param nbytes(int): 待播放的音频数据字节数
   :returns: 实际填充的音频数据，不大于nbytes数据；负数代表填充失败。
   :raises OSError: EINVAL
   """
   pass

def clr_stream(flush):
   """
   停止内存流播放并释放资源。

   :param flush(int): 是否需要等待缓存中的数据全部播放完毕，1表示等待播完，0表示立即停止:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def set_pcminfo(rate, channels, bits, frames):
   """
   设置播放PCM数据时音频播放器的参数。

   :param rate(int): 音频采样率
   :param channels(int): 音频通道数
   :param bits(int): 音频位宽，取值范围为16，24或32，大部分音频为16.
   :param frames(int): 音频帧字节数，对应DMA中断周期。每播放完成frames字节数据产生一次DMA中断。该值太小会持续产生DMA终端，太大会引入较大的音频播放延迟。
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def get_duration():
   """
   获取正在播放的文件总时长，单位为秒。

   :param 空:
   :returns: 正在播放的文件总时长，单位为秒。
   :raises OSError: EINVAL
   """
   pass

def get_position():
   """
   获取播放进度，单位为秒。

   :param 空:
   :returns: 播放进度，单位为秒
   :raises OSError: EINVAL
   """
   pass

def set_volume(volume):
   """
   设置播放音量。

   :param volume(int): 音量等级，范围：0~10，0表示静音
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def get_volume():
   """
   获取当前播放音量。

   :param 空:
   :returns: 获取到的当前播放音量
   :raises OSError: EINVAL
   """
   pass

def volume_range():
   """
   播放器支持的音量范围。

   :param 空:
   :returns: dictionary类型， {'vol_min': 0, 'vol_max': 10}
   :raises OSError: EINVAL
   """
   pass

def seek(seconds):
   """
   定位到指定位置开始播放。

   :param seconds(int): 播放定位位置，单位：秒
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def playback(source):
   """
   设置播放器音源并启动播放。

   :param source(str): 音频播放的音源，支持本地音源（data、sdcard）和URL音源。
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def wait_complete():
   """
   等待当前音频播放完毕。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def download(name):
   """
   下载URL音频文件到/sdcard/music文件夹。

   :param name(str): 待下载音频文件URL信息
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def download_abort():
   """
   终止URL音频文件下载。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def cache_config(config_dict):
   """
   设置播放缓存区。示例::

      p0.cache_config({'place': 1, 'mem_size': 512, 'file_path': '/sdcard/'})

   :param config_dict(dictionary): 格式如示例所示，其中:

      place表示缓存文件存放的路径信息：0-空，1-文件系统，2-内存系统;

      mem_size表示缓存文件大小，单位是KB;

      file_path表示缓存文件全路径;
   :returns: 实际填充的音频数据，不大于nbytes数据；负数代表填充失败。
   :raises OSError: EINVAL
   """
   pass

def set_fade(in_ms, out_ms):
   """
   设置音频启动和停止的时候渐入渐出的时长，单位为毫秒。

   :param in_ms(int): 渐入时长
   :param out_ms(int): 渐出时长
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def set_format(format):
   """
   设置音频格式。

   :param format(int): 具体取值参考 ``set_stream``
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def set_standby(msec):
   """
   设置播放器静默时间，单位为毫秒。

   :param msec(int): 静默时间:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def eq_enable(enable):
   """
   是否使能音频播放器均衡功能。

   :param enable(int): 是否使能均衡: 0表示不使能，1表示使能
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def state_dump():
   """
   导出播放器当前播放状态。

   :param 空:
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def pcmdump_enable(enable):
   """
   是否使能导出播放器音频数据到指定目录，以便对音频播放数据分析。

   :param enable(int): 是否使能导出功能: 0表示不使能，1表示使能
   :returns: 0: 成功，其他: 失败
   :raises OSError: EINVAL
   """
   pass

def get_state():
   """
   获取播放器的状态。

   :param 空:
   :returns: 播放器状态，取值范围如下:

   .. c:enum:: player_state_t

      PLAYER_STAT_IDLE = 0,

      PLAYER_STAT_READY,

      PLAYER_STAT_RUNNING,

      PLAYER_STAT_PAUSED,

      PLAYER_STAT_RESUME,

      PLAYER_STAT_STOP,

      PLAYER_STAT_COMPLETE,

      PLAYER_STAT_SEEK_CPLT,

      PLAYER_STAT_MEDIA_INFO,

      PLAYER_STAT_SOURCE_INVALID,

      PLAYER_STAT_FORMAT_UNSUPPORT,

      PLAYER_STAT_ERROR,
   :raises OSError: EINVAL
   """
   pass

def get_delay():
   """
   获取播放器延迟信息，单位为毫秒。

   :param 空:
   :returns: 播放器延迟。
   :raises OSError: EINVAL
   """
   pass

def get_mediainfo():
   """
   获取当前播放的音频文件信息。

   :param 空:
   :returns: 当前音频文件信息，格式为dictionary：

      ‘name’: 名称

      ‘author’: 演唱者

      ‘album’: 专辑

      ‘year’: 发行年月

      ‘valid’: 有效性

      ‘type’: 音频类型

      ‘bitrate’: 音频码率

      ‘media_size’: 音频文件大小

      ‘duration: 音频文件时长
   :raises OSError: EINVAL
   """
   pass

def get_cacheinfo():
   """
   获取缓存信息。

   :param 空:
   :returns: 缓存区信息，格式为dictionary，具体格式参考 ``cache_config``。
   :raises OSError: EINVAL
   """
   pass

def format_support(format):
   """
   设置播放器支持的音源类型。

   :param format(int): 音源类型，具体内容参考 ``set_stream``
   :returns: 0: 成功，其他: 失败。
   :raises OSError: EINVAL
   """
   pass
