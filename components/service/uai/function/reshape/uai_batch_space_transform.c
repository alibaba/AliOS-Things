/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include "uai_batch_space_transform.h"
#include "string.h"

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

static inline int dims_offset(const int16_t *dims, int i0, int i1, int i2, int i3)
{
	UAI_VALID_COND_CHECK_RET(i0 >= 0 && i0 < dims[0], 0);
	UAI_VALID_COND_CHECK_RET(i1 >= 0 && i1 < dims[1], 0);
	UAI_VALID_COND_CHECK_RET(i2 >= 0 && i2 < dims[2], 0);
	UAI_VALID_COND_CHECK_RET(i3 >= 0 && i3 < dims[3], 0);
	return ((i0 * dims[1] + i1) * dims[2] + i2) * dims[3] + i3;
}

#ifdef UAI_ODLA_SUPPORT
int uai_space_to_batch_nd(uai_tensor_s *input, uai_tensor_s *block, uai_tensor_s *paddings, uai_tensor_s *output)
{
	int16_t input_size[4] = { 0 };
	int16_t output_size[4] = { 0 };
	int16_t block_size[4] = { 0 };

	UAI_VALID_PTR_CHECK_INT(input, UAI_FAIL);
	UAI_VALID_PTR_CHECK_INT(block, UAI_FAIL);
	UAI_VALID_PTR_CHECK_INT(paddings, UAI_FAIL);
	UAI_VALID_PTR_CHECK_INT(output, UAI_FAIL);

	input_size[0] = input->dims.dims[0];
	input_size[1] = input->dims.dims[1];
	input_size[2] = input->dims.dims[2];
	input_size[3] = input->dims.dims[3];

	block_size[0] = block->dims.dims[0];
	block_size[1] = block->dims.dims[1];
	block_size[2] = block->dims.dims[2];
	block_size[3] = block->dims.dims[3];

	for (int dim = 0; dim < 3; ++dim) {
		int final_dim_size = (input_size[dim + 1] + paddings->buffer[dim * 2] +
			paddings->buffer[dim * 2 + 1]);
		output_size[dim + 1] = final_dim_size / block_size[dim + 1];
	}

	output->dims.dims[0] = input->dims.dims[0] * block->dims.dims[1] * block->dims.dims[2];
	output_size[0] = output->dims.dims[0];
	output->dims.dims[1] = output_size[1];
	output->dims.dims[2] = output_size[2];
	output->dims.dims[3] = input->dims.dims[3];
	output_size[3] = output->dims.dims[3];

	output->size = output->dims.dims[0] * output->dims.dims[1] * output->dims.dims[2] * output->dims.dims[3];
	output->buffer = (int8_t *)uai_malloc(output->size);
	if (output->buffer != NULL) {
		const int depth = input->dims.dims[3];
		const int input_width = input->dims.dims[2];
		const int input_height = input->dims.dims[1];
		const int input_batch_size = input->dims.dims[0];

		const int output_width = output->dims.dims[2];
		const int output_height = output->dims.dims[1];
		const int output_batch_size = output->dims.dims[0];

		const int block_shape_height = block->dims.dims[1];
		const int block_shape_width = block->dims.dims[2];

		const int padding_top = paddings->buffer[0];
		const int padding_left = paddings->buffer[2];

		// For uint8 quantized, the correct padding "zero value" is the output offset.
		const int32_t pad_value = 0;

		for (int out_b = 0; out_b < output_batch_size; ++out_b) {
			int input_batch = out_b % input_batch_size;
			int shift_w = (out_b / input_batch_size) % block_shape_width;
			int shift_h = (out_b / input_batch_size) / block_shape_width;
			for (int out_h = 0; out_h < output_height; ++out_h) {
				for (int out_w = 0; out_w < output_width; ++out_w) {
					int8_t* out = output->buffer + dims_offset(output_size, out_b, out_h, out_w, 0);
					if (out_h * block_shape_height + shift_h < padding_top ||
						out_h * block_shape_height + shift_h >=
						padding_top + input_height ||
						out_w * block_shape_width + shift_w < padding_left ||
						out_w * block_shape_width + shift_w >= padding_left + input_width) {

						memset(out, pad_value, depth * sizeof(int8_t));
					}
					else {
						const int8_t* in = input->buffer +
							dims_offset(input_size, input_batch,
							(out_h * block_shape_height + shift_h) - padding_top,
								(out_w * block_shape_width + shift_w) - padding_left, 0);
						memcpy(out, in, depth * sizeof(int8_t));
					}
				}
			}
		}
		return UAI_SUCCESS;
	}
	return UAI_FAIL;
}

static inline void GetIndexRange(int spatial_index_dim, int block_shape_dim,
	int input_dim, int output_dim, int* start_index,
	int* end_index)
{
	/* (*start_index) * block_shape_dim is effectively rounded up to the next
	 multiple of block_shape_dim by the integer division. */
	*start_index =
		max(0, (-spatial_index_dim + block_shape_dim - 1) / block_shape_dim);
	/* Similarly, (*end_index) * block_shape_dim is rounded up too (note that
	 end_index is exclusive). */
	*end_index = min(
		input_dim,
		(output_dim - spatial_index_dim + block_shape_dim - 1) / block_shape_dim);
}

int uai_batch_to_space_nd(uai_tensor_s *input, uai_tensor_s *block, uai_tensor_s *crops, uai_tensor_s *output)
{
	int16_t input_size[4] = { 0 };
	int16_t output_size[4] = { 0 };

	UAI_VALID_PTR_CHECK_INT(input, UAI_FAIL);
	UAI_VALID_PTR_CHECK_INT(block, UAI_FAIL);
	UAI_VALID_PTR_CHECK_INT(crops, UAI_FAIL);
	UAI_VALID_PTR_CHECK_INT(output, UAI_FAIL);

	if (crops->size != 4) {
		UAI_LOGE("crops size is not 4, not supported!");
		return UAI_FAIL;
	}
	output->dims.dims[0] = input->dims.dims[0] / (block->dims.dims[1] * block->dims.dims[2]);
	output->dims.dims[1] = input->dims.dims[1] * block->dims.dims[1] - crops->buffer[0] - crops->buffer[1];
	output->dims.dims[2] = input->dims.dims[2] * block->dims.dims[2] - crops->buffer[2] - crops->buffer[3];
	output->dims.dims[3] = input->dims.dims[3];

	input_size[0] = input->dims.dims[0];
	input_size[1] = input->dims.dims[1];
	input_size[2] = input->dims.dims[2];
	input_size[3] = input->dims.dims[3];

	output_size[0] = output->dims.dims[0];
	output_size[1] = output->dims.dims[1];
	output_size[2] = output->dims.dims[2];
	output_size[3] = output->dims.dims[3];

	output->size = output->dims.dims[0] * output->dims.dims[1] * output->dims.dims[2] * output->dims.dims[3];
	output->buffer = (int8_t *)uai_malloc(output->size);
	if (output->buffer != NULL) {
		const int output_width = output->dims.dims[2];
		const int output_height = output->dims.dims[1];
		const int output_batch_size = output->dims.dims[0];

		const int depth = input->dims.dims[3];
		const int input_width = input->dims.dims[2];
		const int input_height = input->dims.dims[1];
		const int input_batch_size = input->dims.dims[0];

		const int block_shape_width = block->dims.dims[2];
		const int block_shape_height = block->dims.dims[1];
		const int crops_top = crops->buffer[0];
		const int crops_left = crops->buffer[2];

		for (int in_batch = 0; in_batch < input_batch_size; ++in_batch) {
			const int out_batch = in_batch % output_batch_size;
			const int spatial_offset = in_batch / output_batch_size;

			int in_h_start = 0;
			int in_h_end = 0;
			/* GetIndexRange ensures start and end indices are in [0, output_height). */
			GetIndexRange(spatial_offset / block_shape_width - crops_top,
				block_shape_height, input_height, output_height, &in_h_start,
				&in_h_end);

			for (int in_h = in_h_start; in_h < in_h_end; ++in_h) {
				const int out_h = in_h * block_shape_height +
					spatial_offset / block_shape_width - crops_top;

				int in_w_start = 0;
				int in_w_end = 0;
				/* GetIndexRange ensures start and end indices are in [0, output_width). */
				GetIndexRange(spatial_offset % block_shape_width - crops_left,
					block_shape_width, input_width, output_width, &in_w_start,
					&in_w_end);

				for (int in_w = in_w_start; in_w < in_w_end; ++in_w) {
					const int out_w = in_w * block_shape_width +
						spatial_offset % block_shape_width - crops_left;

					int8_t* out = output->buffer + dims_offset(output_size, out_batch, out_h, out_w, 0);
					const int8_t* in =
						input->buffer + dims_offset(input_size, in_batch, in_h, in_w, 0);
					memcpy(out, in, depth * sizeof(int8_t));
				}
			}
		}
		return UAI_SUCCESS;
	}
	return UAI_FAIL;
}
#endif