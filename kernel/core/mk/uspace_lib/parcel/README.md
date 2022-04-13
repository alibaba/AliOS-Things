## Contents

## Introduction
**Parcel** 为RPC提供序列化参数打包和解包方法，具体的封包协议采用Protocol Buffer协议

### Features
- 使用Protocol Buffer协议序列化和反序列化参数列表

### Directories

```sh
parcel
├── aos.mk
├── Config.in
├── include
│   └── parcel.h    # Parcel对外暴露的API和数据结构
├── parcel.c        # Parcel对外API的实现
├── parcel_decode.c # Protocol Buffer反序列化参数实现
├── parcel_encode.c # Protocol Buffer序列化参数实现
├── proto_buf.h     # Parcel内部使用的Protocol Buffer头文件

```

### Dependencies

## Reference
- [AliOS Things VFS Porting Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-VFS-Porting-Guide)
