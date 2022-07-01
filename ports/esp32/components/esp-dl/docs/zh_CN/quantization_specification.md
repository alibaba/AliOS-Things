# 量化规范 [[English]](../en/quantization_specification.md)

[训练后量化](https://www.tensorflow.org/lite/performance/post_training_quantization)将浮点模型转换为定点模型。这种转换技术可以缩减模型大小，降低 CPU 和硬件加速器延迟，同时不会降低准确度。

如 ESP32-S3 芯片，存储空间相对有限，在 240 MHz 的情况下每秒乘加累计运算次数 (MACs) 仅达 75 亿次。在这样的芯片上必须要用量化后的模型做推理。您可使用我们提供的[量化工具包](../../tools/quantization_tool/README_cn.md)量化浮点模型，或根据 [convert.py 使用说明](../../tools/convert_tool/README_cn.md)中的步骤部署定点模型。

## 全整数量化

模型中的所有数据都需要量化为 8 位或 16 位整数。所有数据包括
  - 常量：权重、偏差和激活函数
  - 变量：张量，如中间层（激活函数）的输入和输出

8 位或 16 位量化使用以下公式近似表示浮点值：

```math
real\_value = int\_value * 2^{\ exponent}
```

### 有符号整数

8 位量化的 `int_value` 代表一个 **int8** 的有符号数, 其范围是 [-128, 127]。16 位量化的 `int_value` 代表一个 **int16** 的有符号数, 其范围是 [-32768, 32767]。

### 对称

所有量化后的数据都是**对称**的，也就是说没有零点（偏差），因此可以节省将零点与其他值相乘的运算时间。

### 粒度

**按张量（又名按层）量化**的意思是每个完整张量只有一个指数位，该张量内的所有值都按照该指数位量化。

**按通道量化**的意思是卷积核的每个通道对应着不同的指数位。

与按张量量化相比，按通道量化通常对于部分模型来说精确度会更高，但也会更耗时间。您可使用[量化工具包](../../tools/quantization_tool/README_cn.md)中的*评估器*模拟在芯片上进行量化推理的性能，之后再决定使用哪种量化形式。

16 位量化为确保更快的运算速度，目前仅支持按张量量化。8 位量化支持按张量和按通道两种形式，可让您在性能和速度之间折中。


## 量化算子规范

以下是 API 的量化要求：
```
Add2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

AvgPool2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

Concat
  Input ...:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Inputs and output must have the same exponent

Conv2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Weight):
    data_type  : int8 / int16
    range      : [-127, 127] / [-32767, 32767]
    granularity: {per-channel / per-tensor for int8} / {per-tensor for int16} 
  Input 2 (Bias):
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
    restriction: exponent = output_exponent
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

DepthwiseConv2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Weight):
    data_type  : int8 / int16
    range      : [-127, 127] / [-32767, 32767]
    granularity: {per-channel / per-tensor for int8} / {per-tensor for int16} 
  Input 2 (Bias):
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
    restriction: exponent = output_exponent
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

ExpandDims
 Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Flatten
 Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

FullyConnected
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Weight):
    data_type  : int8 / int16
    range      : [-127, 127] / [-32767, 32767]
    granularity: {per-channel / per-tensor for int8} / {per-tensor for int16} 
  Input 2 (Bias):
    data_type  : int8 / int16
    range      : {[-32768, 32767] for int8 per-channel / [-128, 127] for int8 per-tensor} / {[-32768, 32767] for int16}
    granularity: {per-channel / per-tensor for int8} / {per-tensor for int16} 
    restriction: {exponent = input_exponent + weight_exponent + 4 for per-channel / exponent = output_exponent for per-tensor}
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

GlobalAveragePool2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

GlobalMaxPool2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

LeakyReLU
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Alpha):
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Max2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
  
MaxPool2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Min2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
  
Mul2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

PReLU
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1 (Alpha):
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
  
ReLU
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Reshape
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent

Squeeze
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
  
Sub2D
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Input 1:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor

Transpose
  Input 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  Output 0:
    data_type  : int8 / int16
    range      : [-128, 127] / [-32768, 32767]
    granularity: per-tensor
  restriction: Input and output must have the same exponent
```