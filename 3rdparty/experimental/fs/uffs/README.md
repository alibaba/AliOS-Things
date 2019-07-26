## Contents

## Introduction
**uffs** is a nand flash file system designed for embedded system. It also attempts to provide AOS-compatible filesystem behaviour.

### Features
- Low cost
- Fast booting
- Superb Reliability
- Fast file create/read/write/seek
- Bad-block tolerant, ECC enable and good wear-leveling
- There is no garbage collection needed for UFFS
- Support multiple NAND flash class in one system
- Support bare flash hardware
- Support static memory allocation
- Fully simulated on PC platform

### Directories

```sh
uffs
├── include
│   ├── aos_uffs.h  # UFFS register interface declaration
│   └── uffs_port.h # UFFS configuration adapt layer
├── uffs_aos.c      # UFFS OS adapt layer implementation
└── uffs_port.c     # UFFS VFS adapt layer implementation
```

### Dependencies

### Notes

- Building:
    - get uffs zipball from official repo.(https://github.com/rickyzheng/uffs)
    - Put the souce files under this directory
    - Add component dependency to your target board or your target app.(`$(NAME)_COMPONENTS += 3rdparty.experimental.fs.uffs`)
    - Move the aos\_uffs.h to include directory (which is under root directory) 
    - Just build and enjoy it ! 

- License:
This software component is used to help users port third-party programs, but WITHOUT ANY WARRANTY. The entire risk as to the quality and performance of the program is with you. Should the program prove defective, you assume the cost of all necessary servicing, repair or correction. The use of third-party programs must also follow its own permissive license.

## Reference
