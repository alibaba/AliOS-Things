# BoneEngine @lite for AliOS Things #

BoneEngine for AOS, 是一套为AliOS Things优化定制的JavaScript引擎, 集成了各IoT模块，为AliOS Things及运行AliOS的设备提供一套JavaScript的编程及运行环境。

采用 BoneEngine for AOS来构建物联网应用，极大的降低开发门槛，让更多开发者得以进入到硬件开发领域中。



## 使用方法


详情请参考[BoneEngine@lite for aos 使用说明.md](/doc/BoneEngine@lite%20for%20aos%20%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E.md)


```
代码放置路径： aos.git/framework/bone_engine

BoneEngine例程编译方法：
在example目录中创建参考例程目的符号链接
cd aos.git
ln -s ../framework/bone_engine/examples example/boneengine_demo

编译方法
aos make boneengine_demo@linuxhost
aos make boneengine_demo@mk3060
aos make boneengine_demo@mk3060 VERBOSE=1
aos make boneengine_demo@mk3060 BUILD_TYPE=release
aos make boneengine_demo@mk3060 JTAG=jlink download
```




## 说明文档

请参考doc目录下相关文档
*   [BoneEngine@lite for aos 使用说明.md](/doc/BoneEngine@lite%20for%20aos%20%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E.md)
*   [javascript能力集介绍.md](/doc/javascript%E8%83%BD%E5%8A%9B%E9%9B%86%E4%BB%8B%E7%BB%8D.md)
*   [javascript支持范围.xlsx](/doc/javascript%E6%94%AF%E6%8C%81%E8%8C%83%E5%9B%B4.xlsx)
*   [alink对象说明.md](/doc/alink%E5%AF%B9%E8%B1%A1%E8%AF%B4%E6%98%8E.md)
*   [gpio对象说明.md](/doc/gpio%E5%AF%B9%E8%B1%A1%E8%AF%B4%E6%98%8E.md)
*   [process对象说明.md](/doc/process%E5%AF%B9%E8%B1%A1%E8%AF%B4%E6%98%8E.md)
*   [timers说明.md](/doc/timers%E8%AF%B4%E6%98%8E.md)
*   [uart对象说明.md](/doc/uart%E5%AF%B9%E8%B1%A1%E8%AF%B4%E6%98%8E.md)

## 重要参考资料

*   [tiny-js](https://github.com/gfwilliams/tiny-js "tiny-js")
*   [ECMAScript® Language Specification](https://www.ecma-international.org/ecma-262/5.1/)
*   [ECMAScript5.1中文版](http://yanhaijing.com/es5/#0)
*   《Compilers: Principles, Techniques, and Tools (2nd Edition)》 ISBN-13: 978-0321486813

## 后续计划

**将重构JSE代码并持续优化BoneBngine。 **


# License

  BoneEngine @lite for AliOS Things is released under the Apache 2.0 license.

    Copyright 2015-2017 Alibaba Group Holding Ltd.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at following link.

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.