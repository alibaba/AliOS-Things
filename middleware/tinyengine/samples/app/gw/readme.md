# 功能介绍
模组当作一个网关，下挂灯和温湿度传感器两个子设备，每10秒获取一次温湿度，并上报云端，当湿度大于50%时控制开灯，小于50%时关灯，并把灯的状态上报云端

# 应用介绍

* [linkdevelop平台](https://linkdevelop.aliyun.com)
    登录账号：电子书测试账号002
    项目名字：gateway
    产品名称：网关
    URL： [https://linkdevelop.aliyun.com/p/a124XKk9O6qua0XK/device/productaccess/#/productaccess/productdetail?productKey=a1P3UMxL1e8&key=definition](https://linkdevelop.aliyun.com/p/a124XKk9O6qua0XK/device/productaccess/#/productaccess/productdetail?productKey=a1P3UMxL1e8&key=definition)

   **备注**：如果是开发者可以根据场景自行在linkdevelop平台注册网关及自设备，并更改sample代码中的三要素。

![image.png | left | 827x427](https://cdn.yuque.com/lark/0/2018/png/16861/1526382833993-87636872-bbf4-4f60-93bd-2cf8ae8f7a0c.png "")


* 模组IO配置

```json
{
  "GPIO": [
    {
      "id": "monochrome_light",
      "port": 22,
      "dir": 0,
      "pull": 1
    },
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



![image.png | left | 715x518](https://cdn.yuque.com/lark/0/2018/png/16861/1526383090959-f5b46877-be4c-476c-810e-cc502007d46e.png "")


# 云端控制

* 查看网关子设备



![image.png | left | 827x286](https://cdn.yuque.com/lark/0/2018/png/16861/1526383083484-dbe418da-d2b6-4db3-863f-d2fa09117e5c.png "")

还可以分别查看子设备详情

# 内存占用情况

* JS符号表

```bash
# meminfo 
 [4271.710]<W> main [handle_meminfo_cmd#227] : call handle_meminfo_cmd ...
 [4271.710]<W> BoneEngine [handle_meminfo_cmd#228] : memory usage= 941 max=1008
#
```

* 系统内存

```bash
# dumpsys mm_info
......
------------------------- memory allocation statistic ------------------------------ 
     free     |     used     |     maxused
      117680  |      123608  |      144504

-----------------number of alloc times:-----------------
[2^06] bytes: 87655   |[2^07] bytes: 16616   |[2^08] bytes:  6556   |[2^09] bytes:  9848   |
[2^10] bytes:   514   |[2^11] bytes: 66353   |[2^12] bytes:     7   |[2^13] bytes:     6   |
[2^14] bytes:     5   |[2^15] bytes:     0   |[2^16] bytes:     0   |[2^17] bytes:     0   |
[2^18] bytes:     0   |[2^19] bytes:     0   |[2^20] bytes:     0   |[2^21] bytes:     0   |
-----------------fix pool information:-----------------
     free     |     used     |     total
        4384  |        3808  |        8192

# 

```

