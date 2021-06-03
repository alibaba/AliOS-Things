# * coding: UTF8 *
"""
该模块实现相应CPython模块的子集，如下所示

  

"""

class array(object):

    """
    使用给定类型的元素创建数组。数组的初始内容由 iterable 给定。若未给定内容，则创建一个空数组
    """

    def __init__(self,typecode,iterable=None):
        pass

    def append(self,val):
        """
        将新元素添加到数组末尾，并不断增加
        """
        pass

    def extend(self,iterable):

        """
        将包含在迭代中的新元素添加到数组末尾，并不断增加
        """
        pass