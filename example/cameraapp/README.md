## cameraapp

### Content
- cameraapp
    - [Content](#content)
    - [Overview](#overview)
    - [Requirements](#requirements)
    - [Build](#build)
    - [Run](#run)

### Overview

* An easy sample for developerkit , you can use it to take pictures . 
* According to the example , you can know how to use the board quickly ,
also it will help you to begin **AliOS Things** .

### Requirements

* Prepare a **SD card** not larger than 32GB , which needs to be inserted to the SD card slot before you run the image .

### Build

* Build and flash cameraapp image for developerkit, e.g.:  

```sh
aos make cameraapp@developerkit
aos upload cameraapp@developerkit
```

### Run

* Once cameraapp ready , the LCD screen will display a preview of the camera.

* Then press the key B , it will take a photo . 
  The LED2 at top right corner of the board will be on for two seconds , which means the photo has be stored .

* The photos will be stored in DCMI content of the SD card .

![](https://i.imgur.com/UnDyqQN.png)

