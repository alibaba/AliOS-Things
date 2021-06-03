# -*- coding: UTF-8 -*-
"""

模块功能:提供操作系统相关的功能函数

"""


def set_header(header):
    """
    设置请求header参数

    :param header(str): 设置请求的header参数
    :returns: 0: 成功，其他: 失败

    """
    pass

def open(path, mode):
    """
    打开文件，具体使用方式参考Linux C下的fopen函数。

    :param path(str): 待打开的文件路径及文件名;
    :param mode(str): 待打开的文件长度操作形态;
    :returns(nil): 若文件打开成功，返回值为文件流指针。若文件打开失败, 则返回Null。
    :raises OSError: ENOENT
    """
    pass

def read(buf, size, nmemb, stream):
    """
    从文件流读取数据，具体使用方式参考Linux C下的fread函数。

    :param buf(bytearray): 数据空间，用来存放读取进来的数据;
    :param size(int): 待读取的每个数据项占据的字节数;
    :param nmemb(int): 待读取的数据项数目，读取的总字符数以参数size*nmemb来决定;
    :param stream(nil): 通过open函数获取到的已打开的文件指针;
    :returns: 实际读取到的总字符数目。
    :raises OSError: EINVAL
    """
    pass

def write(buf, size, nmemb, stream):
    """
    从文件流写入数据，具体使用方式参考Linux C下的fwrite函数。

    :param buf(bytearray): 数据空间，用来存放待写入的数据;
    :param size(int): 待写入的每个数据项占据的字节数;
    :param nmemb(int): 待写入的数据项数目，写入的总字符数以参数size*nmemb来决定;
    :param stream(nil): 通过open函数获取到的已打开的文件指针;
    :returns: 实际写入到的总字符数目。
    :raises OSError: EINVAL
    """
    pass

def close(stream):
    """
    关闭文件，具体使用方式参考Linux C下的fclose函数。

    :param stream(nil): 通过open函数获取到的已打开的文件指针;
    :returns: 0: 成功，其他: 失败
    :raises OSError: EINVAL
    """
    pass

def seek(buf, size, nmemb, stream):
    """
    移动文件流的读写位置，具体使用方式参考Linux C下的fseek函数。

    :param stream(nil): 通过open函数获取到的已打开的文件指针;
    :param offset(int): 相对 whence 的偏移量，以字节为单位;
    :param whence(int): 开始添加偏移 offset 的位置，它一般指定为: SEEK_SET, SEEK_CUR和SEEK_END;
    
    :returns: 0: 成功，其他: 失败
    :raises OSError: EINVAL
    """
    pass

def tell(stream):
    """
    获取给定流stream的当前文件位置，具体使用方式参考Linux C下的ftell函数。

    :param stream(nil): 通过open函数获取到的已打开的文件指针;
    
    :returns: 若获取成功则返回位置标识符的当前值；若获取失败则返回-1L。
    :raises OSError: EINVAL
    """
    pass

def rewind(stream):
    """
    设置文件位置为给定流stream的文件的开头，具体使用方式参考Linux C下的rewind函数。

    :param stream(nil): 通过open函数获取到的已打开的文件指针;
    
    :returns: 空
    :raises OSError: EINVAL
    """
    pass

def getpos(stream):
    """
    获取流 stream 的当前文件位置，具体使用方式参考Linux C下的fgetpos函数。

    :param stream(nil): 通过open函数获取到的已打开的文件指针;
    
    :returns: 如果成功则返回当前文件位置，如果失败则返回-1。
    :raises OSError: EINVAL
    """
    pass

def setpos(stream, pos):
    """
    设置给定流 stream 的文件位置为给定的位置，具体使用方式参考Linux C下的fsetpos函数。
    
    :param stream(nil): 通过open函数获取到的已打开的文件指针;
    :param pos(int): 需要设置的文件位置;
    :returns: 0: 成功，其他: 失败
    :raises OSError: EINVAL
    """
    pass