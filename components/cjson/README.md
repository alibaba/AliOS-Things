@page cjson cJSON

[更正文档](https://gitee.com/alios-things/cjson/edit/rel_3.3.0/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
示例代码参考example/cjson_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
cjson组件的package.yaml中添加example
```
source_file:
  - "src/*.c"
  - "example/cjson_example.c"
```

## 添加组件
helloworld_demo组件的package.yaml中添加
```
depends:
  - cjson: master
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make helloworld_demo@haas100 -c config
```

## 烧录固件
参考具体板子的快速开始文档。

## 示例测试
CLI命令行输入：
```
cjson_example
```

## 关键日志
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
