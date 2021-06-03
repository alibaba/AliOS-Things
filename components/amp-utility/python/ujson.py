# * coding: UTF8 *
"""


该模块实现相应 `CPython` 模块的子集，如下所示。

该模块允许Python对象和JSON数据格式之间的转换。


函数
------------------------------   

"""



def dumps(obj):
   """
   将字典类型的数据obj转换为str


   json转str::

      import ujson

      data = {
        'id': 'EventTest' ,
        'params': {
        'test_event' : 100
        }
      }

      str = ujson.dumps(data)
      print(str)
      print(type(str))

   期望输出::

      {"id": "EventTest", "params": {"test_event": 100}}
      <class 'str'>
   """

   pass

def loads(str):
   """
   将JSON ``str`` 转换为字典对象

   str转json::

      import ujson

      strDic = '{"name":"Lihua","ages":18,"weigth":140,"heigth":170}'
      str = ujson.loads(strDic)
      print(str)
      print(type(str))


   期望输出::

      {'heigth': 170, 'ages': 18, 'name': 'Lihua', 'weigth': 140}
      <class 'dict'>

   """
   pass