# Cat Face Detection [[中文]](./README_cn.md)

This project is an example of cat face detection interface. The input to this interface is a static image. The detection results are confidence scores and coordinate values shown in Terminal, which can be converted by a tool into an image shown on your PC screen.

Below is the structure of this project:

```shell
cat_face_detect/
├── CMakeLists.txt
├── image.jpg
├── main
│   ├── app_main.cpp
│   ├── CMakeLists.txt
│   └── image.hpp
├── README.md
├── README_cn.md
└── result.png
```



## Run the Example

1. Open Terminal and go to esp-dl/examples/cat_face_detect, the directory where this project is stored:

    ```shell
    cd ~/esp-dl/examples/cat_face_detect
    ```

2. Set SoC target:

    ```shell
    idf.py set-target [SoC]
    ```
    Replace [SoC] with your target, such as esp32, esp32s2, and esp32s3.

3. Flash the program and launch IDF monitor to obtain the fractional and coordinate values of detection results:

   ```shell
   idf.py flash monitor
   
   ... ...
   
   [0] score: 1.709961, box: [122, 2, 256, 117]
   ```

4. The tool `display_image.py` stored in [examples/tool/](../tool/) allows you to directly view the image of detection results. According to instructions on [Tools](../tool/README.md), run the following command:

   ```shell
   python display_image.py -i ../cat_face_detect/image.jpg -b "(122, 2, 256, 117)"
   ```
   The image of detection results will show on your PC screen as follows:
   
   <p align="center">
    <img width="%" src="./result.png"> 
   </p>


## Customize Input Image

In this example project, [./main/image.hpp](./main/image.hpp) is the default input image. You can use the script `convert_to_u8.py` following instructions on [Tools](../tool/README.md), to convert your own image into C/C++ code in replace of the default input image.

1. Save your image to directory ./examples/cat_face_detect, and use [examples/tool/convert_to_u8.py](../tool/convert_to_u8.py) to convert the image into an hpp file:

   ```shell
   # Assume you are in cat_face_detect directory

   python ../tool/convert_to_u8.py -i ./image.jpg -o ./main/image.hpp
   ```

2. According to steps in Section [Run the Example](#run-the-example), flash the firmware, print the confidence scores and coordinate values of detection results, and view the image of detection results.



## Latency

|   SoC    |    Latency |
| :------: | ---------: |
|  ESP32   | 149,765 us |
| ESP32-S2 | 416,590 us |
| ESP32-S3 |  18,909 us |

> Results above are based on the default configuration of this example.

