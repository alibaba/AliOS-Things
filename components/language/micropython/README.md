## Contents

## Introduction
**micropython** is a lean and efficient implementation of the Python 3 programming language that includes a small subset of the Python standard library and is optimised to run on microcontrollers and in constrained environments.

### Features
- The basic resource costs 110K for ROM, and 8.6K for RAM.
- Basic futures can supports all boards whose hardware ROM and RAM resource can meet the needs.
- "mpy_main" is the entry function for micropython. Calling mpy_main with no arguments will start basic cmds service. If you want to run a specific py script, argument argv is needed. And it should be used with filesystem to recognize the py file.
- This future supports basic functions such as print、arithmetical and logic process、file and directory access and so on. Other supported futures depend on the realization of posix interface, which not support functions like pthread、signal and so on.

### Directories

```sh
micropython
├── drivers               # original micropython dir for driver
├── extmod                # original micropython dir
├── py                    # original micropython dir
├── lib                   # original micropython dir
├── include               # micropython head file
├── port
│   ├── mphalport.c       # micropython hal port
│   ├── mphalport.h       # micropython hal port
│   ├── mpy_main.c        # micropython main entry
│   ├── others
├── micropython.mk

```

### Dependencies

## Reference
- https://micropython.org/
