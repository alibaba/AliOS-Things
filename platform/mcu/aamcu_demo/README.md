## Contents

This is a mcu demo for consuling, not a true realization.
Directory structure:

```
Dir\File                          Description                       Necessary for kernel run
============================================================================================
|-- drivers                 # board peripheral driver                           Y
|-- hal                    # hal API layer, hal uart is necessary               Y
|-- aos.mk                 # mcu makefile                                       Y
|-- Config.in                # menuconfig component config                      Y
|-- ucube.py                # aos build system file(for scons)                  N
|-- README.md                                                                   Y
```

Attention: If there are several mcu types here, use independent sub mk file to put different config.

## Overview

## Resources

## Reference
