# * coding: UTF8 *
"""
该模块实现相应CPython模块的子集，如下所示

"""




def decompress(data, wbits=0, bufsize=0):
   """
  
   
   **入参**
   
    - wbits 是压缩时使用的DEFLATE字典窗口大小（8-15，字典的大小为该数值的2次幂）。 另外，若该值为正， data 则被假定为zlib流（带有zlib首标）。否则，若该值为负， 则假定为原始DEFLATE流。 
    - bufsize 参数是为与CPython兼容，此处忽略。

   **返回值**

   将解压缩数据作为bytes类型返回
   """
   pass

def DecompIO(stream, wbits=0):
   """
   创建一个 `stream` 装饰器，该装饰器允许在另一个 *stream* 中进行压缩数据的透明解压。 
   这允许使用大于可用堆大小的数据处理压缩流。除 :func:`decompress` 中所述的值外， 
   *wbits* 可能取值 24..31 (16 + 8..15)，这也就意味着输入流带有gzip首标。
   """
   pass

