#ifndef _OPEN_DEEP_LEARNING_API_H_
#define _OPEN_DEEP_LEARNING_API_H_

#include <stddef.h>
#include <stdint.h>
#include "uai_odla_adapt.h"

/*! \mainpage Open Deep Learning API
 */

/*! \file
 * \details This file defines the Open Deep Learning APIs (C interface)
 */

#ifdef __cplusplus
extern "C" {
#endif

//! \brief An abstract wrapper of a computation definition
typedef struct _odla_Computation* odla_computation;

//! \brief An abstract wrapper that can execute a computation on the platform
typedef struct _odla_Session* odla_session;

//! \brief A wrapper for computation input / output.
typedef struct _odla_Value* odla_value;

//! \brief Status code
typedef int odla_status;

#define OLDA_MAX_DIMENSION 12

//! \brief Dimension of data
typedef struct {
  unsigned size;
  size_t dims[OLDA_MAX_DIMENSION];
} _odla_dims;

typedef _odla_dims* odla_dims;

//! \brief Data types of element
typedef enum {
  FLOAT,
  DOUBLE,
  FLOAT16,
  INT8,
  INT32,
  UINT32,
  INT64,
  UINT64
} odla_element_type;

//! \brief Memory Layout of data
typedef enum {
  CHANNELS_FIRST, // assumes (N)C(DH)W
  CHANNELS_LAST,  // assumes (N)(DH)WC
  SIO,            // spatital dimmension, input and output channels
  SOI,            // spatial first, output and input channels
  OIS,            // output channls, input channels and spatial dimensions
  IOS,            // input channles, output channels and spatial dimensions
} odla_layout;

//! \brief Create an empty computation object
/*! \return A pointer to the computation object
 */
odla_computation odla_CreateComputation(odla_helper_func helper_func);

//! \brief Creats a session object
/*! \param comp A pointer to a computation object
    \return A poitner to a session object
*/
odla_session odla_CreateSession(odla_computation comp);

//! \brief Release a session object
/*! \param sess the pointer to the session to be released
 */
void odla_DestroySession(odla_session sess);

//! \brief Release a computation object
/*! \param comp the pointer to the computation to be release
 */
void odla_DestroyComputation(odla_computation comp);

//! \brief Execute a computation task
/*! Start the computation on the device and make sure result is filled into
    output buffers as specified by bindings. After the execution, temporary
    resources should be release and device should be ready for next run.
 *! \param session the pointer to the session
    \return status code
 */
odla_status odla_Execute(odla_session session);

//! \brief Check if the status is OK or not.
/*! \param ec the status code
    \return non-zero if the status is OK.
*/
int olda_IsStatusOK(odla_status status);

//! \brief Get the text description of an status code
/*! \param ec the status code
    \param len the length of buffer
    \param buf the detailed info about the status
*/
void odla_GetStatusDescription(odla_status status, size_t len, char* buf);

//! \brief Create an input object
/*!
  \param comp the computation object
  \param name a unique name to identify the input, which is also be used for
              binding.
  \param dims the shape of input data \return the result object
*/
odla_value odla_AddArgument(odla_computation comp, const char* name,
                            odla_element_type type, odla_dims dims);

//! \brief Bind the memory pointer to Argument
/*!
  \param session the session object
  \param name the name for the argument object
  \param type the element type
  \param ptr the pointer to the input data
  */
void odla_BindArgument(odla_session session, const char* name,
                       odla_element_type type, odla_dims dims, const void* ptr);

//! \brief Create a constant object
/*!
  \param comp the computation object
  \param type the element type
  \param ptr the raw pointer for constant data
  \param dims the shape of input data
  \return the result object
*/
odla_value odla_AddConstant(odla_computation comp, odla_element_type type,
                            odla_dims dims, const void* ptr);

//! \brief Mark a value as output
/*!
  \param comp the computation object
  \param name an unique name to identify an output value
  \param val the output object
*/
void odla_SetOutput(odla_computation comp, const char* name,
                    const odla_value val);

//! \brief Pre-launch binding the pointer to the output value.
/*! If the platform requires binding output buffer before launching the
  computation, this API provides the interface.
  \param session the session object
  \param name the name that specifies the output value
  \param ptr the pointer to the output data buffer
*/
void odla_BindOutput(odla_session session, const char* name,
                     odla_element_type type, odla_dims dims, void* ptr);

//! \brief Post-launch copying of result data.
/*! If the platform requires copying results to host memory after the
  computation, this API provides the interface.
  \param sess Session object
  \param name  the name that specifies the output value
  \param ptr the pointer to the host memory buffer
*/
void odla_CopyResultToHost(odla_session sess, const char* name, void* ptr);

//! Addition (with implicit broadcasting)
/*!
  \param comp the computation object
  \param lhs the first operand
  \param rhs the second operand
  \param dims_lhs the dimensions of the first operand
  \param dims_rhs the dimensions of the second operand
  \return the result value
 */
odla_value odla_Add(odla_computation comp, const odla_value lhs,
                    const odla_value rhs, odla_dims dims_lhs,
                    odla_dims dims_rhs);

//! \brief Pad the input
/*!
  \param comp the computation object
  \param type the element type
  \param input the input operand
  \param input_dims the input shape
  \param padding_front the padding amount applied to the start of input
  \param padding_back the padding amoutt appleid to the end of input
  \param output_dims the result shape. output_dim.dims[i] = padding_front[i] +
  input_dims.dims[i] + padding_back[i]
  \return the result value
*/
odla_value odla_Pad(odla_computation comp, odla_element_type type,
                    const odla_value input, odla_dims input_dims,
                    const int* padding_front, const int* padding_back,
                    odla_dims output_dims);

//! \brief N-dimensional Convolution
/*!
  \param comp the computation object
  \param type the element type
  \param input_dims the logical shape of input specified by [batch, channel,
  spatial dimensions...]
  \param input_layout the storage layout of input in memory
  \param input the input value
  \param kernel_dims the logical shape of kernel specified by [oc, ic, spatial
  dimensions...]
  \param kernel_layout the storage layout of kernel in memory
  \param kernel the kernel value
  \param strides the strides. The size should be the same as spatial dims.
  \param dilations the dilations. Same number of values as `strides`.
  \param output_dims the logical shape of output (same layout as input). If the
  the dimension is zero, the implemention should infer the output shape.
  \param paddings_front paddings applied to start of each spatial dim
  \param paddings_back paddings applied to end of each spatial dim
  \return the result value
*/
odla_value odla_Convolution(odla_computation comp, odla_element_type type,
                            odla_dims input_dims, odla_layout input_layout,
                            odla_value input, odla_dims kernel_dims,
                            odla_layout kernel_layout, odla_value kernel,
                            const unsigned* strides, const unsigned* dilations,
                            const unsigned* paddings_front,
                            const unsigned* paddings_back,
                            odla_dims output_dims);

//! \brief Batch Normalization
/*!
  \param comp the computation object
  \param type the element type
  \param input_dims the logical shape of input specified by [batch, channel,
  ...]
  \param input_layout the storage layout of input in memory
  \param input the input value
  \param mean the mean value
  \param var the variance value
  \param epsilon the epsilon
  \param scale optional scale value. Default is nullptr.
  \param offset optional offset value. Default is nullptr.
  \param scalar_scale If `scale` is nullptr, this value will be
  used. Default is 1.0
  \param scalar_offset if `offset` is nullptr, this value will be used. Default
  is 0.
  \return the result value
*/
odla_value odla_BatchNormalization(
    odla_computation comp, odla_element_type type, odla_dims input_dims,
    odla_layout input_layout, odla_value input, odla_value mean, odla_value var,
    float epsilon, odla_value scale, odla_value offset,
    float scalar_scale, float scalar_offset);

//! \brief Max Pooling
/*!
  \param comp the computation object
  \param input the input value
  \return the result value
*/
odla_value odla_MaxPooling(odla_computation comp, odla_element_type type,
                           odla_dims input_dims, odla_layout layout,
                           odla_value input, const unsigned* window_dims,
                           const unsigned* strides,
                           const unsigned* paddings_front,
                           const unsigned* paddings_back,
                           odla_dims output_dims);

//! \brief Relu
/*!
  \param comp the computation object
  \param type the element type
  \param dims the shape of input
  \param input the input value
  \return the result value
*/
odla_value odla_Relu(odla_computation comp, odla_element_type type,
                     odla_dims dims, odla_value input);

//! \brief Softmax
/*!
  \param comp the computation object
  \param type the element type
  \param dims the shape of input
  \param input the input value
  \return the result value
*/
odla_value odla_Softmax(odla_computation comp, odla_element_type type,
                        odla_dims dims, odla_value input);

//! \brief Reduce Mean
/*!
  \param comp the computation object
  \param type the element type
  \param dims the shape of input
  \param input the input value
  \param num_of_axes nubmer of axes to reduce
  \param axes the axes to reduce
  \param output_dims the output shape
  \return the result value
*/
odla_value odla_ReduceMean(odla_computation comp, odla_element_type type,
                           odla_dims dims, odla_value input, size_t num_of_axes,
                           const unsigned* axes, odla_dims output_dims);

//! \brief GEMM
/*!
  \param comp the computation object
  \param type the element type
  \param lhs_dims the shape of lhs input
  \param lhs the left operand
  \param lhs_tranpose indicates if lhs operand is transposed or not
  \param rhs_dims the shape of rhs input
  \param rhs the right operand
  \param rhs_tranpose indicates if rhs operand is transposed or not
  \param output_dims the output shape
  \return the result value
*/
odla_value odla_Gemm(odla_computation comp, odla_element_type type,
                     odla_dims lhs_dims, odla_value lhs, bool lhs_transpose,
                     odla_dims rhs_dims, odla_value rhs, bool rhs_transpose,
                     odla_dims output_dims);
#ifdef __cplusplus
} // C extern
#endif

#endif // _OPEN_DEEP_LEARNING_API_H_
