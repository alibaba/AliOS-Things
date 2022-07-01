# Customize a Layer Step by Step [[中文]](../zh_CN/implement_custom_layer.md)

The implemented layers of ESP-DL, e.g. Conv2D, DepthwiseConv2D, are derived from the base **Layer** class in [`./include/layer/dl_layer_base.hpp`](../../include/layer/dl_layer_base.hpp). The Layer class only has one member variable `name`. Although if `name` is not used it would be unnecessary to customize a layer derived from the Layer class, we recommend doing so for code consistency.

The example in this document is not runnable, but only for design reference. For a runnable example, please check header files in the [./include/layer/](../../include/layer/) folder, which contains layers such as Conv2D, DepthwiseConv2D, Concat2D, etc.

As the input and output of a layer are tensors, **it is quite necessary to learn about Tensor in [About Variables and Constants](./about_type_define.md/#Tensor)**.

Let's start to customize a layer!



## Step 1: Derive from the Layer class

Derive a new layer (named `MyLayer` in the example) from the Layer class, and then member variables, constructor and destructor according to requirements. Do not forget to initialize the constructor of the base class.

```c++
class MyLayer : public Layer
{
private:
    /* private member variables */
public:
    /* public member variables */
    Tensor<int16_t> output; /*<! output of this layer */

    MyLayer(/* arguments */) : Layer(name)
    {
        // initialize anything frozen
    }

    ~MyLayer()
    {
        // destroy
    }
};
```



## Step 2: Implement `build()`

A layer always has one or multiple inputs and one output. For now, `build()` is implemented for the purposes of:

- **Updating Output Shape**:
    
    The output shape is determined by the input shape, and sometimes the shape of coefficients as well. For example, the output shape of Conv2D is determined by its input shape, filter shape, stride, and dilation. In a running application, some configurations of a layer are fixed, such as the filter shape, stride and dilation, but the input shape is probably variable. Once the input shape is changed, the output shape should be changed accordingly. `build()` is implemented for the first purpose: updating the output shape according to the input shape.

- **Updating Input Padding**: 

    In 2D convolution layers such as Conv2D and DepthwiseConv2D, input tensors probably need to be padded. Like output shape, input padding is also determined by input shape, and sometimes the shape of coefficients as well. For example, the padding of an input to a Conv2D layer is determined by the input shape, filter shape, stride, dilation and padding type. `build()` is implemented for the second purpose: updating input padding according to the shape of the to-be-padded input.

`build()` is not limited to the above two purposes. **All updates made according to input could be implemented by build()**.


```c++
class MyLayer : public Layer
{
    // ellipsis member variables
    // ellipsis constructor and destructor

    void build(Tensor<int16_t> &input)
    {
        /* get output_shape according to input shape and other configuration */
        this->output.set_shape(output_shape); // update output_shape

        /* get padding according to input shape and other configuration */
        input.set_padding(this->padding);
    }
};
```



## Step 3: Implement call()

Implement layer inference operation in `call()`. Please pay attention to:

- **memory allocation** for `output.element` via [`Tensor.apply_element()`](../../include/typedef/dl_variable.hpp), [`Tensor.malloc_element()`](../../include/typedef/dl_variable.hpp) or [`Tensor.calloc_element()`](../../include/typedef/dl_variable.hpp/#122);
- **dimension order of tensors described in [About Variables and Constants](./about_type_define.md/#Tensor)**, as both input and output are [`dl::Tensor`](../../include/typedef/dl_variable.hpp).

```c++
class MyLayer : public Layer
{
    // ellipsis member variables
    // ellipsis constructor and destructor
    // ellipsis build(...)

    Tensor<feature_t> &call(Tensor<int16_t> &input, /* other arguments */)
    {
        this->output.calloc_element(); // calloc memory for output.element

        /* implement operation */
        
        return this->output;
    }
};
```

