# Specification of config.json [[中文]](./specification_of_config_json_cn.md)

> The config.json file saves configurations used to quantize floating points in coefficient.npy.



## Specification

Each item in config.json stands for the configuration of one layer. Take the following code as an example:

```json
{
    "l1": {"/* the configuration of layer l1 */"},
    "l2": {"/* the configuration of layer l2 */"},
    "l3": {"/* the configuration of layer l3 */"},
    ...
}
```

The key of each item is the **layer name**. The convert tool ``convert.py`` searches for the corresponding .npy files according to the layer name. For example, if a layer is named "l1", the tool will search for l1's filter coefficients in "l1_filter.npy". **The layer name in config.json should be consistent with the layer name in the name of .npy files.**



The value of each item is the **layer configuration**. Please fill arguments about layer configuration listed in Table 1:

<div align=center>Table 1: Layer Configuration Arguments</div>

| Key | Type | Value |
|---|:---:|---|
| "operation" | string | - "conv2d"<br>- "depthwise_conv2d"<br>- "fully_connected"|
| "feature_type" | string | - "s16" for int16 quantization with element_width = 16<br>- "s8" for int8 quantization with element_width = 8 |
| "filter_exponent" | integer | - If filled, filter is quantized according to the equation: value_float = value_int * 2^exponent<sup>[1](#note1)</sup><br>- If dropped<sup>[2](#note2)</sup>, exponent is determined according to the equation: exponent = log2(max(abs(value_float)) / 2^(element_width - 1)), while filter is quantized according to the equation: value_float = value_int * 2^exponent |
| "bias" | string | - "True" for adding bias<br>- "False" and dropped for no bias |
| "output_exponent" | integer | Both output and bias are quantized according to the equation: value_float = value_int * 2^exponent.<br>For now, "output_exponent" is effective only for "bias" coefficient conversion. "output_exponent" must be provided when using per-tensor quantization. If there is no "bias" in a specific layer or using per-channel quantization, "output_exponent" could be dropped. |
| "input_exponent" | integer | When using per-channel quantization, the exponent of bias is related to input_exponent and filter_exponent. <br>"input_exponent" must be provided for "bias" coefficient conversion. If there is no "bias" in a specific layer or using per-tensor quantization, "output_exponent" could be dropped. |
| "activation" | dict | - If filled, see details in Table 2<br>- If dropped, no activation |

<div align=center>Table 2: Activation Configuration Arguments</div>

| Key | Type | Value |
|---|:---:|---|
| "type" | string | - "ReLU"<br>- "LeakyReLU"<br>- "PReLU" |
| "exponent" | integer | - If filled, activation is quantized according to the equation, value_float = value_int  * 2^exponent<br>- If dropped, exponent is determined according to the equation: exponent = log2(max(abs(value_float)) / 2^(element_width - 1)) |


> <a name="note1">1</a>: **exponent**: the number of times the base is multiplied by itself for quantization. For better understanding, please refer to [*Quantization Specification*](../../docs/en/quantization_specification.md).
>
> <a name="note2">2</a>: **dropped**: to leave a specific argument empty.




## Example

Assume that for a one-layer model:

##### 1. using int16 per-tensor quantization:
- layer name: "mylayer"
- operation: Conv2D(input, filter) + bias
- output_exponent: -10, exponent for the result of operation
- feature_type: s16, which means int16 quantization
- type of activation: PReLU 

The config.json file should be written as:

```json
{
	"mylayer": {
		"operation": "conv2d",
		"feature_type": "s16",
        "bias": "True",
        "output_exponent": -10,
        "activation": {
            "type": "PReLU"
        }
	}
}
```
> "filter_exponent" and "exponent" of "activation" are dropped. <br/>
>  must provide "output_exponent" for bias in this layer.


##### 2. using int8 per-tensor quantization:
- layer name: "mylayer"
- operation: Conv2D(input, filter) + bias
- output_exponent: -7, exponent for the result of this layer
- feature_type: s8
- type of activation: PReLU 

The config.json file should be written as:

```json
{
	"mylayer": {
		"operation": "conv2d",
		"feature_type": "s8",
        "bias": "True",
        "output_exponent": -7,
        "activation": {
            "type": "PReLU"
        }
	}
}
```
> must provide "output_exponent" for bias in this layer.


##### 3. using int8 per-channel quantization:
- layer name: "mylayer"
- operation: Conv2D(input, filter) + bias
- input_exponent: -7, exponent for the input of this layer
- feature_type: s8
- type of activation: PReLU 

The config.json file should be written as:

```json
{
	"mylayer": {
		"operation": "conv2d",
		"feature_type": "s8",
        "bias": "True",
        "input_exponent": -7,
        "activation": {
            "type": "PReLU"
        }
	}
}
```
> must provide "input_exponent" for bias in this layer.

Meanwhile, `mylayer_filter.npy`, `mylayer_bias.npy` and `mylayer_activation.npy` should be ready.
