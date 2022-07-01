# 人脸识别 [[English]](./README.md)

本项目为人脸识别接口的示例。人脸识别接口的输入图片为一张带有人脸的静态图片，输出是录入人脸、识别人脸、删除人脸等接口功能的运行结果，显示在终端中。

该接口提供了 16 位量化与 8 位量化两个版本的模型。16 位量化的模型相比于 8 位量化的模型，精度更高，但是占用内存更多，运行速度也更慢。您可以根据实际使用场景挑选合适的模型。

项目所在文件夹结构如下：

```shell
face_recognition/
├── CMakeLists.txt
├── image.jpg
├── main
│   ├── app_main.cpp
│   ├── CMakeLists.txt
│   └── image.hpp
├── partitions.csv
└── README.md
└── README_cn.md
```



## 运行示例

1. 打开终端，进入人脸检测示例所在文件夹 esp-dl/examples/face_recognition

    ```shell
    cd ~/esp-dl/examples/face_recognition
    ```

2. 设定目标芯片：

    ```shell
    idf.py set-target [SoC]
    ```
    将 [SoC] 替换为您的目标芯片，如 esp32、esp32s2、esp32s3。
    
    由于 ESP32-S3 芯片在 AI 应用上的运行速度远快于其他芯片，我们更推荐您使用 ESP32-S3 芯片。

3. 烧录程序，运行 IDF 监视器获取各功能的运行结果：

   ```shell
   idf.py flash monitor
   
   ... ...
   
   E (1907) MFN: Flash is empty
   
   enroll id ...
   name: Sandra, id: 1
   name: Jiong, id: 2
   
   recognize face ...
   [recognition result] id: 1, name: Sandra, similarity: 0.728666
   [recognition result] id: 2, name: Jiong, similarity: 0.827225
   
   recognizer information ...
   recognizer threshold: 0.55
   input shape: 112, 112, 3
   
   face id information ...
   number of enrolled ids: 2
   id: 1, name: Sandra
   id: 2, name: Jiong
   
   delete id ...
   number of remaining ids: 1
   [recognition result] id: -1, name: unknown, similarity: 0.124767
   
   enroll id ...
   name: Jiong, id: 2
   write 2 ids to flash.
   
   recognize face ...
   [recognition result] id: 1, name: Sandra, similarity: 0.758815
   [recognition result] id: 2, name: Jiong, similarity: 0.722041
   
   ```

## 其他设置

1. [./main/app_main.cpp](./main/app_main.cpp) 开头处的宏定义 `QUANT_TYPE`，可定义模型的量化类型。

    - `QUANT_TYPE` = 0：使用 8 位量化模型，识别精度低于 16 位模型，但速度更快，内存占用更少。
    - `QUANT_TYPE` = 1：使用 16 位量化模型，识别精度与浮点模型一致。

    您可根据实际使用场景挑选合适的模型。


2. [./main/app_main.cpp](./main/app_main.cpp) 开头处的宏定义 `USE_FACE_DETECTOR`，可定义人脸关键点 (landmark) 坐标的获得方式。

    - `USE_FACE_DETECTOR` = 0：使用存放在 ./image.hpp 中的关键点坐标。
    - `USE_FACE_DETECTOR` = 1：使用人脸检测模型获得关键点坐标。

   请注意关键点坐标顺序为：
   
   ```
    left_eye_x, left_eye_y, 
    mouth_left_x, mouth_left_y,
    nose_x, nose_y,
    right_eye_x, right_eye_y, 
    mouth_right_x, mouth_right_y
   ```

## 延时情况

|   SoC    |      8 位 |    16 位 |
| :------: | --------: | -------: |
|  ESP32   | 13,301 ms | 5,041 ms |
| ESP32-S3 |    287 ms |   554 ms |