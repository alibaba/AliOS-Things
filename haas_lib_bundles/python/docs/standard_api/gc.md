# gc - 内存碎片回收

* 模块功能:  
gc 模块实现内存垃圾回收机制。

## enable - 启用自动回收内存碎片机制

* 函数原型：
> gc.enable()

## disable - 禁用自动回收机制

* 函数原型：
> gc.disable()

## collect - 回收内存碎片

* 函数原型：
> gc.collect()

## mem_alloc - 返回分配的堆RAM的字节数

* 函数原型：
> gc.mem_alloc()

* 返回值：  
返回已分配的堆内存的字节数。

* 示例：
```python
>>> gc.mem_free()
52096
```

## mem_free - 返回可用堆RAM的字节数

* 函数原型：
> gc.mem_free()

* 返回值：  
返回可用堆内存的字节数，如果此数量未知，则返回-1。

* 示例：
```python
>>> gc.mem_free()
52096
```

