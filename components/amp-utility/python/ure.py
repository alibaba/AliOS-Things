# * coding: UTF8 *
"""

该模块实现相应CPython模块的子集，如下所示。

该模块实现正则表达式操作。支持的正则表达式语法是CPython re 模块的一个子集




``.``
   匹配任何字符

``[]``
   匹配字符集。支持单个字符和范围

``^``
   匹配开头

``$``
   匹配结尾


函数
------------------------------   

"""




def compile(regex_str):
   """
   编译正则表达式，返回 正则表达式对象 对象

   使用示例::

      import ure

      pattern = ure.compile("d")

   """
   pass

def match(regex_str, string):
   """
   将 正则表达式 str  与 string 匹配。匹配通常从字符串的起始位置进行

   使用示例::

      import ure

      ure.match("d","dog")

   """
   pass

def  search(regex_str, string):
   """
   在 string 中搜索 正则表达式str 。与 match 不同，这将首先搜索与正则表达式相匹配的字符串

   使用示例::

      import ure

      ure.search("d","abcd")
   """
   pass

