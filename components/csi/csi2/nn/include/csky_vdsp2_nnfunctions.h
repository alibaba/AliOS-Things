/* ----------------------------------------------------------------------
 * Title:        csky_vdsp2_nnfunctions.h
 * Description:  Public header file for CSI NN Library
 *
 * -------------------------------------------------------------------- */

#ifndef _CSKY_VDSP2_NNFUNCTIONS_H
#define _CSKY_VDSP2_NNFUNCTIONS_H

#ifdef __cplusplus
extern    "C"
{
#endif

#ifdef CSKY_VDSP2_MATH_DSP
#include "csky_vdsp2_math.h"
#include "csky_vdsp2_nnsupportfunctions.h"
#endif

/**
 * @brief Struct for specifying activation function types
 *
 */
typedef enum
{
    CSKY_SIGMOID = 0, /**< Sigmoid activation function */
    CSKY_TANH = 1, /**< Tanh activation function */
} csky_vdsp2_nn_activation_type;

  /**
   * @brief Basic Q7 convolution function
   * @param[in]       Im_in       pointer to input tensor
   * @param[in]       dim_im_in   input tensor dimention
   * @param[in]       ch_im_in    number of input tensor channels
   * @param[in]       wt          pointer to kernel weights
   * @param[in]       ch_im_out   number of filters, i.e., output tensor channels
   * @param[in]       dim_kernel  filter kernel size
   * @param[in]       padding     padding sizes
   * @param[in]       stride      convolution stride
   * @param[in]       bias        pointer to bias
   * @param[in]       bias_shift  amount of left-shift for bias
   * @param[in]       out_shift   amount of right-shift for output
   * @param[in,out]   Im_out      pointer to output tensor
   * @param[in]       dim_im_out  output tensor dimension
   * @param[in,out]   bufferA     pointer to buffer space for input
   * @return          none.
   *
   */

void csky_vdsp2_convolve_HWC_q7_basic(const q7_t * Im_in,
                                       const uint16_t dim_im_in,
                                       const uint16_t ch_im_in,
                                       const q7_t * wt,
                                       const uint16_t ch_im_out,
                                       const uint16_t dim_kernel,
                                       const uint16_t padding,
                                       const uint16_t stride,
                                       const q7_t * bias,
                                       const uint16_t bias_shift,
                                       const uint16_t out_shift,
                                       q7_t * Im_out,
                                       const uint16_t dim_im_out,
                                       q15_t * bufferA);

  /**
   * @brief Basic Q15 convolution function
   * @param[in]       Im_in       pointer to input tensor
   * @param[in]       dim_im_in   input tensor dimention
   * @param[in]       ch_im_in    number of input tensor channels
   * @param[in]       wt          pointer to kernel weights
   * @param[in]       ch_im_out   number of filters, i.e., output tensor channels
   * @param[in]       dim_kernel  filter kernel size
   * @param[in]       padding     padding sizes
   * @param[in]       stride      convolution stride
   * @param[in]       bias        pointer to bias
   * @param[in]       bias_shift  amount of left-shift for bias
   * @param[in]       out_shift   amount of right-shift for output
   * @param[in,out]   Im_out      pointer to output tensor
   * @param[in]       dim_im_out  output tensor dimension
   * @param[in,out]   bufferA     pointer to buffer space for input
   * @return          none.
   *
   */

void csky_vdsp2_convolve_HWC_q15_basic(const q15_t * Im_in,
                                        const uint16_t dim_im_in,
                                        const uint16_t ch_im_in,
                                        const q15_t * wt,
                                        const uint16_t ch_im_out,
                                        const uint16_t dim_kernel,
                                        const uint16_t padding,
                                        const uint16_t stride,
                                        const q15_t * bias,
                                        const uint16_t bias_shift,
                                        const uint16_t out_shift,
                                        q15_t * Im_out,
                                        const uint16_t dim_im_out,
                                        q15_t * bufferA);


  /**
   * @brief Fast Q7 convolution function (non-sqaure shape)
   * @param[in]       Im_in        pointer to input tensor
   * @param[in]       dim_im_in_x  input tensor dimention x
   * @param[in]       dim_im_in_y  input tensor dimention y
   * @param[in]       ch_im_in     number of input tensor channels
   * @param[in]       wt           pointer to kernel weights
   * @param[in]       ch_im_out    number of filters, i.e., output tensor channels
   * @param[in]       dim_kernel_x filter kernel size x
   * @param[in]       dim_kernel_y filter kernel size y
   * @param[in]       padding_x    padding size x
   * @param[in]       padding_y    padding size y
   * @param[in]       stride_x     convolution stride x
   * @param[in]       stride_y     convolution stride y
   * @param[in]       bias         pointer to bias
   * @param[in]       bias_shift   amount of left-shift for bias
   * @param[in]       out_shift    amount of right-shift for output
   * @param[in,out]   Im_out       pointer to output tensor
   * @param[in]       dim_im_out_x output tensor dimension x
   * @param[in]       dim_im_out_y output tensor dimension y
   * @param[in,out]   bufferA      pointer to buffer space for input
   * @return          none.
   *
   * This function is the version with full list of optimization tricks, but with
   * some contraints:
   *   ch_im_in is multiple of 4
   *   ch_im_out is multiple of 2
   */

void csky_vdsp2_convolve_HWC_q7_fast_nonsquare(const q7_t * Im_in,
                                                const uint16_t dim_im_in_x,
                                                const uint16_t dim_im_in_y,
                                                const uint16_t ch_im_in,
                                                const q7_t * wt,
                                                const uint16_t ch_im_out,
                                                const uint16_t dim_kernel_x,
                                                const uint16_t dim_kernel_y,
                                                const uint16_t padding_x,
                                                const uint16_t padding_y,
                                                const uint16_t stride_x,
                                                const uint16_t stride_y,
                                                const q7_t * bias,
                                                const uint16_t bias_shift,
                                                const uint16_t out_shift,
                                                q7_t * Im_out,
                                                const uint16_t dim_im_out_x,
                                                const uint16_t dim_im_out_y,
                                                q15_t * bufferA);

  /**
   * @brief Fast Q7 version of 1x1 convolution (non-sqaure shape)
   * @param[in]       Im_in        pointer to input tensor
   * @param[in]       dim_im_in_x  input tensor dimention x
   * @param[in]       dim_im_in_y  input tensor dimention y
   * @param[in]       ch_im_in     number of input tensor channels
   * @param[in]       wt           pointer to kernel weights
   * @param[in]       ch_im_out    number of filters, i.e., output tensor channels
   * @param[in]       dim_kernel_x filter kernel size x
   * @param[in]       dim_kernel_y filter kernel size y
   * @param[in]       padding_x    padding size x
   * @param[in]       padding_y    padding size y
   * @param[in]       stride_x     convolution stride x
   * @param[in]       stride_y     convolution stride y
   * @param[in]       bias         pointer to bias
   * @param[in]       bias_shift   amount of left-shift for bias
   * @param[in]       out_shift    amount of right-shift for output
   * @param[in,out]   Im_out       pointer to output tensor
   * @param[in]       dim_im_out_x output tensor dimension x
   * @param[in]       dim_im_out_y output tensor dimension y
   * @param[in,out]   bufferA      pointer to buffer space for input
   * @return          none.
   *
   * This function implement convolution with 1x1 kernel size (i.e., dim_kernel_x=1
   * and dim_kernel_y=1). It can be used for
   * second half of MobileNets after depthwise separable convolution.
   *
   * This function is the version with full list of optimization tricks, but with
   * some contraints:
   *   ch_im_in is multiple of 4
   *   ch_im_out is multiple of 2
   */
void csky_vdsp2_convolve_1x1_HWC_q7_fast(const q7_t * Im_in,
                                         const uint16_t dim_im_in_x,
                                         const uint16_t dim_im_in_y,
                                         const uint16_t ch_im_in,
                                         const q7_t * wt,
                                         const uint16_t ch_im_out,
                                         const q7_t * bias,
                                         const uint16_t bias_shift,
                                         const uint16_t out_shift,
                                         q7_t * Im_out,
                                         const uint16_t dim_im_out_x,
                                         const uint16_t dim_im_out_y,
                                         q15_t * bufferA);

  /**
   * @brief Q7 version of convolution for RGB image
   * @param[in]       Im_in       pointer to input tensor
   * @param[in]       dim_im_in   input tensor dimention
   * @param[in]       ch_im_in    number of input tensor channels
   * @param[in]       wt          pointer to kernel weights
   * @param[in]       ch_im_out   number of filters, i.e., output tensor channels
   * @param[in]       dim_kernel  filter kernel size
   * @param[in]       padding     padding sizes
   * @param[in]       stride      convolution stride
   * @param[in]       bias        pointer to bias
   * @param[in]       bias_shift  amount of left-shift for bias
   * @param[in]       out_shift   amount of right-shift for output
   * @param[in,out]   Im_out      pointer to output tensor
   * @param[in]       dim_im_out  output tensor dimension
   * @param[in,out]   bufferA     pointer to buffer space for input
   * @return          none.
   *
   * This kernel is written exclusively for convolution with ch_im_in
   * equals 3. This applies on the first layer of CNNs which has input
   * image with RGB format.
   */

void csky_vdsp2_convolve_HWC_q7_RGB(const q7_t * Im_in,
                                     const uint16_t dim_im_in,
                                     const q7_t * wt,
                                     const uint16_t ch_im_out,
                                     const uint16_t dim_kernel,
                                     const uint16_t padding,
                                     const uint16_t stride,
                                     const q7_t * bias,
                                     const uint16_t bias_shift,
                                     const uint16_t out_shift,
                                     q7_t * Im_out,
                                     const uint16_t dim_im_out,
                                     q15_t * bufferA);


  /**
   * @brief Q7 depthwise separable convolution function
   * @param[in]       Im_in       pointer to input tensor
   * @param[in]       dim_im_in   input tensor dimention
   * @param[in]       ch_im_in    number of input tensor channels
   * @param[in]       wt          pointer to kernel weights
   * @param[in]       ch_im_out   number of filters, i.e., output tensor channels
   * @param[in]       dim_kernel  filter kernel size
   * @param[in]       padding     padding sizes
   * @param[in]       stride      convolution stride
   * @param[in]       bias        pointer to bias
   * @param[in]       bias_shift  amount of left-shift for bias
   * @param[in]       out_shift   amount of right-shift for output
   * @param[in,out]   Im_out      pointer to output tensor
   * @param[in]       dim_im_out  output tensor dimension
   * @param[in,out]   bufferA     pointer to buffer space for input
   * @return          none.
   *
   * This function is the version with full list of optimization tricks, but with
   * some contraints:
   *   ch_im_in is multiple of 2
   *   ch_im_out is multiple of 2
   */

void csky_vdsp2_depthwise_separable_conv_HWC_q7(const q7_t * Im_in,
                                                 const uint16_t dim_im_in,
                                                 const uint16_t ch_im_in,
                                                 const q7_t * wt,
                                                 const uint16_t ch_im_out,
                                                 const uint16_t dim_kernel,
                                                 const uint16_t padding,
                                                 const uint16_t stride,
                                                 const q7_t * bias,
                                                 const uint16_t bias_shift,
                                                 const uint16_t out_shift,
                                                 q7_t * Im_out,
                                                 const uint16_t dim_im_out,
                                                 q15_t * bufferA);

  /**
   * @brief Q7 depthwise separable convolution function (non-square shape)
   * @param[in]       Im_in         pointer to input tensor
   * @param[in]       dim_im_in_x   input tensor dimention x
   * @param[in]       dim_im_in_y   input tensor dimention y
   * @param[in]       ch_im_in      number of input tensor channels
   * @param[in]       wt            pointer to kernel weights
   * @param[in]       ch_im_out     number of filters, i.e., output tensor channels
   * @param[in]       dim_kernel_x  filter kernel size x
   * @param[in]       dim_kernel_y  filter kernel size y
   * @param[in]       padding_x     padding sizes x
   * @param[in]       padding_y     padding sizes y
   * @param[in]       stride_x      convolution stride x
   * @param[in]       stride_y      convolution stride y
   * @param[in]       bias          pointer to bias
   * @param[in]       bias_shift    amount of left-shift for bias
   * @param[in]       out_shift     amount of right-shift for output
   * @param[in,out]   Im_out        pointer to output tensor
   * @param[in]       dim_im_out_x  output tensor dimension x
   * @param[in]       dim_im_out_y  output tensor dimension y
   * @param[in,out]   bufferA       pointer to buffer space for input
   * @return          none.
   *
   * This function is the version with full list of optimization tricks, but with
   * some contraints:
   *   ch_im_in is multiple of 2
   *   ch_im_out is multiple of 2
   */
void csky_vdsp2_depthwise_separable_conv_HWC_q7_nonsquare(const q7_t * Im_in,
                                                 const uint16_t dim_im_in_x,
                                                 const uint16_t dim_im_in_y,
                                                 const uint16_t ch_im_in,
                                                 const q7_t * wt,
                                                 const uint16_t ch_im_out,
                                                 const uint16_t dim_kernel_x,
                                                 const uint16_t dim_kernel_y,
                                                 const uint16_t padding_x,
                                                 const uint16_t padding_y,
                                                 const uint16_t stride_x,
                                                 const uint16_t stride_y,
                                                 const q7_t * bias,
                                                 const uint16_t bias_shift,
                                                 const uint16_t out_shift,
                                                 q7_t * Im_out,
                                                 const uint16_t dim_im_out_x,
                                                 const uint16_t dim_im_out_y,
                                                 q15_t * bufferA);


  /**
   * @brief Q7 basic fully-connected layer function
   * @param[in]       pV          pointer to input vector
   * @param[in]       pM          pointer to matrix weights
   * @param[in]       dim_vec     length of the vector
   * @param[in]       num_of_rows number of rows in weight matrix
   * @param[in]       bias_shift  amount of left-shift for bias
   * @param[in]       out_shift   amount of right-shift for output
   * @param[in]       bias        pointer to bias
   * @param[in,out]   pOut        pointer to output vector
   * @return          none.
   */

void csky_vdsp2_fully_connected_q7(const q7_t * pV,
                                    const q7_t * pM,
                                    const uint16_t dim_vec,
                                    const uint16_t num_of_rows,
                                    const uint16_t bias_shift,
                                    const uint16_t out_shift,
                                    const q7_t * bias,
                                    q7_t * pOut);


  /**
   * @brief Q15 basic fully-connected layer function
   * @param[in]       pV          pointer to input vector
   * @param[in]       pM          pointer to matrix weights
   * @param[in]       dim_vec     length of the vector
   * @param[in]       num_of_rows number of rows in weight matrix
   * @param[in]       bias_shift  amount of left-shift for bias
   * @param[in]       out_shift   amount of right-shift for output
   * @param[in]       bias        pointer to bias
   * @param[in,out]   pOut        pointer to output vector
   * @return          none.
   *
   */

void csky_vdsp2_fully_connected_q15(const q15_t * pV,
                                     const q15_t * pM,
                                     const uint16_t dim_vec,
                                     const uint16_t num_of_rows,
                                     const uint16_t bias_shift,
                                     const uint16_t out_shift,
                                     const q15_t * bias,
                                     q15_t * pOut);


  /**
   * @brief Mixed Q15-Q7 fully-connected layer function
   * @param[in]       pV          pointer to input vector
   * @param[in]       pM          pointer to matrix weights
   * @param[in]       dim_vec     length of the vector
   * @param[in]       num_of_rows number of rows in weight matrix
   * @param[in]       bias_shift  amount of left-shift for bias
   * @param[in]       out_shift   amount of right-shift for output
   * @param[in]       bias        pointer to bias
   * @param[in,out]   pOut        pointer to output vector
   * @return          none.
   *
   */

void csky_vdsp2_fully_connected_mat_q7_vec_q15(const q15_t * pV,
                                                const q7_t * pM,
                                                const uint16_t dim_vec,
                                                const uint16_t num_of_rows,
                                                const uint16_t bias_shift,
                                                const uint16_t out_shift,
                                                const q7_t * bias,
                                                q15_t * pOut);



  /**
   * @brief Q7 RELU function
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @return none.
   */

void csky_vdsp2_relu_q7(q7_t * data, uint16_t size);

  /**
   * @brief Q15 RELU function
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @return none.
   */

void csky_vdsp2_relu_q15(q15_t * data, uint16_t size);

  /**
   * @brief Q7 neural network activation function using direct table look-up
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @param[in]       int_width   bit-width of the integer part, assume to be smaller than 3
   * @param[in]       type        type of activation functions
   * @return none.
   */

void csky_vdsp2_nn_activations_direct_q7(q7_t * data, uint16_t size,
                                   uint16_t int_width,
                                   csky_vdsp2_nn_activation_type type);

  /**
   * @brief Q15 neural network activation function using direct table look-up
   * @param[in,out]   data        pointer to input
   * @param[in]       size        number of elements
   * @param[in]       int_width   bit-width of the integer part, assume to be smaller than 3
   * @param[in]       type        type of activation functions
   * @return none.
   */

void csky_vdsp2_nn_activations_direct_q15(q15_t * data, uint16_t size,
                                    uint16_t int_width,
                                    csky_vdsp2_nn_activation_type type);

  /**
   * @brief Q7 max pooling function
   * @param[in]       Im_in       pointer to input tensor
   * @param[in]       dim_im_in   input tensor dimention
   * @param[in]       ch_im_in    number of input tensor channels
   * @param[in]       dim_kernel  filter kernel size
   * @param[in]       padding     padding sizes
   * @param[in]       stride      convolution stride
   * @param[in]       dim_im_out  output tensor dimension
   * @param[in,out]   bufferA     pointer to buffer space for input
   * @param[in,out]   Im_out      pointer to output tensor
   * @return none.
   *
   */

void csky_vdsp2_maxpool_q7_HWC(q7_t * Im_in,
                         const uint16_t dim_im_in,
                         const uint16_t ch_im_in,
                         const uint16_t dim_kernel,
                         const uint16_t padding,
                         const uint16_t stride,
                         const uint16_t dim_im_out,
                         q7_t * bufferA,
                         q7_t * Im_out);

  /**
   * @brief Q7 average pooling function
   * @param[in]       Im_in       pointer to input tensor
   * @param[in]       dim_im_in   input tensor dimention
   * @param[in]       ch_im_in    number of input tensor channels
   * @param[in]       dim_kernel  filter kernel size
   * @param[in]       padding     padding sizes
   * @param[in]       stride      convolution stride
   * @param[in]       dim_im_out  output tensor dimension
   * @param[in,out]   bufferA     pointer to buffer space for input
   * @param[in,out]   Im_out      pointer to output tensor
   * @return none.
   *
   */

void csky_vdsp2_avepool_q7_HWC(q7_t * Im_in,
                         const uint16_t dim_im_in,
                         const uint16_t ch_im_in,
                         const uint16_t dim_kernel,
                         const uint16_t padding,
                         const uint16_t stride,
                         const uint16_t dim_im_out,
                         q7_t * bufferA,
                         q7_t * Im_out);


  /**
   * @brief Q7 softmax function
   * @param[in]       vec_in      pointer to input vector
   * @param[in]       dim_vec     input vector dimention
   * @param[out]      p_out       pointer to output vector
   * @return none.
   *
   */

void csky_vdsp2_softmax_q7(const q7_t *vec_in, const uint16_t dim_vec, q7_t *p_out);

  /**
   * @brief Q15 softmax function
   * @param[in]       vec_in      pointer to input vector
   * @param[in]       dim_vec     input vector dimention
   * @param[out]      p_out       pointer to output vector
   * @return none.
   *
   */

void csky_vdsp2_softmax_q15(const q15_t *vec_in, const uint16_t dim_vec,
                      q15_t *p_out);

#ifdef __cplusplus
}
#endif

#endif
