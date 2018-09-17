# <a name="8mu2oq"></a>功能介绍
接收云端下发属性来控制灯的开关状态，并更新灯的状态至云端。

# <a name="sov7cr"></a>应用介绍

* [linkdevelop平台](https://linkdevelop.aliyun.com)
    登录账号：<span data-type="color" style="color:rgb(55, 61, 65)">电子书测试账号002</span>
项目名字：gravity\_lite
产品名称：单色LED灯
URL： [https://linkdevelop.aliyun.com/p/a124XKk9O6qua0XK/device/productaccess/#/productaccess/productdetail?productKey=a1Jmy99o5NS&key=definition](https://linkdevelop.aliyun.com/p/a124XKk9O6qua0XK/device/productaccess/#/productaccess/productdetail?productKey=a1Jmy99o5NS&key=definition)



![image.png | left | 827x366](https://cdn.yuque.com/lark/0/2018/png/16861/1526264657058-2693559f-caf0-4351-89c0-2d3f6a96e5a6.png "")



* 模组IO配置
单色灯的连接在GPIO 22管脚上，配置成输出模式
GPIO配置成输出模式， 默认配置成高阻
```json
{
  "GPIO": [
    {
      "id": "monochrome_light",
      "port": 22,
      "dir": 0,
      "pull": 1
    }
  ]
}
```

LED三级管接线示意图：
长边接3.3VDC, 串接一个200欧姆的电阻，LED的输入电流需要小于20MA  (5.0 VDC + 300Ω)
短边接GPIO 22


![image.png | left | 680x540](https://cdn.yuque.com/lark/0/2018/png/16861/1526281048424-f869bb73-0b49-4feb-8006-dc2579563192.png "")


# <a name="xz7eng"></a>云端控制

* 控制LED灯状态


![image.png | left | 827x305](https://cdn.yuque.com/lark/0/2018/png/16861/1526288991164-649a7805-bf8a-423a-bca0-eb427eb5be48.png "")



* 查看LED灯当前状态


![image.png | left | 827x316](https://cdn.yuque.com/lark/0/2018/png/16861/1526289040213-2a5c1e57-a790-4fbb-974d-920ab139ab1e.png "")



# <a name="c1s6wy"></a>内存占用情况

* JS符号表

```bash
# meminfo 
 [ 152.680]<W> main [handle_meminfo_cmd#227] : call handle_meminfo_cmd ...
 [ 152.680]<W> BoneEngine [handle_meminfo_cmd#228] : memory usage= 454 max=516

#
```


* 系统内存

```bash
# dumpsys mm_info
......
------------------------- memory allocation statistic ------------------------------ 
     free     |     used     |     maxused
      119848  |      121496  |      131512

-----------------number of alloc times:-----------------
[2^06] bytes:  4561   |[2^07] bytes:   711   |[2^08] bytes:   268   |[2^09] bytes:   289   |
[2^10] bytes:    52   |[2^11] bytes:  3485   |[2^12] bytes:    12   |[2^13] bytes:     5   |
[2^14] bytes:     5   |[2^15] bytes:     0   |[2^16] bytes:     0   |[2^17] bytes:     0   |
[2^18] bytes:     0   |[2^19] bytes:     0   |[2^20] bytes:     0   |[2^21] bytes:     0   |
-----------------fix pool information:-----------------
     free     |     used     |     total
        4864  |        3328  |        8192

# 

```


