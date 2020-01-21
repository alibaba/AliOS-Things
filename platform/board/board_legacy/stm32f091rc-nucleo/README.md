## Overview
This is a board demo for consulting, not a true realization.
## Feature of Board

## Directories
```sh
stm32f091rc-nucleo # configuration files for board stm32f091rc-nucleo
=============================================================================================================
Dir\File                                Description                                           Necessary for kernel run
=============================================================================================================
|-- drivers                  # board peripheral driver                                              N
|-- config
|   |-- board.h              # board config file, define for user, such as uart port num            Y
|   |-- k_config.c           # user's kernel hook and mm memory region define                       Y
|   |-- k_config.h           # kernel config file .h                                                Y
|   |-- partition_conf.c     # board flash config file                                              N
|-- startup
|   |-- board.c              # board_init implement                                                 Y
|   |-- startup.c            # main entry file                                                      Y
|   |-- startup_gcc.s        # board startup assember for gcc                                       Y
|   |-- startup_iar.s        # board startup assember for iar                                       Y
|   |-- startup_keil.s       # board startup assember for keil                                      Y
|-- stm32f091xc_flash.icf          # linkscript file for iar                                              Y
|-- STM32F091RCTx_FLASH.ld           # linkscript file for gcc                                              Y
|-- stm32f091rc-nucleo.sct          # linkscript file for sct                                              Y
|-- aos.mk                   # board makefile                                                       Y
|-- ucube.py                 # config for CI autorun app                                            N
```

## Board Hardware Resources

## Pin Mapping

## Driver Support

## Programming

## Debugging

## Update log

## Reference
