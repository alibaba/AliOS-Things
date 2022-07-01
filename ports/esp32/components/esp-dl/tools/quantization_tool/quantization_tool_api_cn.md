# 量化工具包 API [[English]](./quantization_tool_api.md)

## 校准器类

### 初始化

```
Calibrator(quantization_bit, granularity='per-tensor', calib_method='minmax')
```

**实参**
- **quantization_bit** _(string)_：
  - 'int8' 代表全 8 位整数量化。
  - 'int16' 代表全 16 位整数量化。
- **granularity** _(string)_：
  - 若 granularity = 'per-tensor'（默认），则整个张量只有一个指数。
  - 若 granularity = 'per-channel'，则卷积层的每个通道都有一个指数。
- **calib_method** _(string)_：   
  - 若 calib_method = 'minmax'（默认），则阈值得自校准数据集每层输出的最小值和最大值。
  - 若 calib_method = 'entropy'，则阈值得自 KL 散度。

### *check_model* 方法

```
Calibrator.check_model(model_proto)
```
检查模型的适配性。

**实参**
- **model_proto** _(ModelProto)_：一个 FP32 ONNX 模型。

**返回值**
- **-1**：模型不适配。

### *set_method* 方法
```
Calibrator.set_method(granularity, calib_method)
```
配置量化。

**实参**
- **granularity** _(string)_：
  - 若 granularity = 'per-tensor'，则整个张量只有一个指数。
  - 若 granularity = 'per-channel'，则卷积层的每个通道都有一个指数。
- **calib_method** _(string)_：   
  - 若 calib_method = 'minmax'，则阈值得自校准数据集每层输出的最小值和最大值。
  - 若 calib_method = 'entropy'，则阈值得自 KL 散度。

### *set_providers* 方法
```
Calibrator.set_providers(providers)
```
配置 ONNX Runtime 的运行环境提供方。

**实参**
- **providers** _(list of strings)_：[列表](https://onnxruntime.ai/docs/reference/execution-providers/)中的运行环境提供方，如 'CPUExecutionProvider'、'CUDAExecutionProvider'。


### *generate_quantization_table* 方法
```
Calibrator.generate_quantization_table(model_proto, calib_dataset, pickle_file_path)
```
生成量化表。

**实参**
- **model_proto** _(ModelProto)_：一个 FP32 ONNX 模型。
- **calib_dataset** _(ndarray)_：用于计算阈值的校准数据集。数据集越大，生成量化表的时间越长。
- **pickle_file_path** _(string)_：存储量化参数的 pickle 文件路径。


### *export_coefficient_to_cpp* 方法
```
Calibrator.export_coefficient_to_cpp(model_proto, pickle_file_path, target_chip, output_path, file_name, print_model_info=False)
```
导出量化参数。

**Arguments**
- **model_proto** _(ModelProto)_: 一个 FP32 ONNX 模型。
- **pickle_file_path** _(string)_: 存储量化参数的 pickle 文件路径。
- **target_chip** _(string)_: 目前支持 'esp32'、'esp32s2'、'esp32c3' 、'esp32s3'。 
- **output_path** _(string)_: 存储输出文件的路径。
- **file_name** _(string)_: 输出文件的名称。
- **print_model_info**_(bool)_: 
  - False (default): 不打印任何信息。
  - True: 打印模型的相关信息。


## 评估器类

### 初始化

```
Evaluator(quantization_bit, granularity, target_chip)
```
**实参**
- **quantization_bit** _(string)_：
  - 'int8' 代表全 8 位整数量化。
  - 'int16' 代表全 16 位整数量化。
- **granularity** _(string)_：
  - 若 granularity = 'per-tensor'，则整个张量只有一个指数。
  - 若 granularity = 'per-channel'，则卷积层的每个通道都有一个指数。
- **target_chip** _(string)_：默认是 'esp32s3'。


### *check_model* 方法
```
Evaluator.check_model(model_proto)
```
检查模型的适配性。

**实参**
- **model_proto** _(ModelProto)_：一个 FP32 ONNX 模型。

**Return**
- **-1**：模型不适配。

### *set_target_chip* 方法
```
Evaluator.set_target_chip(target_chip)
```
配置模拟芯片环境。

**实参**
- **target_chip** _(string)_：目前仅支持 'esp32s3'。


### *set_providers* 方法
```
Evaluator.set_providers(providers)
```

配置 ONNX Runtime 的运行环境提供方。

**实参**
- **providers** _(list of strings)_：[列表](https://onnxruntime.ai/docs/reference/execution-providers/)中的运行环境提供方，如 'CPUExecutionProvider'、'CUDAExecutionProvider'。

### *generate_quantized_model* 方法
```
Evaluator.generate_quantized_model(model_proto, pickle_file_path)
```
生成量化后的模型。

**实参**
- **model_proto** _(ModelProto)_：一个 FP32 ONNX 模型。
- **pickle_file_path** _(string)_：存储 FP32 ONXX 模型所有量化参数的 pickle 文件路径。该 pickle 文件必须包含模型计算图所有输入和输出节点的量化参数。


### *evaluate_quantized_model* 方法
```
Evaluator.evaluate_quantized_model(batch_fp_input, to_float=False)
```
获取量化模型的输出。

**实参**
- **batch_fp_input** _(ndarray)_：批量浮点输入。
- **to_float** _(bool)_： 
  - False（默认）：直接返回输出。
  - True：输出转换为浮点值。

**返回值**

outputs 和 output_names 组成的元组：
- **outputs** _(list of ndarray)_：量化模型的输出。
- **output_names** _(list of strings)_：输出名称。


