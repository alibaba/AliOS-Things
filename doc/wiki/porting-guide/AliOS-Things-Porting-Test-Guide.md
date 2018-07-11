EN| [中文](AliOS-Things-Porting-Test-Guide.zh)

# Content

- [1. Overview](#1)
- [2. Ways of using and customizing test suite](#2)
- [3. List of supported tests](#3)
- [4. Q&A](#4)

------

<h2 id="1">1.Overview</h2>

AliOS Things provides a set of basic test cases for system transplanting quality verification. Users can customize and adjust test suite based on their transplanted components.

This article will talk about the way of using and customizing AliOS test suite, list of test cases that have been supported, and how to add new test case.

<h2 id="2">2. Ways of using and customizing test suite </h2>  

AliOS Things has preset a test application named "YTS" in the "example" directory.
<h3 id="2.1">2.1 How to use the test suite : </h3>  

- Generate test application through `$ aos make yts@board_name`
  If specified test component is needed, you can set the "test" parameter:
  `$ aos make yts@board_name test=component_name`。   
- Burn the compiled binary file into the target board.
- When reset of target board is started, you can input "yts_run" command in the CLI environment to execute the specific test case: 
  ![](https://img.alicdn.com/tfs/TB1C4xOjZrI8KJjy0FhXXbfnpXa-528-430.png)    

<h3 id="2.2">2.2 How to customize the test suite : </h3>  

- Mark the test components list in "board_name.mk" file corresponding to the target board by variable "TEST_COMPONENTS" . (board_name.mk is generally located in the [board](https://github.com/alibaba/AliOS-Things/tree/master/board) directory)

  Take STM32's b_l475e (b_l475e.mk) as an example:

  ![](https://img.alicdn.com/tfs/TB1h_Xfj8fH8KJjy1XbXXbLdXXa-939-122.png)  

- The test component is specified by compiling the parameter "test" of the command:`$ aos make yts@board_name test=component_name`  

<h2 id="3">3. list of supported tests  </h2>  

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

- Q: I can't input command through CLI after target board burns the yts program .

  A: This is because CLI module hasn't been compiled into the application. Please make sure the mk file of your board has declared the dependency on cli, or you should add the dependency in yts.mk. `COMPONENTS += cli`。  

- Q: How can I just test Rhino kernel?  
  A: There are mainly two measures:  1. compile a test application that only tests Rhino  through`aos make yts@board_name test=rhino`；2. set the variable “TEST_COMPONENTS” in board_name.mk `TEST_COMPONENTS := rhino`.