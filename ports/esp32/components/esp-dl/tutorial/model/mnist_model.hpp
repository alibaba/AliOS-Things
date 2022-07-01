#pragma once
#include "dl_layer_model.hpp"
#include "dl_layer_conv2d.hpp"
#include "dl_layer_depthwise_conv2d.hpp"
#include "dl_layer_concat.hpp"
#include "mnist_coefficient.hpp"
#include <stdint.h>

using namespace dl;
using namespace layer;
using namespace mnist_coefficient;

class MNIST : public Model<int16_t> // Derive the Model class in "dl_layer_model.hpp"
{
private:
    // Declare layers as member variables
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
    Concat<int16_t> l3_concat;           // a layer named l3_concat
    DepthwiseConv2D<int16_t> l4_depth;   // a layer named l4_depth
    Conv2D<int16_t> l4_compress;         // a layer named l4_compress
    DepthwiseConv2D<int16_t> l5_depth;   // a layer named l5_depth

public:
    Conv2D<int16_t> l5_compress; // a layer named l5_compress

    /**
     * @brief Initialize layers in constructor function
     * 
     */
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

    /**
     * @brief call each layers' build(...) function in sequence
     * 
     * @param input 
     */
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

    /**
     * @brief call each layers' call(...) function in sequence
     * 
     * @param input 
     */
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