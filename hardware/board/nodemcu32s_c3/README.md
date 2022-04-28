@page nodemcu32s_c3 ESP32-C3开发板定义

[更正文档](https://gitee.com/alios-things/hardware/board/nodemcu32s_c3/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 概述
nodemcu32s_c3为HaaS-Python提供ESP32-C3开发板定义，以及编译指导

## 版权信息

> Apache license v2.0



## 编译指导

### 准备ESP-IDF v4.4环境

* ESP-IDF v4.4安装：

  ```bash
  cd /path/to/AliOS-Things
  cd hardware/chip/espressif_esp32_c3/
  git clone --recursive https://github.com/espressif/esp-idf.git -b release/v4.4
  cd esp-idf
  git submodule update --init --recursive
  ./install.sh
  ```



* 修改esp-idf目录的必要文件以适配AliOS Things底层：

  * hardware/chip/espressif_esp32_c3/esp-idf/components/fatfs/src/diskio.h

    ```c
     extern "C" {
     #endif
    
    + // add for HaaSPython
    + #include "ff.h"
    
     /* Status of Disk Functions */
     typedef BYTE   DSTATUS;
    ```
  
  * hardware/chip/espressif_esp32_c3/esp-idf/components/vfs/include/esp_vfs.h
  
    ```c
         esp_err_t (*end_select)(void *end_select_args);
     #endif // CONFIG_VFS_SUPPORT_SELECT
    +    // add for HaaSPython
    +    union {
    +        int (*statfs_p)(void *ctx, const char *path, void* sb);
    +        int (*statfs)(const char *path, void *sb);
    +    };
     } esp_vfs_t;
    ```
    



### 编译准备：

* 4MB Flash芯片：

  * 无需修改配置，可直接下一步

* 2MB Flash芯片：

  * 辨识：在主控芯片上，会标注2M或者C3-2M

  * 修改 components/py_engine/adapter/esp32/boards/GENERIC_C3/sdkconfig.board，启用如下信息：

    ```ini
    # 2MB规格
    CONFIG_PARTITION_TABLE_OFFSET=0x9000
    CONFIG_ESPTOOLPY_FLASHSIZE_2MB=y
    CONFIG_PARTITION_TABLE_CUSTOM=y
    CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions-2MiB.csv"
    ```


### 编译：

在HaaS Studio中，选择py_engine_esp32_demo@nodemcu32s_c3项目，然后正常编译即可



### 烧录：

```bash
cd /path/to/AliOS-Things
export PATH=$(pwd)/hardware/chip/espressif_esp32_c3/esp-idf/components/esptool_py/esptool/:$PATH

esptool.py --chip esp32c3 erase_flash
esptool.py --chip esp32c3 --baud 460800 write_flash -z 0x0 solutions/py_engine_esp32_demo/firmware.bin

# 可添加参数，手动设置串口：  --port 串口名称
```



### 错误处理：

暂无

