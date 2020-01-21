## cameraapp

### Content
- cameraapp
    - [Content](#content)
    - [Overview](#overview)
    - [Requirements](#requirements)
    - [Build](#build)
    - [Run](#run)

### Overview

* A simple camera demo for Developer Kit board, you will know how to take photoes which will be stored in SD card locally once you press button, 
LCD also will show images which capturing by camera with real time. Of course, this demo is based on **AliOS Things**.

### Requirements

* Prepare a **TF card** not larger than 32GB , which needs to be inserted to the SD card slot as below gif image show.    
![](https://img.alicdn.com/tfs/TB1dLMvhVzqK1RjSZSgXXcpAVXa-240-240.gif)

> if there is no SD card, no any images will be stored, but LCD still show these images which captured by camera 

### Supported Boards

- developerkit

### Build

* Build and flash cameraapp image for developerkit, e.g.:  

```sh
# generate dk.dk_camera@developerkit default config
aos make dk.dk_camera@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Run

* Once cameraapp is ready , the LCD screen will display a preview from the camera.

* Then press button B , it will take a photo, meanwhile, LED2 will turn on/off if images are stored one by one in DCMI content of the SD card if it is ready .

![](https://img.alicdn.com/tfs/TB1j4A1j3HqK1RjSZFkXXX.WFXa-649-775.png)

```
[1306670]<V> Button B pressed, begin to store image

[1307070]<V> image_14.bmp saved ok

[1308080]<V> Image stored.


[1325240]<V> Button B pressed, begin to store image

[1325610]<V> image_15.bmp saved ok

[1326620]<V> Image stored.
```