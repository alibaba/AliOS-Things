# Face Recognition [[中文]](./README_cn.md)

This project is an example of human face recognition interface. The input to this interface is a static image of a human face. The output is results of face ID enrollment, face recognition, face ID deletion, and other functions provided by this interface, shown in Terminal.

The interface provides two model versions: 16-bit quantization model and 8-bit quantization model. Compared with the 8-bit quantization model, the 16-bit quantization model has higher accuracy but larger memory footprint and longer latency. You can select the appropriate model according to your scenario.

Below is the structure of this project:

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



## Run the Example

1. Open Terminal and go to esp-dl/examples/face_recognition, the directory where this project is stored:

    ```shell
    cd ~/esp-dl/examples/face_recognition
    ```

2. Set SoC target:

    ```shell
    idf.py set-target [SoC]
    ```
    Replace [SoC] with your target, such as esp32, esp32s2, and esp32s3.

    We recommend using the ESP32-S3 chip, which runs much faster than other chips for AI applications.

3. Flash the program and launch IDF monitor to obtain the results of functions:

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

## Other Configuration

1. At the beginning of [./main/app_main.cpp](./main/app_main.cpp), there is a macro definition called `QUANT_TYPE` that defines the version of quantization models.

    - `QUANT_TYPE` = 0: Use the 8-bit quantization model, which has lower accuracy but smaller memory footprint and shorter latency.
    - `QUANT_TYPE` = 1: Use the 16-bit quantization model, which has the same recognition accuracy as the floating-point model. 

    You can select the appropriate model according to your scenario.


2. At the beginning of [./main/app_main.cpp](./main/app_main.cpp), there is another macro definition called `USE_FACE_DETECTOR` that defines the way to obtain face landmark coordinates.

    - `USE_FACE_DETECTOR` = 0: Use the face landmark coordinates stored in ./image.hpp.
    - `USE_FACE_DETECTOR` = 1: Obtain face landmark coordinates using our face detection model.

    Note that the order of face landmark coordinates is:
  
   ```
      left_eye_x, left_eye_y, 
      mouth_left_x, mouth_left_y,
      nose_x, nose_y,
      right_eye_x, right_eye_y, 
      mouth_right_x, mouth_right_y
   ```

## Latency

| SoC | 8-bit | 16-bit |
|:---:| ----:| ----:|
| ESP32 | 13,301 ms | 5,041 ms |
| ESP32-S3 | 287 ms | 554 ms |

  

  