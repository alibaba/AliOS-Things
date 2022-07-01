# Quantization Toolkit [[中文]](./README_cn.md)

The quantization toolkit helps you deploy the quantized inference on ESP SoCs with models using ESP-DL. Such toolkit runs based on [Open Neural Network Exchange (ONNX)](https://github.com/onnx/onnx), an open source format for AI models.

The toolkit consists of three independent tools:

- an [optimizer](#optimizer) to perform graph optimization
- a [calibrator](#calibrator) for post-training quantization, without the need for retraining
- an [evaluator](#evaluator) to evaluate the performance of the quantized model

This document covers the specifications of each tool. For corresponding APIs, please refer to [Quantization Toolkit API](quantization_tool_api.md).

> Please ensure that before you use the toolkit, your model is converted to ONNX format. For help with ONNX, please see [Resources](#resources).


## Optimizer

The graph optimizer [optimizer.py](optimizer.py) improves model performance through redundant node elimination, model structure simplification, model fusion, etc. It is based on [ONNX Optimizer](https://github.com/onnx/optimizer) which provides a list of [optimization passes](https://github.com/onnx/optimizer/tree/master/onnxoptimizer/passes), together with some additional passes implemented by us.

It is important to enable graph fusion before quantization, especially batch normalization fusion. Therefore, we recommended passing your model through the optimizer before you use the calibrator or evaluator. You can use [Netron](https://github.com/lutzroeder/netron) to view your model structure.


**Python API example**

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

## Calibrator

The calibrator quantizes a floating-point model which meets the requirements of running inference on ESP SoCs. For details about supported forms of quantization, please check [Quantization Specification](../../docs/en/quantization_specification.md).

To convert a 32-bit floating-point (FP32) model into a 8-bit integer (int8) or a 16-bit integer (int16) model, the overall workflow is:
- prepare an FP32 model
- prepare a calibration dataset
- configure quantization
- obtain quantization parameters

### FP32 Model

The prepared FP32 model must be compatible with the existing library. If the model contains operations that are not supported, the calibrator will not accept it and will generate error messages.

The model's compatibility is checked when you obtain quantization parameters, or via calling *check_model* in advance. 

The input of the model should be already normalized. If the nodes for normalization are included in the model graph, please delete them for better performance.

### Calibration Dataset

Choosing an appropriate calibration dataset is important for quantization. A good calibration dataset should be representative. You can try different calibration datasets and compare performance of quantized models using different quantization parameters.

### Quantization Configuration

The calibrator supports both int8 and int16 quantization. Below is the configuration for int8 and int16 respectively:

int8:
- granularity: 'per-tensor', 'per-channel'
- calibration_method: 'entropy', 'minmax'

int16:
- granularity: 'per-tensor'
- calibration_method: 'minmax'

### Quantization Parameters

As described in [Quantization Specification](../../docs/en/quantization_specification.md), 8-bit or 16-bit quantization in ESP-DL approximates floating-point values using the following formula：

```math
real\_value = int\_value * 2^{\ exponent}
```

where 2^exponent is called scale.

The returned quantization table is a list of quantization scales for all data in the model, including:
- constant values, such as weights, biases and activations;
- variable tensors, such as model input and outputs of intermediate layers (activations).


**Python API example**

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

## Evaluator

The evaluator is a tool to simulate quantization and help you evaluate performance of the quantized model as it runs on ESP SoCs.

If the model contains operations that are not supported by ESP-DL, the evaluator will not accept it and will generate error messages.

If performance of the quantized model does not satisfy your needs, please consider quantization aware training.


**Python API example**

```cpp
// initialize an evaluator to generate an MNIST using int8 per-channel quantization model running on ESP32-S3 SoC
eva = Evaluator('int8', 'per-channel', 'esp32s3')

// use quantization parameters in the pickle file to generate the int8 model
eva.generate_quantized_model(model_proto, pickle_file_path)

// return results in floating-point values
outputs = eva.evaluate_quantized_model(test_images, to_float = True)
res = np.argmax(outputs[0])
```

## Example

For complete example codes to quantize and evaluate a MNIST model, please refer to [example.py](examples/example.py).

For example codes to convert a TensorFlow MNIST model to an ONNX model, please refer to [mnist_tf.py](examples/tensorflow_to_onnx/mnist_tf.py).

For example codes to convert a MXNet MNIST model to an ONNX model, please refer to [mnist_mxnet.py](examples/mxnet_to_onnx/mnist_mxnet.py).

For example codes to convert a PyTorch MNIST model to an ONNX model, please refer to [mnist_pytorch.py](examples/pytorch_to_onnx/mnist_pytorch.py).


## Resources

The following tools may be helpful to convert a model into ONNX format.

- From TensorFlow, Keras and Tflite to ONNX: [tf2onnx](https://github.com/onnx/tensorflow-onnx) 
- From MXNet to ONNX: [MXNet-ONNX](https://mxnet.apache.org/versions/1.8.0/api/python/docs/tutorials/deploy/export/onnx.html) 
- From PyTorch to ONNX: [torch.onnx](https://pytorch.org/docs/stable/onnx.html)

Environment Requirements:
- Python == 3.7
- [Numba](https://github.com/numba/numba) == 0.53.1
- [ONNX](https://github.com/onnx/onnx) == 1.9.0
- [ONNX Runtime](https://github.com/microsoft/onnxruntime) == 1.7.0
- [ONNX Optimizer](https://github.com/onnx/optimizer) == 0.2.6

You can install python packages with requirement.txt:
```cpp
pip install -r requirement.txt
```

