# pwm - PWM脉宽调制

* 类功能：  
PWM脉宽调制驱动。


## PWM - 创建PWM对象
* 函数原型：
> pwmObj = PWM()

* 参数说明：  
无

* 返回值：  
PWM对象成功，返回PWM对象；PWM对象创建失败，返回None

* 示例代码： 

```python
from driver import PWM

pwmObj = PWM()
```


## open - 打开PWM

* 函数功能：  
根据board.json中设备节点的配置打开PWM设备

* 函数原型：
> PWM.open(nodeName)

* 参数说明：  

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|nodeName|string|是|board.json配置文件中PWM设备节点的名称|

<br>

board.json中的PWM类型设备属性配置项说明如下：
|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|port|Number|1|是|配置端口值，这里跟芯片 datasheet上的端口对应|


> ESP32文档中对于PWM有相应介绍，当PWM的频率设置为5kHz时，可以具有13位最大占空比分辨率，占空比可以设置为0~100%的任何位置。如果当调整为最大的40MHz时，占空比分辨率为1位，意味着占空比固定位50%且无法调整。

<br>

* 返回值：  
打开PWM设备成功返回0；打开PWM设备失败返回失败错误码

* 示例： 

```python
# 假设PWM 25通道外接了一个蜂鸣器，在board.json中将此节点命名为"buzzer"，则board.json中的内容如下：
'''
{
    "name": "board-name",
    "version": "1.0.0",
    "io": {
        "buzzer": {
        "type": "PWM",
        "port": 25
        }
    }
}
'''

from driver import PWM

pwmObj = PWM()
ret = pwmObj.open("buzzer")

print(ret)
```

* 输出：

```
0
```

## close - 关闭PWM功能

* 函数功能：  
关闭PWM脉宽调制功能

* 注意事项：  
需确保要关闭的PWM处于open状态

* 函数原型：
> PWM.close()

* 参数说明：  
无

<br>

* 返回值：  
关闭PWM设备成功返回0；关闭PWM设备失败返回失败错误码

* 示例： 

```python
# 假设PWM 25通道外接了一个蜂鸣器，在board.json中将此节点命名为"buzzer"

from driver import PWM

pwmObj = PWM()
ret = pwmObj.open("buzzer")
ret = pwmObj.close()
print(ret)
```

* 输出：

```
0
```

## setOption - 设置脉宽调制参数

* 函数功能：  
设置脉宽调制参数，包括freq和duty的值

* 注意事项：  
需确保此PWM处于open状态

* 函数原型：
> PWM.setOption(Dictionary option)

* 参数说明：
设置的参数为字典类型，字典包含 freq 和 duty 两个值。
如： {'freq':2000, 'duty': 0 }

|属性字段|数据类型|属性值|必选项？|字段说明|
|-----|:---:|----|:---:|----|
|freq|Number|1|是|PWM信号频率配置，这里跟芯片 datasheet上的说明对应|
|duty|Number|1|是|PWM占空比配置，这里按照百分数计算，设置50即为占空比50%|


* 返回值：  
读取成功则返回0并按照设置的值输出PWM波形；负数代表读取数据失败

* 示例： 

```python
# 假设PWM 25通道外接了一个蜂鸣器，在board.json中将此节点命名为"buzzer"

from driver import PWM

pwmObj = PWM()
ret = pwmObj.open("buzzer")

data_r = {'freq':2000, 'duty': 100 }
ret = pwmObj.setOption(data_r)

print(ret)
```

* 输出：

```
0
```

<br>

## getOption - 读取设置的参数

* 函数功能：  
读取PWM设置的参数值

* 注意事项：  
需确保此PWM处于open状态

* 函数原型：
> PWM.getOption()

* 参数说明：
无

* 返回值：  
读取成功返回参数字典，和设置的参数字典结构相同。

* 示例： 

```python
# 假设PWM 25通道外接了一个蜂鸣器，在board.json中将此节点命名为"buzzer"

from driver import PWM

pwmObj = PWM()
ret = pwmObj.open("buzzer")

ret = pwmObj.getOption()

print(ret)
pwmObj.close()
```

* 输出：

```
{'duty': 100, 'freq': 1000}
```

## 使用实例

* 代码：

```python
from driver import PWM
from time import sleep

buzzer = PWM()
buzzer.open("buzzer")

# make buzzer ring
def buzzeStartRing():
    global buzzer

    # assumes buzzer rings when freq is between 200~9000
    param = {'freq':2000, 'duty': 3 }
    buzzer.setOption(param)

# stop buzzer ring
def buzzeStopRing():
    global buzzer

    # assumes buzzer rings when freq is between 200 and 9000, so set PWM's frequence to 100 will stop buzzer ring
    param = {'freq':100, 'duty': 3 }
    buzzer.setOption(param)

# main entry
if __name__ == "__main__":
    while True:
        print('buzzer start to ring')
        buzzeStartRing()
        sleep(3)
        print('buzzer stop to ring')
        buzzeStopRing()
        sleep(3)
```

* 输出：

```python
...
buzzer start to ring
buzzer stop to ring
...
```


<br>
