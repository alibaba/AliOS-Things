@page cjson cjson

[更正文档](https://gitee.com/alios-things/cjson/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
cJSON是符合ANSI C标准的极轻量级JSON解析器。

## 版权说明
> The MIT License

## 目录结构
```
├── src
│   ├── cJSON.c         # 源文件（必需）
│   └── cJPath.c        # 源文件（必需）
├── include
│   └── cJSON.h         # 包含cJSON API
├── package.yaml        # 编译配置文件
└── example
    └── cjson_example.c # 示例代码（默认禁用，可在package.yaml中使能）
```

## 依赖组件
* osal_aos

# 常用配置
无

# API说明

## 将字符串解析为JSON结构体
```c
cJSON *cJSON_Parse(const char *value);
```
|args                                    |description|
|:-----                                  |:----|
|value                                   |JSON字符串|

## 删除JSON结构体
```c
void cJSON_Delete(cJSON *c);
```
|args                                    |description|
|:-----                                  |:----|
|value                                   |待删除的JSON结构体|

## 获取JSON数组包含成员个数
```c
int cJSON_GetArraySize(const cJSON *array);
```
|args                                    |description|
|:-----                                  |:----|
|value                                   |数组类型的JSON结构体|

## 获取JSON数组成员
```c
cJSON *cJSON_GetArrayItem(const cJSON *array, int index);
```
|args                                    |description|
|:-----                                  |:----|
|array                                   |数组类型的JSON结构体|
|index                                   |数组成员索引|

## 获取JSON对象成员
```c
cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON * const object, const char * const string);
```
|args                                    |description|
|:-----                                  |:----|
|object                                  |对象类型的JSON结构体|
|string                                  |成员名称（大小写敏感）|

## 获取出错信息
```c
const char *cJSON_GetErrorPtr(void);
```

## 判断JSON结构体是否为布尔类型
```c
cJSON_bool cJSON_IsBool(const cJSON * const item);
```
|args                                    |description|
|:-----                                  |:----|
|item                                    |待判断的JSON结构体|

## 判断JSON结构体是否为数值类型
```c
cJSON_bool cJSON_IsNumber(const cJSON * const item);
```
|args                                    |description|
|:-----                                  |:----|
|item                                    |待判断的JSON结构体|

## 判断JSON结构体是否为字符串类型
```c
cJSON_bool cJSON_IsString(const cJSON * const item);
```
|args                                    |description|
|:-----                                  |:----|
|item                                    |待判断的JSON结构体|

## 判断JSON结构体是否为数组类型
```c
cJSON_bool cJSON_IsArray(const cJSON * const item);
```
|args                                    |description|
|:-----                                  |:----|
|item                                    |待判断的JSON结构体|

## 判断JSON结构体是否为对象类型
```c
cJSON_bool cJSON_IsObject(const cJSON * const item);
```
|args                                    |description|
|:-----                                  |:----|
|item                                    |待判断的JSON结构体|

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：
```yaml
depends:
  - cjson: master
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install cjson

```

上述命令执行成功后，组件源码则被下载到了./components/cjson路径中。

## 步骤4 添加示例

在cjson组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/cjson/tree/master/example)：
```
source_file:
  - "src/*.c"
  - "example/cjson_example.c"
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

CLI命令行输入：
```
cjson_example
```

**关键日志**
CLI日志：
```
Checking monitor "Awesome 4K"
1280 x 720.00
1920 x 1080.00
3840 x 2160.00
```

# 注意事项
无

# FAQ
无
