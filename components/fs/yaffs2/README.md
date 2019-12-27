## Contents

## Introduction
**yaffs2** is an open-source file system specifically designed to be fast, robust and suitable for embedded use with NAND and NOR flash.. It also attempts to provide AOS-compatible filesystem behaviour.

### Features
- Support for NOR/Nand flash device
- Hard links
- Compression
- Better performance

### Directories

```sh
yaffs2
├── yaffs_alios.c       # YAFFS2 OS adapt layer implementation
├── yaffs_install_drv.c # YAFFS2 flash drive layer implementation
└── yaffs_install_drv.h # YAFFS2 flash drive layer interface declaration
```

### Dependencies

### Notes

- Porting:
    - the porting steps please follow the guide (https://yq.aliyun.com/articles/418865)

- License:
This software component is used to help users port third-party programs, but WITHOUT ANY WARRANTY. The entire risk as to the quality and performance of the program is with you. Should the program prove defective, you assume the cost of all necessary servicing, repair or correction. The use of third-party programs must also follow its own permissive license.

## Reference
