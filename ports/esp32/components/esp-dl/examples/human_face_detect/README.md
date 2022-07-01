# Human Face Detection [[中文]](./README_cn.md)

This project is an example of human face detection interface. The input to this interface is a static image. The detection results are confidence scores and coordinate values shown in Terminal, which can be converted by a tool into an image shown on your PC screen.

Below is the structure of this project:

```shell
human_face_detect/
├── CMakeLists.txt
├── image.jpg
├── main
│   ├── app_main.cpp
│   ├── CMakeLists.txt
│   └── image.hpp
├── partitions.csv
├── README.md
├── README_cn.md
└── result.png
```



## Run the Example

1. Open Terminal and go to esp-dl/examples/human_face_detect, the directory where this project is stored:

    ```shell
    cd ~/esp-dl/examples/human_face_detect
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
   
   [0] score: 0.987580, box: [137, 75, 246, 215]
       left eye: (157, 131), right eye: (158, 177)
       nose: (170, 163)
       mouth left: (199, 133), mouth right: (193, 180)
   ```

4. The tool `display_image.py` stored in [examples/tool/](../tool/) allows you to directly view the image of detection results. According to instructions on [Tools](../tool/README.md), run the following command:

   ```shell
   python display_image.py -i ../human_face_detect/image.jpg -b "(137, 75, 246, 215)" -k "(157, 131, 158, 177, 170, 163, 199, 133, 193, 180)"
   ```
    The image of detection results will show on your PC screen as follows:
   

   <p align="center">
    <img width="%" src="./result.png"> 
   </p>


## Other Configuration

At the beginning of [./main/app_main.cpp](./main/app_main.cpp), there is a macro definition called `TWO_STAGE` that defines target detection algorithms. As annotations suggest:

- `TWO_STAGE` = 1: two-stage detectors with higher accuracy (support for facial landmarks) but lower speed.
- `TWO_STAGE` = 0: one-stage detectors with relatively lower accuracy (no support for facial landmarks) but higher speed.

You can experience the differences of the two detectors.



## Customize Input Image

In this example project, [./main/image.hpp](./main/image.hpp) is the default input image. You can use the script `convert_to_u8.py` following instructions on [Tools](../tool/README.md), to convert your own image into C/C++ code in replace of the default input image.

1. Save your image to directory ./examples/human_face_detect , and use [examples/tool/convert_to_u8.py](../tool/convert_to_u8.py) to convert the image into an hpp file:

   ```shell
   # Assume you are in human_face_detect 

   python ../tool/convert_to_u8.py -i ./image.jpg -o ./main/image.hpp
   ```

2. According to steps in Section [Run the Example](#run-the-example), flash the firmware, print the confidence scores and coordinate values of detection results, and view the image of detection results.



## Latency

|   SoC    | `TWO_STAGE` = 1 | `TWO_STAGE` = 0 |
| :------: | --------------: | --------------: |
|  ESP32   |      415,246 us |      154,687 us |
| ESP32-S2 |    1,052,363 us |      309,159 us |
| ESP32-S3 |       56,303 us |       16,614 us |

> Results above are based on the default configuration of this example.
