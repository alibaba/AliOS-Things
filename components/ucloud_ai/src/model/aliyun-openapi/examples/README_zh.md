# 如何运行例程

## 1. 根据 [README](https://github.com/aliyun/aliyun-openapi-cpp-sdk/blob/master/README-CN.md) 安装 SDK

## 2. 构建例程

### Linux

```bash
cd path/to/aliyun-openapi-cpp-sdk
cd examples
mkdir build
cd build
cmake ..
make -j
```

### Windows

- 通过 CMake 构建 Visual Studio 解决方案

  - 运行 cmake-ui

  - 选在源代码目录(examples 目录)和构建目录(在 examples 创建 build 目录)。

  - 配置 (configure)

  - 生成 (generate)

- 用 Visual Studio 从 build 目录打开 `alibabacloud-sdk-examples.sln`

- 编译： 生成 -> 生成解决方案


## 3. 运行例程

### Linux

- 配置环境变量 `accessKeyId` and `accessKeySecret`

```bash
export ENV_AccessKeyId="your-accessKeyId"
export ENV_AccessKeySecret="your-accessKeySecret"
```

```bash
cd path/to/aliyun-openapi-cpp-sdk
cd examples/build/bin

# run ecs_demo
./ecs_demo
...
```

### Windows

根据您的 Windows 版本设置环境变量

- 在命令行窗口直接运行 build\\bin\\Releaes 目录下的可执行文件，
