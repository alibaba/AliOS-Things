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

### How to use POSIX
Add dependencies on posix like below, and then use it just following the POSIX.1-2017 standard. 
```
$(NAME)_COMPONENTS  += osal_posix
```

### Dependencies

## Reference
- http://pubs.opengroup.org/
