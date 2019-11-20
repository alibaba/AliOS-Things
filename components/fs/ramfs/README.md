## Contents

## Introduction
**ramfs** is a RAM file system. It attempts to provide AOS-compatible filesystem behaviour while at the same time being efficient in terms of time and space used.

### Features
- file open/read/write etc.
- directory open/read/write etc.

### Directories

```sh
ramfs
├── include
│   ├── ramfs_adapt.h # ramfs OS adapt layer declaration
│   └── ramfs_err.h   # ramfs errno defination
├── ramfs_adapt.c     # ramfs OS adapt layer implementation
├── ramfs.c           # ramfs core logic
├── ramfs_types.h     # ramfs internal data structure defination
├── ramfs_vfs.c       # ramfs VFS adapt layer implementation
```

### Dependencies

## Reference
