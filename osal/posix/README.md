## Contents

## Introduction
**POSIX** The Portable Operating System Interface (POSIX) is a family of standards specified by the IEEE Computer Society for maintaining compatibility between operating systems. POSIX defines the application programming interface (API), along with command line shells and utility interfaces, for software compatibility with variants of Unix and other operating systems.

### Features
- pthread
- semaphore
- mutex
- cond
- timer
- mutex
- device IO
- file system

### Directories

```sh
posix
├── aos.mk                       # makefile of posix
├── dirent.c                     # implement of device IO and file system
├── posix_init.c                 # implement of posix init
├── mqueue.c                     # implement of queue
├── prctl.h                      # implement of prctl
├── semaphore.c                  # implement of semaphore
├── timer.c                      # implement of timer
├── pthread.c                    # implement of pthread
├── pthread_attr.h               # implement of pthread attribute
├── pthread_cond.c               # implement of cond
├── pthread_mutex.c              # implement of mutex
├── pthread_sched.c              # implement of pthread sched
├── pthread_tsd.h                # implement of thread specific data
```

### Dependencies

## Reference
- http://pubs.opengroup.org/
