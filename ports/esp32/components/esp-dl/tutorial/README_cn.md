# 定制模型的步骤介绍 [[English]](./README.md)

本教程介绍了定制模型的详细步骤。教程中的示例是可运行的 [MNIST](https://tensorflow.google.cn/datasets/catalog/mnist) 分类项目，以下简称 MNIST。

有关如何定制层，请查看[定制层的步骤介绍](../docs/zh_CN/implement_custom_layer.md)。

本教程的结构如下所示。

```bash
tutorial/
├── CMakeLists.txt
├── main
│   ├── app_main.cpp
│   └── CMakeLists.txt
├── model
│   ├── mnist_coefficient.cpp	(generated in Step 3)
│   ├── mnist_coefficient.hpp	(generated in Step 3)
│   ├── mnist_model.hpp
│   └── npy
│       ├── config.json
│       ├── l1_bias.npy
│       ├── l1_filter.npy
│       ├── l2_compress_bias.npy
│       ├── l2_compress_filter.npy
│       ├── l2_depth_filter.npy
│       ├── l3_a_compress_bias.npy
│       ├── l3_a_compress_filter.npy
│       ├── l3_a_depth_filter.npy
│       ├── l3_b_compress_bias.npy
│       ├── l3_b_compress_filter.npy
│       ├── l3_b_depth_filter.npy
│       ├── l3_c_compress_bias.npy
│       ├── l3_c_compress_filter.npy
│       ├── l3_c_depth_filter.npy
│       ├── l3_d_compress_bias.npy
│       ├── l3_d_compress_filter.npy
│       ├── l3_d_depth_filter.npy
│       ├── l3_e_compress_bias.npy
│       ├── l3_e_compress_filter.npy
│       ├── l3_e_depth_filter.npy
│       ├── l4_compress_bias.npy
│       ├── l4_compress_filter.npy
│       ├── l4_depth_activation.npy
│       ├── l4_depth_filter.npy
│       ├── l5_compress_bias.npy
│       ├── l5_compress_filter.npy
│       ├── l5_depth_activation.npy
│       └── l5_depth_filter.npy
└── README.md
```



## 步骤 1：保存模型系数

使用 [numpy.save ()](https://numpy.org/doc/stable/reference/generated/numpy.save.html?highlight=save#numpy.save) 函数，保存 .npy 格式的模型浮点系数：

```
numpy.save(file=f'{filename}', arr=coefficient)
```

神经网络的每一层都需要有：

- **过滤器**：保存为 `'{layer_name}_filter.npy'`
- **偏差**：保存为 `'{layer_name}_bias.npy'`
- **激活函数**：具有系数的激活函数，如 *LeakyReLU*、*PReLU*，保存为 `'{layer_name}_activation.npy'`

**示例**： [`./model/npy/`](./model/npy/) 文件夹中 .npy 文件里的 MNIST 项目系数。



## 步骤 2：配置模型

根据 [**config.json 配置规范**](../tools/convert_tool/specification_of_config_json_cn.md)，在 config.json 文件中配置模型。

**示例**： [`./model/npy/config.json`](./model/npy/config.json) 文件中 MNIST 项目的配置。



## 步骤 3：转换模型系数


将 coefficient.npy 文件和 config.json 准备好且保存在同一文件夹后，使用 convert.py（请参考 [**convert.py 使用说明**](../tools/convert_tool/README_cn.md)）把系数转换为 C/C++ 代码。

**示例**：

运行如下命令

```bash
python ../convert.py -i ./model/npy/ -n mnist_coefficient -o ./model/
```

然后 [`./model/`](./model/) 文件夹中会生成两个文件：`mnist_coefficient.cpp` 和 `mnist_coefficient.hpp`。

之后，调用 `get_{layer_name}_***()` 即可获取每层的系数。比如要获取 "l1" 的过滤器，可调用 `get_l1_filter()`。


## 步骤 4：构建模型

### 步骤 4.1：从 [`"dl_layer_model.hpp"`](../include/layer/dl_layer_model.hpp) 中的模型类派生一个新类

```c++
class MNIST : public Model<int16_t>
{
};
```



### 步骤 4.2：将层声明为成员变量

```c++
class MNIST : public Model<int16_t>
{
private:
    Conv2D<int16_t> l1;                  // a layer named l1
    DepthwiseConv2D<int16_t> l2_depth;   // a layer named l2_depth
    Conv2D<int16_t> l2_compress;         // a layer named l2_compress
    DepthwiseConv2D<int16_t> l3_a_depth; // a layer named l3_a_depth
    Conv2D<int16_t> l3_a_compress;       // a layer named l3_a_compress
    DepthwiseConv2D<int16_t> l3_b_depth; // a layer named l3_b_depth
    Conv2D<int16_t> l3_b_compress;       // a layer named l3_b_compress
    DepthwiseConv2D<int16_t> l3_c_depth; // a layer named l3_c_depth
    Conv2D<int16_t> l3_c_compress;       // a layer named l3_c_compress
    DepthwiseConv2D<int16_t> l3_d_depth; // a layer named l3_d_depth
    Conv2D<int16_t> l3_d_compress;       // a layer named l3_d_compress
    DepthwiseConv2D<int16_t> l3_e_depth; // a layer named l3_e_depth
    Conv2D<int16_t> l3_e_compress;       // a layer named l3_e_compress
    Concat2D<int16_t> l3_concat;         // a layer named l3_concat
    DepthwiseConv2D<int16_t> l4_depth;   // a layer named l4_depth
    Conv2D<int16_t> l4_compress;         // a layer named l4_compress
    DepthwiseConv2D<int16_t> l5_depth;   // a layer named l5_depth

public:
    Conv2D<int16_t> l5_compress; // a layer named l5_compress. Make the l5_compress public, as the l5_compress.get_output() will be fetched outside the class.
};
```



### 步骤 4.3：用构造函数初始化层

[步骤 3：转换模型系数](#步骤-3转换模型系数)生成的 `"mnist_coefficient.hpp"` 文件中有层的系数，用该系数初始化层。

有关如何初始化每一层，请查看 [esp-dl/include/layer/](../include/layer/) 文件夹中相应的 .hpp 文件。

```c++
class MNIST : public Model<int16_t>
{
    // ellipsis member variables
    
    MNIST() : l1(Conv2D<int16_t>(-2, get_l1_filter(), get_l1_bias(), get_l1_activation(), PADDING_VALID, {}, 2, 2, "l1")),
              l2_depth(DepthwiseConv2D<int16_t>(-1, get_l2_depth_filter(), NULL, get_l2_depth_activation(), PADDING_SAME_END, {}, 2, 2, "l2_depth")),
              l2_compress(Conv2D<int16_t>(-3, get_l2_compress_filter(), get_l2_compress_bias(), NULL, PADDING_SAME_END, {}, 1, 1, "l2_compress")),
              l3_a_depth(DepthwiseConv2D<int16_t>(-1, get_l3_a_depth_filter(), NULL, get_l3_a_depth_activation(), PADDING_VALID, {}, 1, 1, "l3_a_depth")),
              l3_a_compress(Conv2D<int16_t>(-12, get_l3_a_compress_filter(), get_l3_a_compress_bias(), NULL, PADDING_VALID, {}, 1, 1, "l3_a_compress")),
              l3_b_depth(DepthwiseConv2D<int16_t>(-2, get_l3_b_depth_filter(), NULL, get_l3_b_depth_activation(), PADDING_VALID, {}, 1, 1, "l3_b_depth")),
              l3_b_compress(Conv2D<int16_t>(-12, get_l3_b_compress_filter(), get_l3_b_compress_bias(), NULL, PADDING_VALID, {}, 1, 1, "l3_b_compress")),
              l3_c_depth(DepthwiseConv2D<int16_t>(-12, get_l3_c_depth_filter(), NULL, get_l3_c_depth_activation(), PADDING_SAME_END, {}, 1, 1, "l3_c_depth")),
              l3_c_compress(Conv2D<int16_t>(-12, get_l3_c_compress_filter(), get_l3_c_compress_bias(), NULL, PADDING_SAME_END, {}, 1, 1, "l3_c_compress")),
              l3_d_depth(DepthwiseConv2D<int16_t>(-12, get_l3_d_depth_filter(), NULL, get_l3_d_depth_activation(), PADDING_SAME_END, {}, 1, 1, "l3_d_depth")),
              l3_d_compress(Conv2D<int16_t>(-11, get_l3_d_compress_filter(), get_l3_d_compress_bias(), NULL, PADDING_SAME_END, {}, 1, 1, "l3_d_compress")),
              l3_e_depth(DepthwiseConv2D<int16_t>(-11, get_l3_e_depth_filter(), NULL, get_l3_e_depth_activation(), PADDING_SAME_END, {}, 1, 1, "l3_e_depth")),
              l3_e_compress(Conv2D<int16_t>(-12, get_l3_e_compress_filter(), get_l3_e_compress_bias(), NULL, PADDING_SAME_END, {}, 1, 1, "l3_e_compress")),
              l3_concat(-1, "l3_concat"),
              l4_depth(DepthwiseConv2D<int16_t>(-12, get_l4_depth_filter(), NULL, get_l4_depth_activation(), PADDING_VALID, {}, 1, 1, "l4_depth")),
              l4_compress(Conv2D<int16_t>(-11, get_l4_compress_filter(), get_l4_compress_bias(), NULL, PADDING_VALID, {}, 1, 1, "l4_compress")),
              l5_depth(DepthwiseConv2D<int16_t>(-10, get_l5_depth_filter(), NULL, get_l5_depth_activation(), PADDING_VALID, {}, 1, 1, "l5_depth")),
              l5_compress(Conv2D<int16_t>(-9, get_l5_compress_filter(), get_l5_compress_bias(), NULL, PADDING_VALID, {}, 1, 1, "l5_compress")) {}

};
```



### 步骤 4.4：实现 `void build(Tensor<input_t> &input)`

为了便于区分`模型` `build()` 和`层` `build()`，现定义：

* `模型` `build()` 为 `Model.build()`；
* `层` `build()` 为 `Layer.build()`。

`Model.build()` 会调用所有 `Layer.build()`。`Model.build()` 仅在输入形状变化时有效。若输入形状没有变化，则 `Model.build()` 不会被调用，从而节省计算时间。

有关 `Model.build()` 何时被调用，请查看[步骤 5：运行模型](#步骤-5运行模型)。

有关如何调用每一层的 `Layer.build()`，请查看 [esp-dl/include/layer/](../include/layer/) 文件夹中相应的 .hpp 文件。

```c++
class MNIST : public Model<int16_t>
{
    // ellipsis member variables
    // ellipsis constructor function
    
    void build(Tensor<int16_t> &input)
    {
        this->l1.build(input);
        this->l2_depth.build(this->l1.get_output());
        this->l2_compress.build(this->l2_depth.get_output());
        this->l3_a_depth.build(this->l2_compress.get_output());
        this->l3_a_compress.build(this->l3_a_depth.get_output());
        this->l3_b_depth.build(this->l2_compress.get_output());
        this->l3_b_compress.build(this->l3_b_depth.get_output());
        this->l3_c_depth.build(this->l3_b_compress.get_output());
        this->l3_c_compress.build(this->l3_c_depth.get_output());
        this->l3_d_depth.build(this->l3_b_compress.get_output());
        this->l3_d_compress.build(this->l3_d_depth.get_output());
        this->l3_e_depth.build(this->l3_d_compress.get_output());
        this->l3_e_compress.build(this->l3_e_depth.get_output());
        this->l3_concat.build({&this->l3_a_compress.get_output(), &this->l3_c_compress.get_output(), &this->l3_e_compress.get_output()});
        this->l4_depth.build(this->l3_concat.get_output());
        this->l4_compress.build(this->l4_depth.get_output());
        this->l5_depth.build(this->l4_compress.get_output());
        this->l5_compress.build(this->l5_depth.get_output());
    }
};
```



### 步骤 4.5：实现 `void call(Tensor<input_t> &input)`

`Model.call()` 会调用所有 `Layer.call()`。有关如何调用每一层的 `Layer.call()`，请查看 [esp-dl/include/layer/](../include/layer/) 文件夹中相应的 .hpp 文件。

```c++
class MNIST : public Model<int16_t>
{
    // ellipsis member variables
    // ellipsis constructor function
    // ellipsis build(...)

    void call(Tensor<int16_t> &input)
    {
        this->l1.call(input);
        input.free_element();

        this->l2_depth.call(this->l1.get_output());
        this->l1.get_output().free_element();

        this->l2_compress.call(this->l2_depth.get_output());
        this->l2_depth.get_output().free_element();

        this->l3_a_depth.call(this->l2_compress.get_output());
        // this->l2_compress.get_output().free_element();

        this->l3_a_compress.call(this->l3_a_depth.get_output());
        this->l3_a_depth.get_output().free_element();

        this->l3_b_depth.call(this->l2_compress.get_output());
        this->l2_compress.get_output().free_element();

        this->l3_b_compress.call(this->l3_b_depth.get_output());
        this->l3_b_depth.get_output().free_element();

        this->l3_c_depth.call(this->l3_b_compress.get_output());
        // this->l3_b_compress.get_output().free_element();

        this->l3_c_compress.call(this->l3_c_depth.get_output());
        this->l3_c_depth.get_output().free_element();

        this->l3_d_depth.call(this->l3_b_compress.get_output());
        this->l3_b_compress.get_output().free_element();

        this->l3_d_compress.call(this->l3_d_depth.get_output());
        this->l3_d_depth.get_output().free_element();

        this->l3_e_depth.call(this->l3_d_compress.get_output());
        this->l3_d_compress.get_output().free_element();

        this->l3_e_compress.call(this->l3_e_depth.get_output());
        this->l3_e_depth.get_output().free_element();

        this->l3_concat.call({&this->l3_a_compress.get_output(), &this->l3_c_compress.get_output(), &this->l3_e_compress.get_output()}, true);
                
        this->l4_depth.call(this->l3_concat.get_output());
        this->l3_concat.get_output().free_element();

        this->l4_compress.call(this->l4_depth.get_output());
        this->l4_depth.get_output().free_element();

        this->l5_depth.call(this->l4_compress.get_output());
        this->l4_compress.get_output().free_element();

        this->l5_compress.call(this->l5_depth.get_output());
        this->l5_depth.get_output().free_element();
    }
};
```


## 步骤 5：运行模型

- 创建模型对象

- 运行 `Model.forward()` 进行神经网络推理。`Model.forward()` 的过程如下：

  ```c++
  forward()
  {
  	if (input_shape is changed)
  	{
  		Model.build();
  	}
  	Model.call();
  }
  ```

  

**示例**：[`./main/main.cpp`](./main/app_main.cpp) 文件中的 MNIST 对象和 `forward()` 函数。

```c++
// model forward
MNIST model;
model.forward(input);
```

