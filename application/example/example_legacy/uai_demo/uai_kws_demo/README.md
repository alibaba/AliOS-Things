## Overview
* Keyword spotting example based on source code ML-KWS-for-MCU
  https://github.com/ARM-software/ML-KWS-for-MCU

* this example support 10 words spotting: "yes", "no", "up", "down", "left", "right", "on", "off", "stop", "go".

* dnn_weights.c is generated from ML-KWS-for-MCU code weights.h.
    * You can train your own model, and use you own model data.
    * You can change it to a binary file, and put it in file system.

* wav_data.c is generated from wav file, audio sample is 16000, time length is 1 sencond. You can change the code ``` int16_t wav_data[] = YES_WAV_DATA; ``` to
``` int16_t wav_data[] = NO_WAV_DATA; ```, or to ``` int16_t wav_data[] = STOP_WAV_DATA; ``` to test other words. You also can generate this audio data from you own audio file.

* mfcc.c is generated from ML-KWS-for-MCU code mfcc.cpp

* DNN configuration is ```layer_info``` in uai_kws_demo.c, you can change it by your own model.

## Introduction

The **uai_cifar10_demo**  shows uAI CNN model functions.

### Dependencies

* uAI
* CMSIS NN

### Supported Boards

* mk3080
* xr871evb

 *<u>In theory, this example can run on all ARM Cortex M platforms. However, some boards may cause compilation errors due to incompatible compilation options or CMSIS library versions not match. You need to modify the corresponding compilation options or upgrade the CMSIS library version.</u>*

### Build

```sh
# generate uai_demo.uai_kws_demo@xr871evb default config
aos make uai_demo.uai_kws_demo@xr871evb -c config

# or customize config manually
aos make menuconfig

# build
aos make
```






