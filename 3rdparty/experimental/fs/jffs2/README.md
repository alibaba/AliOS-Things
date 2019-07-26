## Contents

## Introduction
**jffs2** provides a log-stuctured filesystem directly on the flash. rather than emulating a block device. It also attempts to provide AOS-compatible filesystem behaviour.

### Features
- Support for NOR/Nand flash device
- Hard links
- Compression
- Better performance

### Directories

```sh
jffs2
├── aos_jffs2.h # JFFS2 register interface declaration
└── vfs_jffs2.c # JFFS2 vfs adapter layer
```

### Dependencies

### Notes

- Usage:
    - This directory include all the jffs2 vfs-adapting files.
    - You should call `vfs_jffs2_register()` function to mount the jffs2 before you can use it.
    - The mount point of the jffs2 in AliOS-Things is `/jffs2`.

- Building:
    - Get the AliOS-Things porting version jffs2 codes first. (you can contact us via https://github.com/alibaba/AliOS-Things)
    - Put the source files under this directory
    - Add component dependency to your target board or your target app. (e.g. `$(NAME)_COMPONENTS += 3rdparty.experimental.fs.jffs2`)
    - Move the aos\_jffs2.h to include directory (which is under root directory) 
    - Just build and enjoy it !

- License:
This software component is used to help users port third-party programs, but WITHOUT ANY WARRANTY. The entire risk as to the quality and performance of the program is with you. Should the program prove defective, you assume the cost of all necessary servicing, repair or correction. The use of third-party programs must also follow its own permissive license.

## Reference
