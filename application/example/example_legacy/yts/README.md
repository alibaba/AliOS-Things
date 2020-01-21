## Contents

```
yts
└── main.c
```

## Introduction

**yts**(yunit test suit) is an test suit for AliOS Things certification service, We offer AliOS Things certification service to ensure the steady operation of chips after porting.

Please read [AliOS Things certification process](https://github.com/alibaba/AliOS-Things/wiki/Certification-Process.zh) and [AliOS Things self test guide](https://github.com/alibaba/AliOS-Things/wiki/Manual) to know the general test process and test items.

### Features

* AliOS Things Kernel test
* AliOS Things channel test
* AliOS Things uMesh test
* AliOS Things network test
* AliOS Things FOTA test
* AliOS Things security test
* AliOS Things uData test

### Dependencies

* testcase
* rhino.test
* log
* kernel.fs.vfs
* yloop
* hal
* network.lwip netmgr
* 3rdparty.experimental.fs.fatfs

### Supported Boards

- all

### Build & Flash

```sh
# generate yts@developerkit default config
aos make yts@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make

# upload
aos upload
```

### Result
when **yts** is running, on the serial console input commands `yts_run <component>`, for example:

```
# yts_run kernel
[3382620]<V> Starting customer yts test...

--------------------------------
0 test cases, 0 failed

--------------------------------
```

## Reference

* [AliOS-Things Certification Home](https://github.com/alibaba/AliOS-Things/wiki/Certification-Home)
* [AliOS Things self test guide](https://github.com/alibaba/AliOS-Things/wiki/Manual)