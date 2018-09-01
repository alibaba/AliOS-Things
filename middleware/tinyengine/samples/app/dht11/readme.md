# <a name="8mu2oq"></a>功能介绍
每10秒获取一次温湿度状态，并上报云端

# <a name="sov7cr"></a>应用介绍

* [linkdevelop平台](https://linkdevelop.aliyun.com)
    登录账号：<span data-type="color" style="color:rgb(55, 61, 65)">电子书测试账号002</span>
    项目名字：<span data-type="color" style="color:rgb(55, 61, 65)"><span data-type="background" style="background-color:rgb(255, 255, 255)">dht11</span></span>
    产品名称：<span data-type="color" style="color:rgb(55, 61, 65)"><span data-type="background" style="background-color:rgb(245, 246, 250)">温湿度传感器</span></span>
    URL： [https://linkdevelop.aliyun.com/p/a124XKk9O6qua0XK/device/productaccess/#/productaccess/productdetail?productKey=a1cXajyhMQ8&key=definition](https://linkdevelop.aliyun.com/p/a124XKk9O6qua0XK/device/productaccess/#/productaccess/productdetail?productKey=a1cXajyhMQ8&key=definition)



![image.png | left | 827x357](https://cdn.yuque.com/lark/0/2018/png/16861/1526351309360-57780572-0246-4f1e-a82b-513f9cf43b3f.png "")


* 模组IO配置
    DATA的连接在GPIO 19管脚上

```json
{
  "GPIO": [
    {
      "id": "dht11",
      "port": 19,
      "dir": 3,
      "pull": 1
    }
  ]
}
```

接线示意图：



![image.png | left | 743x663](https://cdn.yuque.com/lark/0/2018/png/16861/1526352038590-6b465420-e3d5-4d06-a16c-d938bea10c90.png "")


# <a name="xz7eng"></a>云端控制

* 查看当前温湿度



![image.png | left | 827x317](https://cdn.yuque.com/lark/0/2018/png/16861/1526352116459-be444d3c-8f19-4b50-b706-6f3cfca2010d.png "")


# <a name="c1s6wy"></a>内存占用情况

* JS符号表

```bash
# meminfo 
 [3137.460]<W> main [handle_meminfo_cmd#227] : call handle_meminfo_cmd ...
 [3137.460]<W> BoneEngine [handle_meminfo_cmd#228] : memory usage= 474 max=540

#
```

* 系统内存

```bash
# dumpsys mm_info
......
------------------------- memory allocation statistic ------------------------------ 
     free     |     used     |     maxused
      119800  |      121544  |      128160

-----------------number of alloc times:-----------------
[2^06] bytes: 63121   |[2^07] bytes:  8064   |[2^08] bytes:  2617   |[2^09] bytes:  2903   |
[2^10] bytes:   306   |[2^11] bytes: 51079   |[2^12] bytes:     8   |[2^13] bytes:     5   |
[2^14] bytes:     5   |[2^15] bytes:     0   |[2^16] bytes:     0   |[2^17] bytes:     0   |
[2^18] bytes:     0   |[2^19] bytes:     0   |[2^20] bytes:     0   |[2^21] bytes:     0   |
-----------------fix pool information:-----------------
     free     |     used     |     total
        4800  |        3392  |        8192

# 

```

