# modem - 设备模块

* 模块功能：
获取设备信息，实现设备间通信等功能


## sms - 短信功能

* 函数原型：
> sms()

* 参数说明：  
无

## setCallbacks - 实现接收短信功能

* 函数功能：  
实现接收短信功能

* 函数原型：
> sms.setCallbacks(sms_cb)

* 参数说明： 

|参数|参数类型|说明|
|-----|----|:---:|
sms_cb|function|短信回调函数，回调具体形式及用法见示例|

* 示例： 

```python
from modem import sim
import utime as time

def sms_cb(args):
    #字符串
    ind_flag = args[0]
    mes_buf = args[1]
    mes_num = args[2]
    # print("type of mes_buf:",type(mes_buf))
    print('new message:ind_flag:{},mes_buf:{},mes_num:{}'.format(ind_flag,mes_buf,mes_num))

if __name__ == '__main__':
    #实例化
    smss = sms()
    #接收短信回调
    time.sleep(5)
    smss.setCallback(sms_cb)

```

## sendTextMsg - 短信发送

* 函数功能：  
短信发送

* 函数原型：
> sms.sendTextMsg(phoneNumber, msg, codeMode)

* 参数说明：  

|参数|参数类型|说明|
|-----|----|----|
|phoneNumber|string|短信接收号码|
|msg|string|短信内容|
|codeMode|string|使用的字符编码方式，'GSM' - GSM编码方式；'UCS2' - UCS2编码方式|


字符编码方式|说明|
|-----|----|
|'GSM' |GSM编码方式，用于发送英文短信|
|'UCS2'|UCS2编码方式，可以用于发送中文短信以及英文短信|


* 示例： 

```python
from modem import sms
import utime as time

if __name__ == '__main__':
    #实例化
    smss = sms()
    time.sleep(2)
    smss.sendTextMsg('yourPhoneNumber','welcome to use haas506','GSM')
    smss.sendTextMsg('yourPhoneNumber','欢迎使用haas506','UCS2')
    # GSM 只支持英文内容
    # UCS2 支持中英文，且发送时文件格式保存位utf-8


```

