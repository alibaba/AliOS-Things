## Overview
* cifar10 example based on source code CMSIS NN example
  https://github.com/ARM-software/CMSIS_5/tree/develop/CMSIS/NN/Examples/ARM/arm_nn_examples/cifar10

* this example support 10 classes of images: "airplane", "automobile", "bird", "cat", "deer", "dog", "frog", "horse", "ship", "truck".

* cifar10_weights.h is generated from CMSIS NN example code arm_nnexamples_cifar10_weights.h and arm_nnexamples_cifar10_parameter.h.
    * You can train your own model, and use you own model data.
    * You can change it to a binary file, and put it in file system.

* img_data.c is generated from cifar10 dataset ```cifar-10-batches-bin/test_batch.bin```. [http://www.cs.toronto.edu/~kriz/cifar.html]
    * You also can generate this data by you own image file (size 32*32, RGB format).

* CNN configuration is ```layer_info``` in uai_cifar10_demo.c, you can change it by your own model.

## Introduction

The **uai_cifar10_demo**  shows uAI CNN model functions.

### Dependencies

* uAI
* CMSIS NN

### Supported Boards

* mk3080
* xr871evb

  *<u>In theory, this example can run on all ARM Cortex M platforms. However, some boards may cause compilation errors due to incompatible compilation options or CMSIS library versions not match. You need to modify the corresponding compilation options or upgrade the CMSIS library version. </u>*

### Build

```sh
# generate uai_demo.uai_cifar10_demo@xr871evb default config
aos make uai_demo.uai_cifar10_demo@xr871evb -c config

# or customize config manually
aos make menuconfig

# build
aos make
```


