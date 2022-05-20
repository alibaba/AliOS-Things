# sntp  - 获取并设定ntp时间


- 模块功能：  
获取并设定ntp时间

## setTime - 设置时间

-  函数功能：  
指定时区（默认“CST-8”）或者授时服务器（默认“cn.pool.ntp.org”），获取并设定ntp时间。

-  函数原型：
> sntp.setTime([zone], [ntp_server_addr])

- 参数说明：

| 参数 | 类型 | 必选参数？ | 说明 |
| --- | --- | --- | --- |
| zone | string | 否 | 时区信息 |
| ntp_server_addr | string | 否 | 授时服务器地址 |


-  返回值：  
无

-  示例： 

```python
import sntp

sntp.setTime("CST-8", "cn.pool.ntp.org")

```