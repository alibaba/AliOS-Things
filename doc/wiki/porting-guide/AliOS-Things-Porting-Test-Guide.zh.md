[EN](AliOS-Things-Porting-Test-Guide) | 中文

# 目录  

* [1. 概述](#1)
* [2. 测试用例集的应用及定制方式](#2)
* [3. 已支持的测试用例列表](#3)
* [4. Q&A](#4)
------
<h2 id="1">1. 概述</h2>
AliOS Things 提供了一套基础的测试用例集来进行系统移植质量验证， 用户可根据移植的组件来进行定制化调整测试用例集。  
本文涉及的内容包括：AliOS测试用例集的使用方式、定制方式、已支持测试用例列表、添加自定义测试用例等。

<h2 id="2">2. 测试用例集的应用及定制方式</h2>  

AliOS Things 在“example”目录下已经预置了名为“[yts](https://github.com/alibaba/AliOS-Things/tree/master/example/yts)”的测试应用程序。  
<h3 id="2.1">2.1 测试用例集应用步骤：</h3>  

* 通过`$ aos make yts@board_name`生成测试应用程序。  
  若需指定测试组件，可通过“**test**”参数设置即:  
  `$ aos make yts@board_name test=component_name`。   

* 将编译生成的binary文件烧录到目标板中。  

* 目标板复位启动后，在cli环境下键入“**yts_run**”命令即可执行具体测试用例：
   ![](https://img.alicdn.com/tfs/TB1C4xOjZrI8KJjy0FhXXbfnpXa-528-430.png)    

<h3 id="2.2">2.2 测试用例集定制方式: </h3>  

* 在目标板对应的“**board_name.mk**”文件中通过“**TEST_COMPONENTS**”变量标示测试组件列表。(**board_name.mk**一般位于[board](https://github.com/alibaba/AliOS-Things/tree/master/board)目录)  
  以STM32的b_l475e([b_l475e.mk](https://github.com/alibaba/AliOS-Things/blob/master/board/b_l475e/b_l475e.mk))为例：
  ![](https://img.alicdn.com/tfs/TB1h_Xfj8fH8KJjy1XbXXbLdXXa-939-122.png)  

* 通过编译命令的“test”参数来指定测试组件：`$ aos make yts@board_name test=component_name`  

<h2 id="3">3. 已支持的测试用例列表 </h2>  

| Name             | Function              | Description                              |
| ---------------- | --------------------- | ---------------------------------------- |
| basic            | test_basic            | basic list structure test case(slist, dlist) |
| cjson            | test_cjson            | cjson component test case                |
| digest_algorithm | test_digest_algorithm | digest algorithm component test case     |
| hashtable        | test_hashtable        | hashtable component test case            |
| api              | test_api              | aos api test case                        |
| rhino            | test_rhino            | rhino kernel test case                   |
| vcall            | test_vcall            | vcall component test case (only for mico/b_l475e board) |
| vfs              | test_vfs              | vfs component test case                  |
| yloop            | test_yloop            | yloop component test case                |
| kv               | test_kv               | key-value store component test case      |
| fatfs            | test_fatfs            | fatfs file system component test case    |
| deviceIO         | test_deviceIO         | device IO component test case (only for linuxhost) |
| wifi_hal         | test_hal              | wifi HAL & timer HAL interface test case |
| netmgr           | test_netmgr           | netmgr component test case               |
| alink            | test_alink            | alink component test case                |
| mqtt             | test_mqtt             | mqtt component test case                 |
| coap             | test_coap             | coap component test case                 |
| fota             | test_fota             | fota component test case                 |
| alicrypto        | test_alicrypto        | alicrypto component test case            |
| tfs              | test_tfs              | tfs component test case                  |
| tls              | test_tls              | mbedtls component test case              |

<h2 id="4">4. Q&A </h2>  

* Q: 目标板烧录yts程序后，无法通过cli输入命令。  
  A: 这是cli模块未被编译链接进应用程序，请确认板级mk文件有声明cli组件依赖，或在yts.mk中添加cli组件依赖`COMPONENTS += cli`。  

* Q: 只移植了rhino内核，想单独测试rhino内核如何配置？  
  A: 两种方式: 1. 通过`aos make yts@board_name test=rhino`编译单独测试rhino内核的应用程序；2. 配置board_name.mk中的“TEST_COMPONENTS”变量`TEST_COMPONENTS := rhino`。