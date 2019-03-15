## Contents

## Introduction
supported boards' code here. It includes board init related codes.
    * board init codes
    * kernel config codes
    * startup.c which include main function
    * ld\scatter file
    * peripheral drivers for this board

### Features
- board supported

### Standand directories layout
```sh
|--board
    |--xxx
        |--config
        |   --board.h                   : board config file, define for user, such as uart port num             Y
        |   --k_config.c                : user's kernel hook and mm memory region define                        Y
        |   --k_config.h                : kernel config file .h                                                 Y
        |   --partition_conf.c          : board flash config file                                               N
        |--drivers                      : board peripheral driver                                               N
        |--startup
        |   --board.c                   : board config implement                                                Y
        |   --startup.c                 : main entry file                                                       Y
        |--xxx.ld                       : link script                                                           Y
        |--aos.mk                       : board makefile                                                        Y
        |--README
```

## Reference