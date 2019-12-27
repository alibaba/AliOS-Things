## Contents

## Introduction
**fatfs** is a generic FAT/exFAT filesystem module for small embedded system. The fatfs module is written in compliance with ANSI C (C89). It attempts to provide AOS-compatible filesystem behaviour.

### Features
- DOS/Windows compatible FAT/exFAT filesystem
- Very small footprint for program code and work area
- Various configuration options to support for:
    - Long file name in ANSI/OEM or Unicode.
    - exFAT filesystem.
    - Thread safe for RTOS.
    - Multiple volumes (physical drives and partitions)
    - Variable sector size.
    - Multiple code page including DBCS.
    - Read-only, optional API, I/O buffer and etc...

### Directories

```sh
fatfs
├── diskio.c           # Disk I/O adapt layer
├── fatfs.c            # Fatfs VFS adapter layer
├── ff
│   ├── ff.c           # Fatfs core implementation
│   ├── ffunicode.c    # Fatfs unicode support
│   └── include
│       ├── diskio.h   # Disk I/O layer declaration
│       ├── ffconf.h   # Fatfs Configuration options
│       ├── ff.h       # Fatfs internal structure defination
│       └── integer.h  # Fatfs variable types defination
├── include
│   ├── fatfs_diskio.h # Fatfs disk I/O constant defination
└─── README.md

```

## Reference

