## Contents

## Introduction
**cplusplus** cpp functions support in kernel.
If you want to use c++ in Alios-Things, you should follow these four steps:
1. call cpp_init() in system init.
2. add c++ compiler flags.
for example, if you want to use float in cortex m4, you should add "GLOBAL_CXXFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16" in makefile.
3. add #include <alios_cpp.h> in the file you want to use c++.

### Features
- cplusplus feature

### Directories

```sh
cplusplus
├── include
│   ├── cpp_mem.h          # cpp interface of mem
│   ├── mutex.h            # cpp interface of mutex
│   ├── queue.h            # cpp interface of queue
│   ├── semaphore.h        # cpp interface of semaphore
│   ├── thread.h           # cpp interface of thread
│   ├── timer.h            # cpp interface of timer
│   └── workQueue.h        # cpp interface of workqueue
├── cplusplus.mk                 # makefile of cplusplus
├── alios_cpp.h                  # global include head file for app
├── cpp_init.c                   # cplusplus init functions
├── cpp_mem.c                    # implement of mem for cpp
├── mutex.c                      # implement of mutex for cpp
├── queue.c                      # implement of queue for cpp
├── semaphore.c                  # implement of semaphore for cpp
├── thread.h                     # implement of thread for cpp
├── timer.c                      # implement of timer for cpp
├── workQueue.c                  # implement of workQueue for cpp
```
### Dependencies

## Reference