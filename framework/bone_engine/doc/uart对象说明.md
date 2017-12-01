
# uart对象说明

uart对象是一个全局对象，能够在任意位置访问。



## uart 介绍


## 属性



## 方法

### **1. uart.open(config)**
打开指定串口，返回值数字
- 0： 成功

输入参数:
- config : 串口配置，类型为Object
    对象属性如下:
    - port     ： 串口号，必选
    - baudRate ： 波特率，可选，默认115200
    - dataBits ： data bits, 可选， 默认8

```
var config={};
config.port=1;
config.baudRate=115200;
config.dataBits=8;
var serial=uart.open(config);
print('uart.open = ' + serial);
```
输出
```
BoneEngine > uart.open = 1
```

### **2. uart.close(port)**
关闭指定串口, 返回值数字
- 0： 成功

输入参数:
- port : 串口号



### **3. uart.write(port, data)**
写指定串口，返回值数字
- 0： 成功

输入参数:
- port : 串口号
- data : 类型为String，**长度限制为127Byte**


### **4. uart.read(port, [timeout])**
读指定串口，返回值读到的字符串


输入参数:
- port    : 串口号
- timeout : 读取串口超时值，单位是毫秒, 可选

**若无timeout参数，则超时值为0**
**读取字符串长度最大127Byte**

## 事件
 