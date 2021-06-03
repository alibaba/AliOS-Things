# * coding: UTF8 *
"""
该模块实现相应CPython模块的子集，如下所示,它提供了对 OSError 异常的符号错误代码的相关常量


常量
-----------

.. data:: errorcode

.. data:: EPERM
.. data:: ENOBUFS
.. data:: ENODEV
.. data:: ENOENT
.. data:: ENOMEM
.. data:: ENOBUFS
.. data:: ENODEV
.. data:: ENOENT
.. data:: ENOMEM
.. data:: ENOTCONN
.. data:: EOPNOTSUPP
.. data:: ETIMEDOUT
.. data:: EIO
.. data:: EAGAIN
.. data:: EACCES
.. data:: EEXIST
.. data:: EISDIR
.. data:: EINVAL
.. data:: EADDRINUSE
.. data:: ECONNABORTED
.. data:: ECONNREFUSED
.. data:: ECONNRESET
.. data:: EHOSTUNREACH
.. data:: EALREADY
.. data:: EINPROGRESS


使用示例
:::::::::

.. code-block:: python
    :linenos:

    import uerrno
    print(uerrno.ETIMEDOUT)
    print(errno.errorcode)

输出结果
:::::::::

.. code-block:: JSON
    :linenos:

    >>>116
    >>>{1: 'EPERM', 2: 'ENOENT', 5: 'EIO', 9: 'EBADF', 11: 'EAGAIN', 12: 'ENOMEM', 13: 'EACCES', 17: 'EEXIST', 19: 'ENODEV', 21: 'EISDIR', 22: 'EINVAL', 95: 'EOPNOTSUPP', 112: 'EADDRINUSE', 113: 'ECONNABORTED', 104: 'ECONNRESET', 105: 'ENOBUFS', 128: 'ENOTCONN', 116: 'ETIMEDOUT', 111: 'ECONNREFUSED', 118: 'EHOSTUNREACH', 120: 'EALREADY', 119: 'EINPROGRESS'}
    
"""


errorcode = {1: 'EPERM', 2: 'ENOENT', 5: 'EIO', 9: 'EBADF', 11: 'EAGAIN', 12: 'ENOMEM', 13: 'EACCES', 17: 'EEXIST', 19: 'ENODEV', 21: 'EISDIR', 22: 'EINVAL', 95: 'EOPNOTSUPP', 112: 'EADDRINUSE', 113: 'ECONNABORTED', 104: 'ECONNRESET', 105: 'ENOBUFS', 128: 'ENOTCONN', 116: 'ETIMEDOUT', 111: 'ECONNREFUSED', 118: 'EHOSTUNREACH', 120: 'EALREADY', 119: 'EINPROGRESS'}


