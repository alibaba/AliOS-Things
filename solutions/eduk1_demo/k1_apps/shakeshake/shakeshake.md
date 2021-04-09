@page shakeshake 分歧争端机
# 实验介绍
分歧争端机使用的场景是当出现分歧时，分别摇一摇HaaS EDU K1来摇出一个0到100之间的数字，通过比较谁摇出的数字大来解决分歧的一种方法。本质是通过摇一摇这个动作来随机产生一个数字。这个实验的核心如何通过加速度传感器来检测到摇一摇这个动作，并随机生成一个数字。


**显示效果**

<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN019OP4kF22I19ZudgVF_!!6000000007096-1-tps-1200-800.gif" style="zoom:50%;" />
</div>

# 涉及知识点

- 三轴加速度传感器的原理和使用
- 随机数生成的方法
- OLED绘图


# 开发环境准备
## 硬件
    开发用电脑一台
    HAAS EDU K1 开发板一块
    USB2TypeC 数据线一根

## 软件
### AliOS Things开发环境搭建
    开发环境的搭建请参考 @ref HaaS_EDU_K1_Quick_Start (搭建开发环境章节)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

### HaaS EDU K1 DEMO 代码下载
    HaaS EDU K1 DEMO 的代码下载请参考 @ref HaaS_EDU_K1_Quick_Start (创建工程章节)，其中，
    选择解决方案: 基于教育开发板的示例
    选择开发板: haaseduk1 board configure

### 代码编译、烧录
    参考 @ref HaaS_EDU_K1_Quick_Start (3.1 编译工程章节)，点击 ✅ 即可完成编译固件。
    参考 @ref HaaS_EDU_K1_Quick_Start (3.2 烧录镜像章节)，点击 "⚡️" 即可完成烧录固件。




# 硬件介绍 - MPU-6050
请参考《陀螺仪小球》中的相同章节。


# 应用开发
     本实验通过MPU_Get_Accelerometer去获得三轴加速度传感器的原始值。然后计算出在y方向上和z方向上的差值，作为震动大小的强弱标志。当两个方向上的震动大小都超过了阈值，就认为是一次有效的摇晃，就生成随机数。生成随机的方法直接采用OS自动的rand()方法，他的返回值对100进行取模，保证是一个小于100的数字。然后将这个数据显示在OLED屏幕上。


```
void shakeshake_task()
{
    while (1)
    {
        // 获取三轴加速度信息
        MPU_Get_Accelerometer(&ax, &ay, &az);
        // 计算出y轴和z轴方向上的变化量
        y_change = (ay >= ay_pre) ? (ay - ay_pre) : (ay_pre - ay);
        z_change = (az >= az_pre) ? (az - az_pre) : (az_pre - az);
        // 变化量都超过阈值，则进行下一步处理
        if ((y_change > SHAKE_Y_THRESHOLD) && (z_change > SHAKE_Z_THRESHOLD))
        {
            // 生成100以下的随机数
            rand_value = rand() % 100;
            // 将随机数转化为需要显示的字符
            itoa(rand_value, showstr, 10);
            // 清除屏幕memory
            OLED_Clear();
            // 画出显示的图标
            OLED_Icon_Draw(50, 0, &icon_shakeshake_32_32_v2, 1);
            // 画出显示的字符
            OLED_Show_String(54, 36, showstr, 24, 1);
            // 画出左右翻页的图标
            OLED_Icon_Draw(2, 24, &icon_skip_left, 0);
            OLED_Icon_Draw(122, 24, &icon_skip_right, 0);
            // 将屏幕memory显示出来
            OLED_Refresh_GRAM();
        }
        // 保存本次y轴和z轴方向上的值
        az_pre = az;
        ay_pre = ay;
        // 暂停50ms
        aos_msleep(50);
    }
}
```
