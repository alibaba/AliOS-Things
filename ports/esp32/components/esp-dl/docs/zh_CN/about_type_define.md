# 变量与常量介绍 [[English]](../en/about_type_define.md)

ESP-DL 有以下变量和常量：

- **变量（值可以变化）**：[张量](../../include/typedef/dl_variable.hpp/#15)
- **常量（值固定不变）**：[过滤器](../../include/typedef/dl_constant.hpp/#33)、[偏差](../../include/typedef/dl_constant.hpp/#55)和[激活函数](../../include/typedef/dl_constant.hpp/#67)



## 张量

张量是矩阵向更高维度的泛化。也就是说，张量可以是：

- 0 维，表示为标量
- 1 维，表示为向量
- 2 维，表示为矩阵
- 难以想象的多维结构

维数和每个维度的大小即为张量的形状。ESP-DL 的主要数据结构就是张量。一个层的所有输入和输出均为张量。



### 二维操作中，张量的维度顺序

二维操作中，层的输入张量和输出张量均是三维。张量的维度顺序固定，按照[高度，宽度，通道]的顺序排序。

假设张量的形状是 [5, 3, 4]，则张量中的元素应如下排列：

   <p align="center">
    <img width="%" src="../../img/tensor_3d_sequence.drawio.svg"> 
   </p>




## 过滤器、偏差和激活函数

与张量不同，过滤器、偏差和激活函数无需填充。这三个`元素`的顺序是灵活的，可根据特定操作调整。

更多细节，可参考 [`dl_constant.hpp`](../../include/typedef/dl_constant.hpp) 或 API 文档。
