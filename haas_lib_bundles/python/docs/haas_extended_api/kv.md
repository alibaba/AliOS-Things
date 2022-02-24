# KV - 键值对存储

* 模块功能：  
基于键值对（KV：Key-Value）的存储功能

* 注意事项：  
当前只支持Nor flash设备


## set - 写入kv键值对

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
key|String|是|kv中的key值
value|String|是|kv中的value值

* 返回值：  
kv键值对写入成功返回0；写入失败返回非0

* 示例： 

```python
import kv

TEST_KEY   = 'test_kv_key'
TEST_VALUE = 'test_kv_value'

ret = kv.set(TEST_KEY, TEST_VALUE)
print(ret)
```
* 输出：

```
0
```


## get  - 根据key值获取value值

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
key|String|是|kv中的key值

* 返回值：  
键值对中的value值

* 示例： 

```python
val = kv.get(TEST_KEY)
print(val)
```
* 输出：

```
test_kv_value
```


## remove  - 删除kv键值对

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
key|String|是|kv中的key值

* 返回值：  
kv键值对删除成功返回0；删除失败返回非0

* 示例： 

```python
val = kv.remove(TEST_KEY)
print(val)
```
* 输出：
```
0
```


## 使用实例
* 代码：

```python
import kv

TEST_KEY   = 'test_kv_key'
TEST_VALUE = 'test_kv_value'

"""
测试标题：set/get API功能测试
输入条件：key-val键值对(String类型)
测试步骤：
    1. 写入kv键值对key-value
    2. 读出键值对，检查读出的value值
预期结果：读出的value与写入的相同
"""
ret = kv.set(TEST_KEY, TEST_VALUE)
if ret == 0:
	print("kv set success")

val = kv.get(TEST_KEY)
if val == TEST_VALUE:
    print("kv get success")

ret = kv.remove(TEST_KEY)
if ret == 0:
	print("kv del success")
```

* 输出：

```python
kv set success
kv get success
kv del success
```
