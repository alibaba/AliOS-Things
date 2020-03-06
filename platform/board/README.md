## Contents

## Introduction
supported boards' code here. It includes board init related codes.
    * board init codes
    * kernel config codes
    * startup.c which include main function
    * ld\scatter file
    * peripheral drivers for this board

## Attentions
1、 Boards in platform/board/board_legacy can only support apps in example/example_legacy.
While other boards in platform/board support all examples list in application dir.
2、 Apps in example/example_legacy dir run start from API of "application_start" which jump from startup.c in board.
Other apps in example dir firstly run start from API of "aos_maintask" and than jump to "application_start".
3、 When compiling new board in platform/board and old app in example/example_legacy, component of "app_adapter" is added automatically. Please see codes in directory of application/app_adapter

The difference between new and legacy boads is in startup.c.
For legacy boad, startup.c include the realize of "aos_maintask".
For new format boad, startup.c jump to API of "aos_maintask" which realized in new format app.

### Features
- board supported

### Standand directories layout
```sh
|--board/board_legacy
|--board/*								: new format different from board_legacy
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