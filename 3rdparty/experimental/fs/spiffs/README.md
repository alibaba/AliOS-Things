## Contents

## Introduction
**spiffs** is a file system intended for SPI NOR flash devices on embedded targets.
It is designed with following characteristics in mind:
- small targets, sparse RAM without heap.
- Only big areas of data can be erased.
- An erase will reset all bits in block to ones.
- Writing pulls one to zeroes.
- Zeroes can only be pulled to one by erase.
- Wear leveling.

It also attempts to provide AOS-compatible filesystem behaviour.

### Features
- Specifically designed for low ram usage
- Use statically sized ram buffers, independent of number of files
- Posix-like api: open, close, read, write, seek, stat, etc
- It can run on any NOR flash, not only SPI flash - theoretically also on embedded flash of a micro-processor
- Multiple spiffs configuations can run on same target and even on same SPI flash device
- Implements static wear leveling
- Built in file system consistency checks
- Highly configurable

### Directories

```sh
spiffs
├── include
│   └── spiffs_config.h      # SPIFFS AOS configuration
├── spiffs
│   ├── default
│   │   └── spiffs_config.h  # SPIFFS default configuration
│   ├── include
│   │   ├── spiffs.h         # SPIFFS api declaration
│   │   └── spiffs_nucleus.h # SPIFFS internal structure defination
│   ├── spiffs_cache.c       # SPIFFS cache implementation
│   ├── spiffs_check.c       # SPIFFS filesyetm check implementation
│   ├── spiffs_gc.c          # SPIFFS garbage collection implementation
│   ├── spiffs_hydrogen.c    # SPIFFS api implementation
│   └── spiffs_nucleus.c     # SPIFFS inode/entry implementation
└── spiffs_port.c            # SPIFFS VFS adapt layer implementation
```

## Reference
