
# process对象说明

process对象是一个全局对象，能够在任意位置访问。



## process 介绍


## 属性

### **1. process.version**
获取BoneEngine版本信息

### **2. process.platform**
获取BoneEngine的运行平台信息



## 方法

### **1. process.nextTick(callback)**
在下一个事件循环中调用callback函数
```
process.nextTick(function(){
    print('process.uptime()=' + process.uptime());
    print(process.memoryUsage());
});
```
输出
```
BoneEngine > process.uptime()=0 
BoneEngine > { heapTotal: 500,
 heapUsed: 138 } 
```

### **2. process.uptime()**
返回`AOS`系统已经运行的秒数


### **3. process.memoryUsage()**
返回一个String对象，描述当前JS内存使用情况

```
process.nextTick(function(){
    print('process.uptime()=' + process.uptime());
    print(process.memoryUsage());
});
```
输出
```
BoneEngine > { heapTotal: 500,
 heapUsed: 138 }
```

## 事件