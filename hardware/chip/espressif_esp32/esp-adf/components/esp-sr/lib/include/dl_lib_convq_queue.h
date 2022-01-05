// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef DL_LIB_CONVQ_QUEUE_H
#define DL_LIB_CONVQ_QUEUE_H


#include "dl_lib_matrixq.h"
#include "dl_lib_conv_queue.h"

//fixed-point convolution FIFO queue. 
typedef struct {
    int n;           /*< the length of queue */
    int c;           /*< the channel number of queue element*/
    int front;       /*< the front(top) position of queue */
    int flag;        /*< not used */
    int exponent;    /*< The values in items should be multiplied by pow(2,exponent) 
                         to get the real values */
    qtp_t *itemq;    /*< Pointer to item array */
} dl_convq_queue_t;

/**
 * @brief Allocate a fixed-point convolution queue
 *
 * @param n     The length of queue
 * @param c     The channel number of elements in the queue
 * @return      The convolution queue, or NULL if out of memory
 */
dl_convq_queue_t *dl_convq_queue_alloc(int n, int c);

void dl_convq_to_matrix2dq(dl_convq_queue_t *cq, dl_matrix2dq_t* out, int row);

/**
 * @brief Free a fixed-point convolution queue
 *
 * @param cq     The fixed-point convolution queue to free
 */
void dl_convq_queue_free(dl_convq_queue_t *cq);

/**
 * @brief Move the front pointer of queue forward, 
          the First(oldest) element become the last(newest) element, 
 *
 * @param cq    Input fixed-point convolution queue
 * @return      Pointer of oldest element  
 */
qtp_t *dl_convq_queue_pop(dl_convq_queue_t *cq);

/**
 * @brief  Remove the oldest element, then insert the input element at the end of queue
 *
 * @param cq     Input fixed-point convolution queue
 * @param item   The new element
 */
void dl_convq_queue_push(dl_convq_queue_t *cq, dl_matrix2dq_t *a, int shift);

/**
 * @brief  Insert the float-point element at the end of queue.
 *         The precision of fixed-point numbers is described by the Qm.f notation,  
 *
 * @param cq     Input fixed-point convolution queue
 * @param item   The float-point element
 * @param m_bit  The number of integer bits including the sign bits
 * @param f_bit  The number of fractional bits
 */
void dl_convq_queue_push_by_qmf(dl_convq_queue_t *cq, fptp_t* item, int m_bit, int f_bit);

/**
 * @brief   Get the pointer of element in the queue by offset
 *
 * @param cq      Input fixed-point convolution queue
 * @param offset  Offset from the front of the queue
 * @return        Pointer of the element
 */
qtp_t *dl_get_queue_itemq(dl_convq_queue_t *cq, int offset);

/**
 * @brief   Does a tanh operation on the one of element in the convolution queue.
 *          Gets the pointer of element in the convolution queue by offset, and does a 
 *          tanh operation by this pointer, then return the pointer 
 *
 * @param cq      Input fixed-point convolution queue
 * @param offset  Offset from the front of the queue
 * @return        Pointer of the element
 */
void dl_tanh_convq(dl_convq_queue_t *cq, int last_num);

/**
 * @brief   Does a relu operation on the one of element in the convolution queue.
 *          Gets the pointer of element in the convolution queue by offset, and does a 
 *          relu operation by this pointer, then return the pointer 
 *
 * @param cq      Input fixed-point convolution queue
 * @param offset  Offset from the front of the queue
 * @return        Pointer of the element
 */
void dl_relu_convq(dl_convq_queue_t *cq, fptp_t clip, int last_num);

/**
 * @brief   Does a softmax operation on the one of element in the convolution queue.
 *          Gets the pointer of element in the convolution queue by offset, input data
            stay as it is. Results are saved into the *out* array. 
 *
 * @param cq      Input fixed-point convolution queue
 * @param offset  Offset from the front of the queue
 * @param out     Old array to re-use. Passing NULL will allocate a new matrix.
 * @return        softmax results
 */
fptp_t * dl_softmax_step_q(dl_convq_queue_t *cq, int offset, fptp_t *out);

/**
 * @brief Fast and quantised implement for 1D atrous convolution (a.k.a. convolution with holes or dilated convolution)
 *        based on convolution queue.
 *
 * @Warning All input and output convolution queue and matrix should be allocated. The return pointer
 *          is last element of output queue and should not be freed separately.
 *
 * @param in       Input fixed-point convolution queue
 * @param out      Output fixed-point convolution queue
 * @param rate     A positive int, the stride with which we sample input value 
 * @param size     A positive int, the size of 1D-filter
 * @param kernel   The kernel matrix of filter
 * @param bias     The bias matrix of filter. Can be NULL if a bias of 0 is required.
 * @param shift    Shift ratio used in dot operation between two 16-bit fixed point vector 
 * @return         The result of atrous convolution
 */
qtp_t *dl_atrous_conv1dq(dl_convq_queue_t *in, dl_convq_queue_t *out, int rate, int size,
                             dl_matrix2dq_t* kernel, dl_matrix2dq_t* bias, int shift);
qtp_t *dl_atrous_conv1dq_steps(dl_convq_queue_t *in, dl_convq_queue_t *out, int rate, int size,
                             dl_matrix2dq_t* kernel, dl_matrix2dq_t* bias, int shift, int offset);
/**
 * @brief Fast implement of dilation layer as follows
 *
 *               |-> [gate(sigmoid)] -|        
 *      input -  |                    |-> (*) - output
 *               |-> [filter(tanh)]  -|   
 *
 * @Warning All input and output convolution queue and matrix should be allocated. The return pointer
 *          is last element of output queue and should not be freed separately.
 *
 * @param in              Input fixed-point convolution queue
 * @param out             Output fixed-point convolution queue
 * @param rate            A positive int, the stride with which we sample input value 
 * @param size            A positive int, the size of 1D-filter
 * @param filter_kernel   The kernel matrix of filter
 * @param filter_bias     The bias matrix of filter. Can be NULL if a bias of 0 is required.
 * @param gate_kernel     The kernel matrix of gate
 * @param gate_bias       The bias matrix of gate. Can be NULL if a bias of 0 is required.
 * @filter_shift          Shift ratio used in filter operation between two 16-bit fixed point vector
 * @gate_shift            Shift ratio used in gate operation between two 16-bit fixed point vector
 * @return                The result of dilation layer
 */
qtp_t *dl_dilation_layerq(dl_convq_queue_t *in, dl_convq_queue_t *out, int rate, int size,
                          dl_matrix2dq_t* filter_kernel, dl_matrix2dq_t* filter_bias,
                          dl_matrix2dq_t* gate_kernel, dl_matrix2dq_t* gate_bias, 
                          int filter_shift, int gate_shift);

dl_matrix2dq_t *dl_basic_lstm_layer1_q(const dl_convq_queue_t *in, dl_matrix2dq_t *state_c, dl_matrix2dq_t *state_h,
   const dl_matrix2dq_t *weight, const dl_matrix2dq_t *bias, int step, int shift);
void test_atrous_convq(int size, int rate, int in_channel, int out_channel);

dl_conv_queue_t *dl_convq_queue_add(dl_convq_queue_t *cq1, dl_convq_queue_t *cq2);

#endif