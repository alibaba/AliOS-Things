
## Introduction

This file introduces how to run sound location and gesture recognition application on stm32f413h discovery board with AliOS Things inside.

### Features

- Sound location. 

	Detect 360 degrees sound location and indicate location on screen.
- Gesture detection.

	Control system according to gesture action. For example, switch display interface, turn on/off led bar...


### Directories
example/sensordemo: application entries for sound location.

board/stm32f413h-discovery/: audio and display drivers, sound location algorithm.

**Note**

sound location algorithm is not open on github because of copyright. You can use it by following steps.

1. Download library package named X-CUBE-MEMSMIC1 from ST website([https://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-expansion-packages/x-cube-memsmic1.html](https://www.st.com/content/st_com/en/products/embedded-software/mcus-embedded-software/stm32-embedded-software/stm32cube-expansion-packages/x-cube-memsmic1.html)). 

2. Copy library file en.X-CUBE-MEMSMIC1\STM32CubeExpansion_MEMSMIC1_V3.0.0\Middlewares\ST\STM32_AcousticSL_Library\Lib\libAcoustic_SL_CM4F_GCC_ot.a to AliOS-Things\board\stm32f413h-discovery\drv\audio\lib directory.

3. Change "sound_enable" to 1 in AliOS-Things\board\stm32f413h-discovery\stm32f413h-discovery.mk

4. Compile and flash the firmware. Sound location will work.


### Dependencies

- device.sensor(necessary)
- framework.GUI.littlevGL(necessary)
- board.stm32f413h-discovery.audio(necessary)


### Others
**Gesture function**

- Up  —— Turn on LED Bar
- Down  —— Turn off LED Bar
- Left —— Display previous interface
- Right —— Display next interface

**Display interfaces**

- Interface 1：Display sound location. The sound location algorithm starts after switched to the interface. And location is indicated through screen icon. We suggest to test at the distance of 30cm to micphone array to get optimum results.
- Interface 2：Display volue. Calculate micphone signal data and convert to volume to display.
- Interface 3：Display temperature and humidity.

## Reference

- Blog: [Gesture application on AliOS Things](https://yq.aliyun.com/articles/645862?spm=a2c4e.11155435.0.0.56993312y5JkKD)
- Video: [Gesture application on AliOS Things](http://v.youku.com/v_show/id_XMzg0MDAwODIxNg==.html?spm=a2c4e.11153940.blogcont645862.11.61bc6d9bX64sO9)
- Blog: [Sound location application on AliOS Things](https://yq.aliyun.com/articles/646182)
- Video: [Sound location application on AliOS Things](http://v.youku.com/v_show/id_XMzg0MDEzMzAwNA==.html?spm=a2c4e.11153940.blogcont646182.12.2831706bsmEffH)