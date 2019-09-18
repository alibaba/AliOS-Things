## Overview
* cifar10 example based on source code CMSIS NN example
  https://github.com/ARM-software/CMSIS_5/tree/develop/CMSIS/NN/Examples/ARM/arm_nn_examples/cifar10

* this example support 10 classes of images: "airplane", "automobile", "bird", "cat", "deer", "dog", "frog", "horse", "ship", "truck".

* cifar10_weights.h is generated from CMSIS NN example code arm_nnexamples_cifar10_weights.h and arm_nnexamples_cifar10_parameter.h.
    * You can train your own model, and use you own model data.
    * You can change it to a binary file, and put it in file system.

* img_data.c is generated from cifar10 dataset. [http://www.cs.toronto.edu/~kriz/cifar.html]
    * You also can generate this data by you own image file(size 32*32, RGB format).

* CNN configuration is ```layer_info``` in uai_cifar10_demo.c, you can change it by your own model.



