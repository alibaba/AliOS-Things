 

# gpio对象说明

gpio对象是一个全局对象，能够在任意位置访问。



## gpio 介绍


## 属性



## 方法

### **1. gpio.open(id,config)**
配置gpio端口，返回值数字
- 0： 成功

输入参数:
- id : GPIO端口序号，类型为数字，从0开始
    GPIO id序号, 与实际板型硬件有映射, board.h中有配置
- config : GPIO端口属性配置, 类型为数字
    配置属性如下:
    - 0 ： INPUT_PULL_UP
    - 1 ： INPUT_PULL_DOWN
    - 2 ： INPUT_HIGH_IMPEDANCE
    - 3 ： OUTPUT_PUSH_PULL
    - 4 ： OUTPUT_OPEN_DRAIN_NO_PULL
    - 5 ： OUTPUT_OPEN_DRAIN_PULL_UP
 

### **2. gpio.write(id,value)**
配置GPIO id号对应的端口状态, 返回值数字
- 0： 成功

输入参数:
- id : GPIO端口序号，类型为数字，从0开始
- value : GHIO端口状态
    - \>0 : high
    - \<0 : low

### **3. gpio.read(id)**
获取GPIO指定端口的状态，返回值数字
- 1 : high
- 0 : low

**如果gpio pin配置成output mode，返回无效**

### **4. gpio.close(id)**
结束操作GPIO操作,返回值数字
- 0： 成功

输入参数:
- id : GPIO端口序号，类型为数字，从0开始


## 事件

