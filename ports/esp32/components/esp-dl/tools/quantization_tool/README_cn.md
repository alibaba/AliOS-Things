# 量化工具包 [[English]](./README.md)

量化工具包能够帮助您量化模型，使用乐鑫芯片进行推理。该工具包以开源的 AI 模型格式 [ONNX](https://github.com/onnx/onnx) 运行。

该工具包包括三个独立的工具：

- [优化器](#优化器)，用于优化计算图
- [校准器](#校准器)，用于训练后量化，不需要重新训练
- [评估器](#评估器)，用于评估量化后模型的性能

本文档介绍了每个工具的规范。API 介绍请参阅[量化工具包 API](quantization_tool_api_cn.md)。

> 请确保使用工具包前，您已将模型转换为 ONNX 格式，相关资料请参考[资源](#资源)。


## 优化器

计算图优化器 [optimizer.py](optimizer.py) 可以通过移除多余节点、简化模型结构、模型融合等方式提高模型性能。该优化器基于 [ONNX 优化器](https://github.com/onnx/optimizer)的[优化传递 (pass)](https://github.com/onnx/optimizer/tree/master/onnxoptimizer/passes)，以及我们额外增加的传递。

在量化前开启计算图融合很重要，尤其是对批量归一化 (batch normalization) 的融合。因此我们推荐您在使用校准器和评估器之前，先用优化器优化模型。您可以用 [Netron](https://github.com/lutzroeder/netron) 查看模型结构。


**Python API 示例**

```cpp
// load your ONNX model from given path
model_proto = onnx.load('mnsit.onnx')

// fuse batch normalization layers and convolution layers, and fuse biases and convolution layers
model_proto = onnxoptimizer.optimize(model_proto, ['fuse_bn_into_conv', 'fuse_add_bias_into_conv'])

// set input batch size as dynamic
optimized_model = convert_model_batch_to_dynamic(model_proto)

// save optimized model to given path
optimized_model_path = 'mnist_optimized.onnx'
onnx.save(new_model, optimized_model_path)
```

## 校准器

校准器可量化浮点模型，使之符合在乐鑫芯片上进行推理的要求。有关校准器支持的量化形式，请查看[量化规范](../../docs/zh_CN/quantization_specification.md)。

要将一个 32 位浮点 (FP32) 模型转换为一个 8 位整数 (int8) 或 16 位整数 (int16) 模型，工作流程如下：
- 准备 FP32 模型
- 准备校准数据集
- 配置量化
- 获取量化参数

### FP32 模型

准备的 FP32 模型必须与 ESP-DL 库适配。如果模型中有库中不支持的操作，校准器会不接受并生成错误信息。

模型的适配性可在获取量化参数时检查，也可提前通过调用 *check_model* 检查。

模型的输入应当是一个归一化后的数据。若您的归一化过程包含在模型的计算图中，为保证量化性能，请删除图中的相关节点并提前做好归一化。

### 校准数据集

选择适当的校准数据集对量化来说很重要。一个好的校准数据集应具有代表性。您可尝试不同的校准数据集，比较使用不同参数量化后的模型性能。

### 量化配置

校准器支持 int8 和 int16 量化。int8 和 int16 各自的配置如下：

int8：
- granularity：'per-tensor'、'per-channel'
- calibration_method：'entropy'、'minmax'

int16：
- granularity：'per-tensor'
- calibration_method：'minmax'

### 量化参数

如[量化规范](../../docs/zh_CN/quantization_specification.md)所述，ESP-DL 中 8 位或 16 位量化使用以下公式近似表示浮点值：

```math
real\_value = int\_value * 2^{\ exponent}
```

其中 2^exponent 为尺度。

返回的量化表列出的是模型中所有数据的量化尺度，这些数据包括：
  - 常量：权重、偏差和激活函数；
  - 变量：张量，如中间层（激活函数）的输入和输出。


**Python API 示例**

```cpp
// load your ONNX model from given path
model_proto = onnx.load(optimized_model_path)

// initialize an calibrator to quantize the optimized MNIST model to an int8 model per channel using entropy method
calib = Calibrator('int8', 'per-channel', 'entropy')

// set ONNX Runtime execution provider to CPU
calib.set_providers(['CPUExecutionProvider'])

// use calib_dataset as the calibration dataset, and save quantization parameters to the pickle file
pickle_file_path = 'mnist_calib.pickle'
calib.generate_quantization_table(model_proto, calib_dataset, pickle_file_path)
```

## 评估器

评估器用于模拟乐鑫芯片的量化解决方案，帮助评估量化后模型的性能。

如果模型中有不支持的操作，校准器会不接受并生成错误信息。

如果量化后模型的性能无法满足需求，可考虑量化感知训练。


**Python API 示例**

```cpp
// initialize an evaluator to generate an MNIST using int8 per-channel quantization model running on ESP32-S3 SoC
eva = Evaluator('int8', 'per-channel', 'esp32s3')

// use quantization parameters in the pickle file to generate the int8 model
eva.generate_quantized_model(model_proto, pickle_file_path)

// return results in floating-point values
outputs = eva.evaluate_quantized_model(test_images, to_float = True)
res = np.argmax(outputs[0])
```

## 示例

量化、评估 MNIST 模型的完整代码示例，请参考 [example.py](examples/example.py)。

将 TensorFlow MNIST 模型转换为 ONNX 模型的代码示例，请参考 [mnist_tf.py](examples/tensorflow_to_onnx/mnist_tf.py)。

将 MXNet MNIST 模型转换为 ONNX 模型的代码示例，请参考 [mnist_mxnet.py](examples/mxnet_to_onnx/mnist_mxnet.py)。

将 PyTorch MNIST 模型转换为 ONNX 模型的代码示例，请参考 [mnist_pytorch.py](examples/pytorch_to_onnx/mnist_pytorch.py)。


## 资源

下列工具可帮助您将模型转换为 ONNX 格式。

- TensorFlow、Keras 和 Tflite 转换为 ONNX：[tf2onnx](https://github.com/onnx/tensorflow-onnx) 
- MXNet 转换为 ONNX：[MXNet-ONNX](https://mxnet.apache.org/versions/1.8.0/api/python/docs/tutorials/deploy/export/onnx.html) 
- PyTorch 转换为 ONNX：[torch.onnx](https://pytorch.org/docs/stable/onnx.html)

环境要求：
- Python == 3.7
- [Numba](https://github.com/numba/numba) == 0.53.1
- [ONNX](https://github.com/onnx/onnx) == 1.9.0
- [ONNX Runtime](https://github.com/microsoft/onnxruntime) == 1.7.0
- [ONNX Optimizer](https://github.com/onnx/optimizer) == 0.2.6

您可以使用 requirement.txt 来安装相关 Python 依赖包：
```cpp
pip install -r requirement.txt
```