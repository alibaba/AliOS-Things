## 用法

alios-things 支持将特定的目标转化成keil和iar来进行开发。自动生成keil或iar工程的命令就是在原有构建命令的基础上加上 IDE=keil 或 IDE=iar 的选项。如：

`aos make mqttapp@b_l475e IDE=keil`

`aos make mqttapp@b_l475e IDE=iar`

执行后会产生如下打印，显示自动生成工程所在的位置：

![](https://img.alicdn.com/tfs/TB1J6ZEpDtYBeNjy1XdXXXXyVXa-818-68.png)

在projects目录下相应路径中即可找到所生成的工程



## 生成结果介绍

在显示的路径下，找到生成的工程，按照一般 keil，iar的用法使用即可

![](https://img.alicdn.com/tfs/TB1Rx1upAyWBuNjy0FpXXassXXa-335-110.png)

![](https://img.alicdn.com/tfs/TB14zRDpwmTBuNjy1XbXXaMrVXa-371-90.png)

**需要注意的是**：在自动生成的工程目录下，有一个opts目录，其中存储了工程所要用到的选项。在工程设置中，指定了从文件中读取选项。所以这个目录不能缺少。



## 支持情况

1. 目前支持 b_l475e 和 startkit 两个board工程自动生成。其他stm32系列的自动生成的工程可能需要人为做一些修改。
2. 转化出的工程默认未开优化选项
3. 工程内存信息，可能与board实际的有所差别，需要完善