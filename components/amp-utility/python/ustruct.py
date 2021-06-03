# * coding: UTF8 *
"""
该模块实现的功能是原始数据类型打包和解压缩

支持的大小/字节顺序前缀: @, <, >, !.

支持的格式编码: b, B, h, H, i, I, l, L, q, Q, s, P, f, d 后两个取决于浮点支持

函数
----------

"""



def calcsize(fmt):
   """
   返回需存入给定 fmt 的字节数量
   """
   pass

def pack(fmt, v1, v2):
   """
   根据格式字符串fmt，打包 v1, v2, … 值。返回值为一个解码该值的字节对象
   """
   pass

def pack_into(fmt, buffer, offset, v1, v2):

   """
   根据格式字符串fmt，将 v1, v2, … 值打包进从 offset 开始的缓冲区。从缓冲区的末端计数， offset 可能为负值

   """
   pass

def unpack(fmt, data):

   """
   根据格式字符串 fmt 对数据进行解压。返回值为一个解压值元组
   """
   pass

def unpack_from(fmt, data, offset=0):

   """
   根据格式字符串 fmt 解压从 offset 开始的数据。从缓冲区的末端计数， offset 可能为负值。返回值是一个解压值元组

   """
   pass
