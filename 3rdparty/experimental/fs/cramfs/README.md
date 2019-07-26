## Contents

## Introduction
**cramfs** is a compressed ROM filesystem. It uses the zlib routines to compress a file one page at a time, and allow random page access. It also attempts to provide AOS-compatible filesystem behaviour.

### Features
- support file which size is less than 16MB
- Max filesystem size is over 256MB

### Directories

```sh
cramfs
├── aos_cramfs.h # Cramfs register api declaration
└── vfs_cramfs.c # Cramfs vfs adapt layer implementation
```

### Dependencies

### Notes

- Usage:
    - This directory include all the cramfs vfs-adapting files.
    - You should call "vfs\_cramfs\_register" function to mount the cramfs before you can use it.
    - The mount point of the cramfs in AliOS-Things is "/cramfs".

- Building:
    - Get the AliOS-Things porting version cramfs codes first. (you can contact us via https://github.com/alibaba/AliOS-Things)
    - Put the source files under this directory
    - Add component dependency to your target board or your target app. (e.g. `$(NAME)_COMPONENTS += 3rdparty.experimental.fs.cramfs`)
    - Move the aos\_cramfs.h to include directory (which is under root directory)
    - Just build and enjoy it !

- License:
This software component is used to help users port third-party programs, but WITHOUT ANY WARRANTY. The entire risk as to the quality and performance of the program is with you. Should the program prove defective, you assume the cost of all necessary servicing, repair or correction. The use of third-party programs must also follow its own permissive license.

## Reference
