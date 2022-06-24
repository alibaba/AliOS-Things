# modem - 设备模块

* 模块功能：
获取设备信息，实现设备间通信等功能

## voiceCall - 通话功能

* 函数原型：
> voiceCall()

* 参数说明：  
无

## setCallback - 设置监听回调函数

* 函数功能：  
设置监听回调函数

* 函数原型：
> voiceCall.setCallback(voice_callback)

* 参数说明： 

|参数|参数类型|说明？|
|-----|----|:---:|
voice_callback|function|监听回调函数|



* 返回值： 
注册成功返回整型0，失败返回整型-1 。


## setAutoAnswer - 设置自动应答时间

* 函数功能：  
设置自动应答时间，来电时自动接通

* 函数原型：
>voiceCall.setAutoAnswer(ms)

* 参数说明：  

|参数|参数类型|说明|
|-----|----|----|
|ms|int|自动应答时间,单位ms|


* 示例： 

```python
from modem import voiceCall
from audio import Audio
import utime

def voice_callback(args):
  if args[0] == 10:
      print('voicecall incoming call, PhoneNO.: ', args[6])
  elif args[0] == 11:
      print('voicecall connected, PhoneNO.: ', args[6])
  elif args[0] == 12:
      print('voicecall disconnect')
  elif args[0] == 13:
      print('voicecall is waiting, PhoneNO.: ', args[6])
  elif args[0] == 14:
      print('voicecall dialing, PhoneNO.: ', args[6])
  elif args[0] == 15:
      print('voicecall alerting, PhoneNO.: ', args[6])
  elif args[0] == 16:
      print('voicecall holding, PhoneNO.: ', args[6])

if __name__ == '__main__':
  #实例化
  vc = voiceCall()
  ad = Audio()
  #电话功能需要预先开启功放
  ad.set_pa()
  #设置音量
  ad.setVolume(10)
  # 设置监听回调函数
  vc.setCallback(voice_callback)
  # 设置自动应答时间 ms
  vc.setAutoAnswer(5000)

```


## callStart- 拨打电话

* 函数功能：  
拨打电话

* 函数原型：
> voiceCall.callStart(phonenumber)

* 参数说明： 

|参数|参数类型|说明？|
|-----|----|:---:|
phonenumber|string|拨打的电话号码|

* 示例： 

```python
from modem import voiceCall
from audio import Audio
import utime


if __name__ == '__main__':
  #实例化
  vc = voiceCall()
  ad = Audio()
  #电话功能需要预先开启功放
  ad.set_pa()
  #设置音量
  ad.setVolume(10)
  utime.sleep(3)
  vc.callStart('PhoneNumber')

```


## callAnswer - 打电话来时接听电话

* 函数功能：  
打电话来时接听电话

* 函数原型：
> voiceCall.callAnswer()

* 参数说明：  
无

## callEnd - 挂断电话

* 函数功能：  
挂断电话

* 函数原型：
> voiceCall.callEnd()

* 参数说明：  
无

* 示例： 

```python
from modem import voiceCall
from audio import Audio
import utime

def voice_callback(args):
  if args[0] == 10:
      print('voicecall incoming call, PhoneNO.: ', args[6])
  elif args[0] == 11:
      print('voicecall connected, PhoneNO.: ', args[6])
  elif args[0] == 12:
      print('voicecall disconnect')
  elif args[0] == 13:
      print('voicecall is waiting, PhoneNO.: ', args[6])
  elif args[0] == 14:
      print('voicecall dialing, PhoneNO.: ', args[6])
  elif args[0] == 15:
      print('voicecall alerting, PhoneNO.: ', args[6])
  elif args[0] == 16:
      print('voicecall holding, PhoneNO.: ', args[6])

if __name__ == '__main__':
  #实例化
  vc = voiceCall()
  ad = Audio()
  #电话功能需要预先开启功放
  ad.set_pa()
  #设置音量
  ad.setVolume(10)
  utime.sleep(3)
  # 设置监听回调函数
  vc.setCallback(voice_callback)
  utime.sleep(2)
  vc.callAnswer()
  #挂断电话
  utime.sleep(10)
  vc.callEnd()



```
