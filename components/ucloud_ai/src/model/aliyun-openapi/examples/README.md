# how to use examples

## 1. Install SDK according to [README](https://github.com/aliyun/aliyun-openapi-cpp-sdk/blob/master/README.md)

## 2. build examples

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

- Generate Visual Studio solution with the same way as buidl SDK.

  - Run cmake-ui

  - select source code path(examples dir) and build path (create build dir in examples dir).

  - configure

  - generate

- Open `alibabacloud-sdk-examples.sln` in build directory.

- Build -> Build Solution


## 3. run examples

### Linux

- configure `accessKeyId` and `accessKeySecret` via env and then Run.

```bash
export ENV_AccessKeyId="your-accessKeyId"
export ENV_AccessKeySecret="your-accessKeySecret"

cd path/to/aliyun-openapi-cpp-sdk
cd examples/build/bin

# run ecs_demo
./ecs_demo
...
```

### Windows

Configure env according to your windows OS.

Run .exe file in command window in directory build\\bin\\Release
