## Contents

## Introduction
**uspace** is the framework of AliOS Things micro kernel. With this component, user application calls kernel servers by system call rather than function call, which makes it easier to divide application from kernel image. User application runs at unpirvileged mode while kernel runs at privileged mode, with the help of MMU or MPU, user application can only access its own program and data, exception will be araised if user application attemps to access kernel resources.

AliOS Things supports light weight process. When an application is loaded, a process is created, all the tasks belong to the process share the process resources, such as address space, global variables.

### Features
- create process and destroy process
- create user task and delete user task
- each user task has two stacks to get rid of leaking kernel information from stack after system call
- each process has its own heap
- process communication is availible by using of msg queue
- rhino kernel APIs and HAL APIs are all implemented as system calls
- user space timer satisfies timer tick requriment

### Directories
```sh
uspace
├── include
│   ├── u_api.h              # overall user application header file
│   ├── u_mm.h               # user heap operation header file
│   ├── u_proc_msg.h         # processes communication header file
│   ├── u_task.h             # user task/process header file
│   └── u_timer.h            # user space timer header file
├── syscall
│   ├── include
│   │   ├── syscall.h        # syscall function argument definition
│   │   └── syscall_no.h     # syscall number definition
│   ├── ksyscall
│   │   └── ksyscall.c       # the implementation of syscall function in kernel
│   └── usyscall
│       └── usyscall.c       # syscall API called by user application
├── u_mm.c                   # user heap operation functions
├── u_proc_msg.c             # process communication buffer queue
├── u_res_task.c             # user task stack mem collection
├── u_task.c                 # user task/process specific functions
└── u_timer.c                # user space timer running in application

```

### Dependencies
- ENABLE_USPACE             # tell makefile we are to compile files to support user space
- RHINO_CONFIG_USER_SPACE   # enable user space feature in the common kernel file, such as u_task.h

## Reference

