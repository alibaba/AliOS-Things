/* Copyright 2019 Google LLC. All Rights Reserved.

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

#include <algorithm>
#include <cstdint>

#include "ruy/check_macros.h"
#include "ruy/kernel_x86.h"
#include "ruy/opt_set.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"

#if RUY_PLATFORM_AVX512 && RUY_OPT(ASM)
#include <immintrin.h>  // IWYU pragma: keep
#endif

namespace ruy {

#if !(RUY_PLATFORM_AVX512 && RUY_OPT(ASM))

void Kernel8bitAvx512(const KernelParams8bit<16, 16>&) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

void Kernel8bitAvx512SingleCol(const KernelParams8bit<16, 16>&) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

void KernelFloatAvx512(const KernelParamsFloat<16, 16>&) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

void KernelFloatAvx512SingleCol(const KernelParamsFloat<16, 16>&) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

#else  // RUY_PLATFORM_AVX512 && RUY_OPT(ASM)

void Kernel8bitAvx512(const KernelParams8bit<16, 16>& params) {
  profiler::ScopeLabel label("Kernel kAvx512 8-bit");

  std::int32_t dst_stride = 0;
  if ((params.dst_type_id == DstTypeId<std::int8_t>::kValue) ||
      (params.dst_type_id == DstTypeId<std::uint8_t>::kValue)) {
    dst_stride = params.dst_stride;
  } else if (params.dst_type_id == DstTypeId<std::int16_t>::kValue) {
    dst_stride = params.dst_stride / sizeof(std::int16_t);
  } else if (params.dst_type_id == DstTypeId<std::int32_t>::kValue) {
    dst_stride = params.dst_stride / sizeof(std::int32_t);
  } else {
    RUY_DCHECK(false);
  }

  const std::int8_t* rhs_col_ptr = params.rhs_base_ptr;
  void* dst_col_ptr = params.dst_base_ptr;

  for (int col = params.start_col; col <= params.last_col; col += 16) {
    const std::int8_t* lhs_col_ptr = params.lhs_base_ptr;
    void* dst_ptr = dst_col_ptr;

    const std::int32_t lhs_zero_point = params.lhs_zero_point;
    const bool has_rhs_sums_offsets =
        (params.flags & RUY_ASM_FLAG_HAS_RHS_SUMS) && lhs_zero_point;
    std::int32_t rhs_sums_offsets[16];
    if (has_rhs_sums_offsets) {
      const __m512i rhs_sums_offset_v =
          _mm512_mullo_epi32(_mm512_set1_epi32(lhs_zero_point),
                             _mm512_loadu_si512(&params.rhs_sums[col]));
      _mm512_storeu_si512(reinterpret_cast<__m512i*>(rhs_sums_offsets),
                          rhs_sums_offset_v);
    }

    for (int row = params.start_row; row <= params.last_row; row += 16) {
      int channel =
          (params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL) ? col : row;
      int multiplier_channel =
          (params.flags & RUY_ASM_FLAG_HAS_PERCHANNEL) ? channel : 0;

      const int residual_rows = std::min(params.dst_rows - row, 16);
      const int residual_cols = std::min(params.dst_cols - col, 16);

      __m512i accum_data_v0;
      __m512i accum_data_v1;
      __m512i accum_data_v2;
      __m512i accum_data_v3;
      __m512i accum_data_v4;
      __m512i accum_data_v5;
      __m512i accum_data_v6;
      __m512i accum_data_v7;
      __m512i accum_data_v8;
      __m512i accum_data_v9;
      __m512i accum_data_va;
      __m512i accum_data_vb;
      __m512i accum_data_vc;
      __m512i accum_data_vd;
      __m512i accum_data_ve;
      __m512i accum_data_vf;

      const __mmask16 row_mask =
          (static_cast<std::uint32_t>(1) << residual_rows) - 1;

      // initial_accum_data will be the initialize of each of the
      // accum_data_* accumulator registers. We compute into it terms that are
      // identical across columns.
      __m512i initial_accum_data = _mm512_set1_epi32(params.prod_zp_depth);

      // In the channels-are-rows case, we can load bias here.
      if ((params.flags & RUY_ASM_FLAG_HAS_BIAS) &&
          !(params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL)) {
        initial_accum_data = _mm512_add_epi32(
            initial_accum_data,
            _mm512_loadu_si512(
                reinterpret_cast<const __m512i*>(params.bias + row)));
      }

      const std::int32_t rhs_zero_point = params.rhs_zero_point;
      if ((params.flags & RUY_ASM_FLAG_HAS_LHS_SUMS) && rhs_zero_point) {
        const __m512i lhs_sums_offset =
            _mm512_mullo_epi32(_mm512_set1_epi32(rhs_zero_point),
                               _mm512_loadu_si512(&params.lhs_sums[row]));
        initial_accum_data =
            _mm512_sub_epi32(initial_accum_data, lhs_sums_offset);
      }

      // Adjustments differing across columns.
      if (has_rhs_sums_offsets) {
        accum_data_v0 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[0]));
        accum_data_v1 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[1]));
        accum_data_v2 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[2]));
        accum_data_v3 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[3]));
        accum_data_v4 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[4]));
        accum_data_v5 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[5]));
        accum_data_v6 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[6]));
        accum_data_v7 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[7]));
        accum_data_v8 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[8]));
        accum_data_v9 = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[9]));
        accum_data_va = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[10]));
        accum_data_vb = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[11]));
        accum_data_vc = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[12]));
        accum_data_vd = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[13]));
        accum_data_ve = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[14]));
        accum_data_vf = _mm512_sub_epi32(
            initial_accum_data, _mm512_set1_epi32(rhs_sums_offsets[15]));
      } else {
        accum_data_v0 = initial_accum_data;
        accum_data_v1 = initial_accum_data;
        accum_data_v2 = initial_accum_data;
        accum_data_v3 = initial_accum_data;
        accum_data_v4 = initial_accum_data;
        accum_data_v5 = initial_accum_data;
        accum_data_v6 = initial_accum_data;
        accum_data_v7 = initial_accum_data;
        accum_data_v8 = initial_accum_data;
        accum_data_v9 = initial_accum_data;
        accum_data_va = initial_accum_data;
        accum_data_vb = initial_accum_data;
        accum_data_vc = initial_accum_data;
        accum_data_vd = initial_accum_data;
        accum_data_ve = initial_accum_data;
        accum_data_vf = initial_accum_data;
      }

      // Finally, in the channels-are-columns case, load bias data here.
      if ((params.flags & RUY_ASM_FLAG_HAS_BIAS) &&
          (params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL)) {
        const __m512i bias_data = _mm512_loadu_si512(
            reinterpret_cast<const __m512i*>(params.bias + col));
        accum_data_v0 = _mm512_add_epi32(
            accum_data_v0,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(0), bias_data));
        accum_data_v1 = _mm512_add_epi32(
            accum_data_v1,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(1), bias_data));
        accum_data_v2 = _mm512_add_epi32(
            accum_data_v2,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(2), bias_data));
        accum_data_v3 = _mm512_add_epi32(
            accum_data_v3,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(3), bias_data));
        accum_data_v4 = _mm512_add_epi32(
            accum_data_v4,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(4), bias_data));
        accum_data_v5 = _mm512_add_epi32(
            accum_data_v5,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(5), bias_data));
        accum_data_v6 = _mm512_add_epi32(
            accum_data_v6,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(6), bias_data));
        accum_data_v7 = _mm512_add_epi32(
            accum_data_v7,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(7), bias_data));
        accum_data_v8 = _mm512_add_epi32(
            accum_data_v8,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(8), bias_data));
        accum_data_v9 = _mm512_add_epi32(
            accum_data_v9,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(9), bias_data));
        accum_data_va = _mm512_add_epi32(
            accum_data_va,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(10), bias_data));
        accum_data_vb = _mm512_add_epi32(
            accum_data_vb,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(11), bias_data));
        accum_data_vc = _mm512_add_epi32(
            accum_data_vc,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(12), bias_data));
        accum_data_vd = _mm512_add_epi32(
            accum_data_vd,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(13), bias_data));
        accum_data_ve = _mm512_add_epi32(
            accum_data_ve,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(14), bias_data));
        accum_data_vf = _mm512_add_epi32(
            accum_data_vf,
            _mm512_permutexvar_epi32(_mm512_set1_epi32(15), bias_data));
      }

      const std::int8_t* lhs_ptr = lhs_col_ptr;
      const std::int8_t* rhs_ptr = rhs_col_ptr;
      for (int d = 0; d < params.depth; d += 4) {
        const __m512i lhs_data = _mm512_loadu_si512(lhs_ptr);
        __m512i rhs_data_8bit = _mm512_loadu_si512(rhs_ptr);

        // Each "int32" is two 16-bit RHS values, sign extended from 8-bit.
        std::int32_t rhs_data[32];
        const __m256i rhs_data_bottom_lane =
            _mm512_castsi512_si256(rhs_data_8bit);
        const __m256i rhs_data_top_lane =
            _mm512_extracti32x8_epi32(rhs_data_8bit, 1);
        const __m512i rhs_16_bit_dup_low =
            _mm512_cvtepi8_epi16(rhs_data_bottom_lane);
        const __m512i rhs_16_bit_dup_high =
            _mm512_cvtepi8_epi16(rhs_data_top_lane);
        // Now that we have cast the RHS data, we store it so that each value
        // can be separately loaded in the accumulation loop.
        _mm512_storeu_si512(reinterpret_cast<__m256i*>(rhs_data),
                            rhs_16_bit_dup_low);
        _mm512_storeu_si512(reinterpret_cast<__m256i*>(rhs_data + 16),
                            rhs_16_bit_dup_high);

        // Take bytes 0, 1, 4, 5, 8, 9, ... and expand to 16-bit.
        const __m512i lhs_16_bit_low =
            _mm512_cvtepi8_epi16(_mm512_cvtepi32_epi16(lhs_data));
        // Take bytes 2, 3, 6, 7, 10, 11, ... and expand to 16-bit.
        const __m512i lhs_16_bit_high = _mm512_cvtepi8_epi16(
            _mm512_cvtepi32_epi16(_mm512_srli_epi32(lhs_data, 16)));

        auto process_column = [=](int col, __m512i& accum) {
          const __m512i rhs_16_bit_dup_low =
              _mm512_set1_epi32(rhs_data[2 * col]);
          const __m512i rhs_16_bit_dup_high =
              _mm512_set1_epi32(rhs_data[2 * col + 1]);

          accum = _mm512_add_epi32(
              accum, _mm512_madd_epi16(lhs_16_bit_low, rhs_16_bit_dup_low));
          accum = _mm512_add_epi32(
              accum, _mm512_madd_epi16(lhs_16_bit_high, rhs_16_bit_dup_high));
        };
        process_column(0, accum_data_v0);
        process_column(1, accum_data_v1);
        process_column(2, accum_data_v2);
        process_column(3, accum_data_v3);
        process_column(4, accum_data_v4);
        process_column(5, accum_data_v5);
        process_column(6, accum_data_v6);
        process_column(7, accum_data_v7);
        process_column(8, accum_data_v8);
        process_column(9, accum_data_v9);
        process_column(10, accum_data_va);
        process_column(11, accum_data_vb);
        process_column(12, accum_data_vc);
        process_column(13, accum_data_vd);
        process_column(14, accum_data_ve);
        process_column(15, accum_data_vf);

        lhs_ptr += 16 * 4;
        rhs_ptr += 16 * 4;
      }

      if (params.dst_type_id != DstTypeId<std::int32_t>::kValue) {
        // The non-per-channel case could equivalently be handled in the per-row
        // or per-column code path. The per-row code path is slightly more
        // efficient so we handle it there.
        const bool per_column_multiplier =
            (params.flags & RUY_ASM_FLAG_HAS_PERCHANNEL) &&
            (params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL);

        __m512i m_vector;
        __m512i e_vector;
        // Does not make use of RUY_ASM_FLAG_NEEDS_LEFT_SHIFT.
        m_vector = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(
            params.multiplier_fixedpoint + multiplier_channel));
        e_vector = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(
            params.multiplier_exponent + multiplier_channel));

        const __m512i m_64bit_low =
            _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(m_vector, 0));
        const __m512i m_64bit_high =
            _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(m_vector, 1));

        const __m512i zero_vector = _mm512_setzero_epi32();
        const __m512i left_shift = _mm512_max_epi32(e_vector, zero_vector);
        const __m512i neg_e_vector = _mm512_sub_epi32(zero_vector, e_vector);
        const __m512i right_shift = _mm512_max_epi32(neg_e_vector, zero_vector);
        const __m512i final_right_shift = _mm512_set1_epi32(31);
        const __m512i right_shift_low =
            _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(right_shift, 0));
        const __m512i right_shift_high =
            _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(right_shift, 1));
        const __m512i final_right_shift_low = _mm512_cvtepi32_epi64(
            _mm512_extracti32x8_epi32(final_right_shift, 0));
        const __m512i final_right_shift_high = _mm512_cvtepi32_epi64(
            _mm512_extracti32x8_epi32(final_right_shift, 1));

        // A "half" added for rounding prior to truncation of 64-bit value.
        const __m512i offset_vector =
            _mm512_slli_epi64(_mm512_set1_epi64(1), 30);

        auto rounding_right_shift = [=](__m512i& results,
                                        const __m512i& exponent) {
          // Construct the "nudge" value for each lane if the exponent is
          // greater than 0. Otherwise, the nudge is 0.
          const __m512i zeros = _mm512_setzero_si512();
          const auto mask_rightshift_gtz =
              _mm512_cmpgt_epi64_mask(exponent, zeros);
          const __m512i one_shift_exp_minus1 = _mm512_sllv_epi64(
              _mm512_set1_epi64(1),
              _mm512_sub_epi64(exponent, _mm512_set1_epi64(1)));
          __m512i nudge = _mm512_mask_mov_epi64(zeros, mask_rightshift_gtz,
                                                one_shift_exp_minus1);
          // Calculate the shifted sum (results + nudge) >> exp.
          const __m512i r_plus_nudge = _mm512_add_epi64(results, nudge);
          const __m512i shifted_sum = _mm512_srav_epi64(r_plus_nudge, exponent);

          // Identify overflow in each lane and create mask.
          const __m512i one_shift_31minus_exp = _mm512_sllv_epi64(
              _mm512_set1_epi64(1),
              _mm512_sub_epi64(_mm512_set1_epi64(31), exponent));
          const auto mask_num_plus_nudge_overflow = _mm512_cmpgt_epi64_mask(
              results, _mm512_sub_epi64(_mm512_set1_epi64(0x7fffffff), nudge));
          // Fill results with either (results + nudge) >> exponent or
          // 1 << (31 - exp) in the case of overflow.
          results = _mm512_mask_mov_epi64(
              shifted_sum, mask_num_plus_nudge_overflow, one_shift_31minus_exp);
        };

        if (per_column_multiplier) {
          auto apply_multiplier = [=](__m512i& accum, int col) {
            __m512i perm_64bit_vals = _mm512_set1_epi64(col % 8);
            // Apply the fixed-point part of the multiplier.
            __m512i left_shift_val =
                _mm512_permutexvar_epi32(_mm512_set1_epi32(col), left_shift);
            __m512i m_64bit_val = _mm512_permutexvar_epi64(
                perm_64bit_vals, col < 8 ? m_64bit_low : m_64bit_high);
            __m512i offset_vector_val =
                _mm512_permutexvar_epi64(perm_64bit_vals, offset_vector);
            __m512i final_right_shift_val = _mm512_permutexvar_epi64(
                perm_64bit_vals,
                col < 8 ? final_right_shift_low : final_right_shift_high);
            __m512i right_shift_val = _mm512_permutexvar_epi64(
                perm_64bit_vals, col < 8 ? right_shift_low : right_shift_high);

            accum = _mm512_sllv_epi32(accum, left_shift_val);
            __m512i scaled_v_low = _mm512_mul_epi32(
                _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(accum, 0)),
                m_64bit_val);
            __m512i scaled_v_high = _mm512_mul_epi32(
                _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(accum, 1)),
                m_64bit_val);

            scaled_v_low = _mm512_add_epi64(scaled_v_low, offset_vector_val);
            scaled_v_high = _mm512_add_epi64(scaled_v_high, offset_vector_val);

            scaled_v_low =
                _mm512_srav_epi64(scaled_v_low, final_right_shift_val);
            scaled_v_high =
                _mm512_srav_epi64(scaled_v_high, final_right_shift_val);

            rounding_right_shift(scaled_v_low, right_shift_val);
            rounding_right_shift(scaled_v_high, right_shift_val);

            accum = _mm512_castsi256_si512(_mm512_cvtepi64_epi32(scaled_v_low));
            accum = _mm512_inserti32x8(accum,
                                       _mm512_cvtepi64_epi32(scaled_v_high), 1);
          };
          apply_multiplier(accum_data_v0, 0);
          apply_multiplier(accum_data_v1, 1);
          apply_multiplier(accum_data_v2, 2);
          apply_multiplier(accum_data_v3, 3);
          apply_multiplier(accum_data_v4, 4);
          apply_multiplier(accum_data_v5, 5);
          apply_multiplier(accum_data_v6, 6);
          apply_multiplier(accum_data_v7, 7);
          apply_multiplier(accum_data_v8, 8);
          apply_multiplier(accum_data_v9, 9);
          apply_multiplier(accum_data_va, 10);
          apply_multiplier(accum_data_vb, 11);
          apply_multiplier(accum_data_vc, 12);
          apply_multiplier(accum_data_vd, 13);
          apply_multiplier(accum_data_ve, 14);
          apply_multiplier(accum_data_vf, 15);
        } else {  // not per-column, so per-row
          auto apply_multiplier = [=](__m512i& accum) {
            accum = _mm512_sllv_epi32(accum, left_shift);
            // Apply the fixed-point part of the multiplier.
            __m512i scaled_v_low = _mm512_mul_epi32(
                _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(accum, 0)),
                m_64bit_low);
            __m512i scaled_v_high = _mm512_mul_epi32(
                _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(accum, 1)),
                m_64bit_high);

            scaled_v_low = _mm512_add_epi64(scaled_v_low, offset_vector);
            scaled_v_high = _mm512_add_epi64(scaled_v_high, offset_vector);

            scaled_v_low =
                _mm512_srav_epi64(scaled_v_low, final_right_shift_low);
            scaled_v_high =
                _mm512_srav_epi64(scaled_v_high, final_right_shift_high);

            rounding_right_shift(scaled_v_low, right_shift_low);
            rounding_right_shift(scaled_v_high, right_shift_high);
            accum = _mm512_castsi256_si512(_mm512_cvtepi64_epi32(scaled_v_low));
            accum = _mm512_inserti32x8(accum,
                                       _mm512_cvtepi64_epi32(scaled_v_high), 1);
          };
          apply_multiplier(accum_data_v0);
          apply_multiplier(accum_data_v1);
          apply_multiplier(accum_data_v2);
          apply_multiplier(accum_data_v3);
          apply_multiplier(accum_data_v4);
          apply_multiplier(accum_data_v5);
          apply_multiplier(accum_data_v6);
          apply_multiplier(accum_data_v7);
          apply_multiplier(accum_data_v8);
          apply_multiplier(accum_data_v9);
          apply_multiplier(accum_data_va);
          apply_multiplier(accum_data_vb);
          apply_multiplier(accum_data_vc);
          apply_multiplier(accum_data_vd);
          apply_multiplier(accum_data_ve);
          apply_multiplier(accum_data_vf);
        }

        if (params.dst_zero_point != 0) {
          __m512i dst_zero_point = _mm512_set1_epi32(params.dst_zero_point);
          accum_data_v0 = _mm512_add_epi32(accum_data_v0, dst_zero_point);
          accum_data_v1 = _mm512_add_epi32(accum_data_v1, dst_zero_point);
          accum_data_v2 = _mm512_add_epi32(accum_data_v2, dst_zero_point);
          accum_data_v3 = _mm512_add_epi32(accum_data_v3, dst_zero_point);
          accum_data_v4 = _mm512_add_epi32(accum_data_v4, dst_zero_point);
          accum_data_v5 = _mm512_add_epi32(accum_data_v5, dst_zero_point);
          accum_data_v6 = _mm512_add_epi32(accum_data_v6, dst_zero_point);
          accum_data_v7 = _mm512_add_epi32(accum_data_v7, dst_zero_point);
          accum_data_v8 = _mm512_add_epi32(accum_data_v8, dst_zero_point);
          accum_data_v9 = _mm512_add_epi32(accum_data_v9, dst_zero_point);
          accum_data_va = _mm512_add_epi32(accum_data_va, dst_zero_point);
          accum_data_vb = _mm512_add_epi32(accum_data_vb, dst_zero_point);
          accum_data_vc = _mm512_add_epi32(accum_data_vc, dst_zero_point);
          accum_data_vd = _mm512_add_epi32(accum_data_vd, dst_zero_point);
          accum_data_ve = _mm512_add_epi32(accum_data_ve, dst_zero_point);
          accum_data_vf = _mm512_add_epi32(accum_data_vf, dst_zero_point);
        }
      }

      const __m512i clamp_max_v = _mm512_set1_epi32(params.clamp_max);
      const __m512i clamp_min_v = _mm512_set1_epi32(params.clamp_min);

      const bool store_full_block =
          (residual_rows == 16) && (residual_cols == 16);

      __m512i accum_data_v[16];

      // In most cases we would make this conditional on (!store_full_block) and
      // unwind the clamp-and-store loop, but the benefit appears small.
      {
        accum_data_v[0] = accum_data_v0;
        accum_data_v[1] = accum_data_v1;
        accum_data_v[2] = accum_data_v2;
        accum_data_v[3] = accum_data_v3;
        accum_data_v[4] = accum_data_v4;
        accum_data_v[5] = accum_data_v5;
        accum_data_v[6] = accum_data_v6;
        accum_data_v[7] = accum_data_v7;
        accum_data_v[8] = accum_data_v8;
        accum_data_v[9] = accum_data_v9;
        accum_data_v[10] = accum_data_va;
        accum_data_v[11] = accum_data_vb;
        accum_data_v[12] = accum_data_vc;
        accum_data_v[13] = accum_data_vd;
        accum_data_v[14] = accum_data_ve;
        accum_data_v[15] = accum_data_vf;
      }

      if (params.dst_type_id == DstTypeId<std::int8_t>::kValue) {
        std::int8_t* tmp_ptr = static_cast<std::int8_t*>(dst_ptr);
        const int block_col_offset = dst_stride;
        if (store_full_block) {
          for (int j = 0; j < 16; ++j) {
            __m512i result = accum_data_v[j];
            result = _mm512_min_epi32(result, clamp_max_v);
            result = _mm512_max_epi32(result, clamp_min_v);
            _mm_storeu_si128(
                reinterpret_cast<__m128i*>(tmp_ptr + j * block_col_offset),
                _mm512_cvtepi32_epi8(result));
          }
        } else {
          for (int j = 0; j < residual_cols; ++j) {
            __m512i result = accum_data_v[j];
            result = _mm512_min_epi32(result, clamp_max_v);
            result = _mm512_max_epi32(result, clamp_min_v);
            _mm_mask_storeu_epi8(tmp_ptr + j * block_col_offset, row_mask,
                                 _mm512_cvtepi32_epi8(result));
          }
        }
        dst_ptr = static_cast<void*>(static_cast<std::int8_t*>(dst_ptr) + 16);
      } else if (params.dst_type_id == DstTypeId<std::uint8_t>::kValue) {
        std::uint8_t* tmp_ptr = static_cast<std::uint8_t*>(dst_ptr);
        const int block_col_offset = dst_stride;
        if (store_full_block) {
          for (int j = 0; j < residual_cols; ++j) {
            __m512i result = accum_data_v[j];
            result = _mm512_min_epi32(result, clamp_max_v);
            result = _mm512_max_epi32(result, clamp_min_v);
            _mm_storeu_si128(
                reinterpret_cast<__m128i*>(tmp_ptr + j * block_col_offset),
                _mm512_cvtepi32_epi8(result));
          }
        } else {
          for (int j = 0; j < residual_cols; ++j) {
            __m512i result = accum_data_v[j];
            result = _mm512_min_epi32(result, clamp_max_v);
            result = _mm512_max_epi32(result, clamp_min_v);
            _mm_mask_storeu_epi8(tmp_ptr + j * block_col_offset, row_mask,
                                 _mm512_cvtepi32_epi8(result));
          }
        }
        dst_ptr = static_cast<void*>(static_cast<std::uint8_t*>(dst_ptr) + 16);
      } else if (params.dst_type_id == DstTypeId<std::int16_t>::kValue) {
        std::int16_t* tmp_ptr = static_cast<std::int16_t*>(dst_ptr);
        const int block_col_offset = dst_stride;
        if (store_full_block) {
          for (int j = 0; j < 16; ++j) {
            __m512i result = accum_data_v[j];
            result = _mm512_min_epi32(result, clamp_max_v);
            result = _mm512_max_epi32(result, clamp_min_v);
            _mm256_storeu_si256(
                reinterpret_cast<__m256i*>(tmp_ptr + j * block_col_offset),
                _mm512_cvtepi32_epi16(result));
          }
        } else {
          for (int j = 0; j < residual_cols; ++j) {
            __m512i result = accum_data_v[j];
            result = _mm512_min_epi32(result, clamp_max_v);
            result = _mm512_max_epi32(result, clamp_min_v);
            _mm256_mask_storeu_epi16(tmp_ptr + j * block_col_offset, row_mask,
                                     _mm512_cvtepi32_epi16(result));
          }
        }
        dst_ptr = static_cast<void*>(static_cast<std::int16_t*>(dst_ptr) + 16);
      } else if (params.dst_type_id == DstTypeId<std::int32_t>::kValue) {
        if (store_full_block) {
          std::int32_t* tmp_ptr = static_cast<std::int32_t*>(dst_ptr);
          for (int j = 0; j < 16; ++j) {
            _mm512_storeu_si512(tmp_ptr + j * dst_stride, accum_data_v[j]);
          }
        } else {
          std::int32_t* tmp_ptr = static_cast<std::int32_t*>(dst_ptr);
          for (int j = 0; j < residual_cols; ++j) {
            _mm512_mask_storeu_epi32(tmp_ptr + j * dst_stride, row_mask,
                                     accum_data_v[j]);
          }
        }
        dst_ptr = static_cast<void*>(static_cast<std::int32_t*>(dst_ptr) + 16);
      } else {
        RUY_DCHECK(false);
      }

      lhs_col_ptr += 16 * params.lhs_stride;
    }  // End row-block loop.

    dst_col_ptr = static_cast<void*>(static_cast<char*>(dst_col_ptr) +
                                     16 * params.dst_stride);
    rhs_col_ptr += 16 * params.rhs_stride;
  }  // End col-block loop.
}  // NOLINT(readability/fn_size)

void Kernel8bitAvx512SingleCol(const KernelParams8bit<16, 16>& params) {
  profiler::ScopeLabel label("Kernel kAvx512 8-bit GEMV");

  RUY_DCHECK_EQ(params.dst_cols, 1);
  RUY_DCHECK_EQ(params.last_col, 0);
  RUY_DCHECK_EQ(params.start_col, 0);

  int bias_ptr_block_increment = params.flags & RUY_ASM_FLAG_HAS_BIAS ? 16 : 0;

  const std::int8_t* rhs_col_ptr = params.rhs_base_ptr;
  void* dst_col_ptr = params.dst_base_ptr;
  const std::int32_t* bias_col_ptr = params.bias;
  if (params.flags & RUY_ASM_FLAG_HAS_BIAS) {
    bias_col_ptr += params.start_row;
  }

  const std::int8_t* lhs_col_ptr = params.lhs_base_ptr;
  void* dst_ptr = dst_col_ptr;
  const std::int32_t* bias_ptr = bias_col_ptr;

  const std::int32_t lhs_zero_point = params.lhs_zero_point;
  const bool has_rhs_sums_offsets =
      (params.flags & RUY_ASM_FLAG_HAS_RHS_SUMS) && lhs_zero_point;
  std::int32_t rhs_sums_offsets[16];
  if (has_rhs_sums_offsets) {
    const __m512i rhs_sums_offset_v =
        _mm512_mullo_epi32(_mm512_set1_epi32(lhs_zero_point),
                           _mm512_loadu_si512(&params.rhs_sums[0]));
    _mm512_storeu_si512(reinterpret_cast<__m512i*>(rhs_sums_offsets),
                        rhs_sums_offset_v);
  }

  for (int row = params.start_row; row <= params.last_row; row += 16) {
    const int residual_rows = std::min(params.dst_rows - row, 16);

    __m512i accum_data_v0;

    // Initialize with bias.
    const __mmask16 row_mask =
        (static_cast<std::uint32_t>(1) << residual_rows) - 1;
    __m512i initial_accum_data =
        _mm512_loadu_si512(reinterpret_cast<const __m512i*>(bias_ptr));
    bias_ptr += bias_ptr_block_increment;

    const std::int32_t rhs_zero_point = params.rhs_zero_point;
    if ((params.flags & RUY_ASM_FLAG_HAS_LHS_SUMS) && rhs_zero_point) {
      const __m512i lhs_sums_offset =
          _mm512_mullo_epi32(_mm512_set1_epi32(rhs_zero_point),
                             _mm512_loadu_si512(&params.lhs_sums[row]));
      initial_accum_data =
          _mm512_sub_epi32(initial_accum_data, lhs_sums_offset);
    }

    const std::int32_t prod_zp_depth = params.prod_zp_depth;
    if (prod_zp_depth != 0) {
      initial_accum_data = _mm512_add_epi32(initial_accum_data,
                                            _mm512_set1_epi32(prod_zp_depth));
    }

    // Adjustments differing across columns.
    if (has_rhs_sums_offsets) {
      accum_data_v0 = _mm512_sub_epi32(initial_accum_data,
                                       _mm512_set1_epi32(rhs_sums_offsets[0]));
    } else {
      accum_data_v0 = initial_accum_data;
    }

    const std::int8_t* lhs_ptr = lhs_col_ptr;
    const std::int8_t* rhs_ptr = rhs_col_ptr;
    for (int d = 0; d < params.depth; d += 4) {
      const __m512i lhs_data = _mm512_loadu_si512(lhs_ptr);
      const __m128i rhs_data_8bit =
          _mm_loadu_si128(reinterpret_cast<const __m128i*>(rhs_ptr));

      // Each "int32" is two 16-bit RHS values, sign extended from 8-bit.
      // For simplicity we load 4x the data that we need and process twice the
      // data  that we need  and store only the data we need.
      std::int32_t rhs_data[2];
      const __m128i rhs_16_bit_dup = _mm_cvtepi8_epi16(rhs_data_8bit);
      // Now that we have cast the RHS data, we store it so that each value
      // can be separately loaded in the accumulation loop.
      _mm_storeu_si64(reinterpret_cast<__m128i*>(rhs_data), rhs_16_bit_dup);

      // Take bytes 0, 1, 4, 5, 8, 9, ... and expand to 16-bit.
      const __m512i lhs_16_bit_low =
          _mm512_cvtepi8_epi16(_mm512_cvtepi32_epi16(lhs_data));
      // Take bytes 2, 3, 6, 7, 10, 11, ... and expand to 16-bit.
      const __m512i lhs_16_bit_high = _mm512_cvtepi8_epi16(
          _mm512_cvtepi32_epi16(_mm512_srli_epi32(lhs_data, 16)));

      // Process column 0.
      __m512i accum_v = accum_data_v0;
      constexpr int index = 0;

      const __m512i rhs_16_bit_dup_low = _mm512_set1_epi32(rhs_data[index]);
      const __m512i rhs_16_bit_dup_high =
          _mm512_set1_epi32(rhs_data[index + 1]);

      accum_v = _mm512_add_epi32(
          accum_v, _mm512_madd_epi16(lhs_16_bit_low, rhs_16_bit_dup_low));
      accum_v = _mm512_add_epi32(
          accum_v, _mm512_madd_epi16(lhs_16_bit_high, rhs_16_bit_dup_high));
      accum_data_v0 = accum_v;

      lhs_ptr += 16 * 4;
      rhs_ptr += 16 * 4;
    }

    if (params.dst_type_id != DstTypeId<std::int32_t>::kValue) {
      __m512i m_vector;
      __m512i e_vector;
      // Does not make use of RUY_ASM_FLAG_NEEDS_LEFT_SHIFT.
      int channel = (params.flags & RUY_ASM_FLAG_HAS_PERCHANNEL) ? row : 0;
      m_vector = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(
          params.multiplier_fixedpoint + channel));
      e_vector = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(
          params.multiplier_exponent + channel));

      const __m512i m_64bit_low =
          _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(m_vector, 0));
      const __m512i m_64bit_high =
          _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(m_vector, 1));

      const __m512i zero_vector = _mm512_setzero_epi32();
      const __m512i left_shift = _mm512_max_epi32(e_vector, zero_vector);
      const __m512i neg_e_vector = _mm512_sub_epi32(zero_vector, e_vector);
      const __m512i right_shift = _mm512_max_epi32(neg_e_vector, zero_vector);
      const __m512i final_right_shift = _mm512_set1_epi32(31);
      const __m512i right_shift_low =
          _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(right_shift, 0));
      const __m512i right_shift_high =
          _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(right_shift, 1));
      const __m512i final_right_shift_low = _mm512_cvtepi32_epi64(
          _mm512_extracti32x8_epi32(final_right_shift, 0));
      const __m512i final_right_shift_high = _mm512_cvtepi32_epi64(
          _mm512_extracti32x8_epi32(final_right_shift, 1));

      // A "half" added for rounding prior to truncation of 64-bit value.
      const __m512i offset_vector = _mm512_slli_epi64(_mm512_set1_epi64(1), 30);

      auto rounding_right_shift = [=](__m512i& results,
                                      const __m512i& exponent) {
        // Construct the "nudge" value for each lane if the exponent is
        // greater than 0. Otherwise, the nudge is 0.
        const __m512i zeros = _mm512_setzero_si512();
        const auto mask_rightshift_gtz =
            _mm512_cmpgt_epi64_mask(exponent, zeros);
        const __m512i one_shift_exp_minus1 =
            _mm512_sllv_epi64(_mm512_set1_epi64(1),
                              _mm512_sub_epi64(exponent, _mm512_set1_epi64(1)));
        __m512i nudge = _mm512_mask_mov_epi64(zeros, mask_rightshift_gtz,
                                              one_shift_exp_minus1);
        // Calculate the shifted sum (results + nudge) >> exp.
        const __m512i r_plus_nudge = _mm512_add_epi64(results, nudge);
        const __m512i shifted_sum = _mm512_srav_epi64(r_plus_nudge, exponent);

        // Identify overflow in each lane and create mask.
        const __m512i one_shift_31minus_exp = _mm512_sllv_epi64(
            _mm512_set1_epi64(1),
            _mm512_sub_epi64(_mm512_set1_epi64(31), exponent));
        const auto mask_num_plus_nudge_overflow = _mm512_cmpgt_epi64_mask(
            results, _mm512_sub_epi64(_mm512_set1_epi64(0x7fffffff), nudge));
        // Fill results with either (results + nudge) >> exponent or
        // 1 << (31 - exp) in the case of overflow.
        results = _mm512_mask_mov_epi64(
            shifted_sum, mask_num_plus_nudge_overflow, one_shift_31minus_exp);
      };

      // Shift and round column 0.
      accum_data_v0 = _mm512_sllv_epi32(accum_data_v0, left_shift);
      // Apply the fixed-point part of the multiplier.
      __m512i scaled_v_low = _mm512_mul_epi32(
          _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(accum_data_v0, 0)),
          m_64bit_low);
      __m512i scaled_v_high = _mm512_mul_epi32(
          _mm512_cvtepi32_epi64(_mm512_extracti32x8_epi32(accum_data_v0, 1)),
          m_64bit_high);

      scaled_v_low = _mm512_add_epi64(scaled_v_low, offset_vector);
      scaled_v_high = _mm512_add_epi64(scaled_v_high, offset_vector);

      scaled_v_low = _mm512_srav_epi64(scaled_v_low, final_right_shift_low);
      scaled_v_high = _mm512_srav_epi64(scaled_v_high, final_right_shift_high);

      rounding_right_shift(scaled_v_low, right_shift_low);
      rounding_right_shift(scaled_v_high, right_shift_high);

      accum_data_v0 =
          _mm512_castsi256_si512(_mm512_cvtepi64_epi32(scaled_v_low));
      accum_data_v0 = _mm512_inserti32x8(
          accum_data_v0, _mm512_cvtepi64_epi32(scaled_v_high), 1);

      if (params.dst_zero_point != 0) {
        __m512i dst_zero_point = _mm512_set1_epi32(params.dst_zero_point);
        accum_data_v0 = _mm512_add_epi32(accum_data_v0, dst_zero_point);
      }
    }

    const __m512i clamp_max_v = _mm512_set1_epi32(params.clamp_max);
    const __m512i clamp_min_v = _mm512_set1_epi32(params.clamp_min);

    if (params.dst_type_id == DstTypeId<std::int8_t>::kValue) {
      std::int8_t* tmp_ptr = static_cast<std::int8_t*>(dst_ptr);
      __m512i result = accum_data_v0;
      result = _mm512_min_epi32(result, clamp_max_v);
      result = _mm512_max_epi32(result, clamp_min_v);
      _mm_mask_storeu_epi8(tmp_ptr, row_mask, _mm512_cvtepi32_epi8(result));
      dst_ptr = static_cast<void*>(static_cast<std::int8_t*>(dst_ptr) + 16);
    } else if (params.dst_type_id == DstTypeId<std::uint8_t>::kValue) {
      std::uint8_t* tmp_ptr = static_cast<std::uint8_t*>(dst_ptr);
      __m512i result = accum_data_v0;
      result = _mm512_min_epi32(result, clamp_max_v);
      result = _mm512_max_epi32(result, clamp_min_v);
      _mm_mask_storeu_epi8(tmp_ptr, row_mask, _mm512_cvtepi32_epi8(result));
      dst_ptr = static_cast<void*>(static_cast<std::uint8_t*>(dst_ptr) + 16);
    } else if (params.dst_type_id == DstTypeId<std::int16_t>::kValue) {
      std::int16_t* tmp_ptr = static_cast<std::int16_t*>(dst_ptr);
      __m512i result = accum_data_v0;
      result = _mm512_min_epi32(result, clamp_max_v);
      result = _mm512_max_epi32(result, clamp_min_v);
      _mm256_mask_storeu_epi16(tmp_ptr, row_mask,
                               _mm512_cvtepi32_epi16(result));
      dst_ptr = static_cast<void*>(static_cast<std::int16_t*>(dst_ptr) + 16);
    } else if (params.dst_type_id == DstTypeId<std::int32_t>::kValue) {
      std::int32_t* tmp_ptr = static_cast<std::int32_t*>(dst_ptr);
      _mm512_mask_storeu_epi32(tmp_ptr, row_mask, accum_data_v0);
      dst_ptr = static_cast<void*>(static_cast<std::int32_t*>(dst_ptr) + 16);
    } else {
      RUY_DCHECK(false);
    }

    lhs_col_ptr += 16 * params.lhs_stride;
  }  // End row-block loop.
}  // NOLINT(readability/fn_size)

void KernelFloatAvx512(const KernelParamsFloat<16, 16>& params) {
  profiler::ScopeLabel label("Kernel kAvx512 float");

  // As parameters are defined, we need to scale by sizeof(float).
  const std::int64_t lhs_stride = params.lhs_stride >> 2;
  const std::int64_t dst_stride = params.dst_stride >> 2;
  const std::int64_t rhs_stride = params.rhs_stride >> 2;

  int bias_ptr_block_increment = params.flags & RUY_ASM_FLAG_HAS_BIAS ? 1 : 0;
  const int end_row = std::min(params.dst_rows, params.last_row + 16);
  const int end_col = std::min(params.dst_cols, params.last_col + 16);

  const float* adj_rhs_col_ptr =
      params.rhs_base_ptr - params.start_col * rhs_stride;
  float* adj_dst_col_ptr =
      params.dst_base_ptr - params.start_col * dst_stride - params.start_row;
  const float* adj_lhs_col_ptr =
      params.lhs_base_ptr - params.start_row * lhs_stride;
  const float* bias_ptr = params.bias;

  const __m512 clamp_max_v = _mm512_set1_ps(params.clamp_max);
  const __m512 clamp_min_v = _mm512_set1_ps(params.clamp_min);
  const bool channel_dimension_is_col =
      params.flags & RUY_ASM_FLAG_CHANNEL_DIMENSION_IS_COL;

  int col = params.start_col;
  for (; col <= end_col - 16; col += 16) {
    const float* rhs_col_ptr = adj_rhs_col_ptr + col * rhs_stride;
    float* dst_col_ptr = adj_dst_col_ptr + col * dst_stride;

    int row = params.start_row;
    for (; row <= end_row - 16; row += 16) {
      const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
      float* dst_ptr = dst_col_ptr + row;

      // Process block in two halves, split by columns.
#pragma unroll(1)
      for (int mmm = 0; mmm < 2; ++mmm) {
        __m512 accum_data_v0;
        __m512 accum_data_v1;
        __m512 accum_data_v2;
        __m512 accum_data_v3;
        __m512 accum_data_v4;
        __m512 accum_data_v5;
        __m512 accum_data_v6;
        __m512 accum_data_v7;

        // Initialize with bias.
        if (channel_dimension_is_col) {
          const float* bias_elem_ptr =
              bias_ptr + (col + 8 * mmm) * bias_ptr_block_increment;
          accum_data_v0 = _mm512_set1_ps(bias_elem_ptr[0]);
          accum_data_v1 = _mm512_set1_ps(bias_elem_ptr[1]);
          accum_data_v2 = _mm512_set1_ps(bias_elem_ptr[2]);
          accum_data_v3 = _mm512_set1_ps(bias_elem_ptr[3]);
          accum_data_v4 = _mm512_set1_ps(bias_elem_ptr[4]);
          accum_data_v5 = _mm512_set1_ps(bias_elem_ptr[5]);
          accum_data_v6 = _mm512_set1_ps(bias_elem_ptr[6]);
          accum_data_v7 = _mm512_set1_ps(bias_elem_ptr[7]);
        } else {
          const __m512 initial_accum_data =
              _mm512_loadu_ps(bias_ptr + row * bias_ptr_block_increment);

          accum_data_v0 = initial_accum_data;
          accum_data_v1 = initial_accum_data;
          accum_data_v2 = initial_accum_data;
          accum_data_v3 = initial_accum_data;
          accum_data_v4 = initial_accum_data;
          accum_data_v5 = initial_accum_data;
          accum_data_v6 = initial_accum_data;
          accum_data_v7 = initial_accum_data;
        }

        const float* lhs_ptr = lhs_col_ptr;
        const float* rhs_ptr = rhs_col_ptr + 8 * mmm;
        for (int d = 0; d < (params.depth - 1); ++d) {
          const __m512 lhs_data = _mm512_loadu_ps(lhs_ptr);
          const float* rhs_data = rhs_ptr;
          lhs_ptr += 16;
          rhs_ptr += 16;

          // GCC and clang can fuse set1+FMA into an FMA with EVEX broadcast:
          // https://gcc.godbolt.org/z/xbfqWYfn1. Clang is more likely to do
          // so if given an rvalue.
          accum_data_v0 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[0]),
                                          accum_data_v0);
          accum_data_v1 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[1]),
                                          accum_data_v1);
          accum_data_v2 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[2]),
                                          accum_data_v2);
          accum_data_v3 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[3]),
                                          accum_data_v3);
          accum_data_v4 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[4]),
                                          accum_data_v4);
          accum_data_v5 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[5]),
                                          accum_data_v5);
          accum_data_v6 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[6]),
                                          accum_data_v6);
          accum_data_v7 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[7]),
                                          accum_data_v7);
        }
        {  // nested extra blocks lead to measurable speed gains
          const __m512 lhs_data = _mm512_loadu_ps(lhs_ptr);
          const float* rhs_data = rhs_ptr;
          accum_data_v0 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[0]),
                                          accum_data_v0);
          accum_data_v1 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[1]),
                                          accum_data_v1);
          accum_data_v2 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[2]),
                                          accum_data_v2);
          accum_data_v3 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[3]),
                                          accum_data_v3);
          accum_data_v4 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[4]),
                                          accum_data_v4);
          accum_data_v5 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[5]),
                                          accum_data_v5);
          accum_data_v6 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[6]),
                                          accum_data_v6);
          accum_data_v7 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[7]),
                                          accum_data_v7);
          {
            float* block_ptr = dst_ptr + (mmm * 8 + 0) * dst_stride;
            accum_data_v0 = _mm512_min_ps(accum_data_v0, clamp_max_v);
            accum_data_v0 = _mm512_max_ps(accum_data_v0, clamp_min_v);
            _mm512_storeu_ps(block_ptr + 0 * dst_stride, accum_data_v0);
            accum_data_v1 = _mm512_min_ps(accum_data_v1, clamp_max_v);
            accum_data_v1 = _mm512_max_ps(accum_data_v1, clamp_min_v);
            _mm512_storeu_ps(block_ptr + 1 * dst_stride, accum_data_v1);
            accum_data_v2 = _mm512_min_ps(accum_data_v2, clamp_max_v);
            accum_data_v2 = _mm512_max_ps(accum_data_v2, clamp_min_v);
            _mm512_storeu_ps(block_ptr + 2 * dst_stride, accum_data_v2);
            accum_data_v3 = _mm512_min_ps(accum_data_v3, clamp_max_v);
            accum_data_v3 = _mm512_max_ps(accum_data_v3, clamp_min_v);
            _mm512_storeu_ps(block_ptr + 3 * dst_stride, accum_data_v3);
            accum_data_v4 = _mm512_min_ps(accum_data_v4, clamp_max_v);
            accum_data_v4 = _mm512_max_ps(accum_data_v4, clamp_min_v);
            _mm512_storeu_ps(block_ptr + 4 * dst_stride, accum_data_v4);
            accum_data_v5 = _mm512_min_ps(accum_data_v5, clamp_max_v);
            accum_data_v5 = _mm512_max_ps(accum_data_v5, clamp_min_v);
            _mm512_storeu_ps(block_ptr + 5 * dst_stride, accum_data_v5);
            accum_data_v6 = _mm512_min_ps(accum_data_v6, clamp_max_v);
            accum_data_v6 = _mm512_max_ps(accum_data_v6, clamp_min_v);
            _mm512_storeu_ps(block_ptr + 6 * dst_stride, accum_data_v6);
            accum_data_v7 = _mm512_min_ps(accum_data_v7, clamp_max_v);
            accum_data_v7 = _mm512_max_ps(accum_data_v7, clamp_min_v);
            _mm512_storeu_ps(block_ptr + 7 * dst_stride, accum_data_v7);
          }
        }
      }
    }    // End row-block loop.

    // The unrolling within this conditional may be somewhat pointless. It
    // depends on the kinds of models.
    if (row < end_row) {
      const int residual_rows = end_row - row;

      const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
      float* dst_ptr = dst_col_ptr + row;

      const __mmask16 row_mask =
          (static_cast<std::uint32_t>(1) << residual_rows) - 1;

      // Process block in two halves, split by columns.
      for (int mmm = 0; mmm < 2; ++mmm) {
        __m512 accum_data_v0;
        __m512 accum_data_v1;
        __m512 accum_data_v2;
        __m512 accum_data_v3;
        __m512 accum_data_v4;
        __m512 accum_data_v5;
        __m512 accum_data_v6;
        __m512 accum_data_v7;

        // Initialize with bias.
        if (channel_dimension_is_col) {
          const float* bias_elem_ptr =
              bias_ptr + (col + 8 * mmm) * bias_ptr_block_increment;
          accum_data_v0 = _mm512_set1_ps(bias_elem_ptr[0]);
          accum_data_v1 = _mm512_set1_ps(bias_elem_ptr[1]);
          accum_data_v2 = _mm512_set1_ps(bias_elem_ptr[2]);
          accum_data_v3 = _mm512_set1_ps(bias_elem_ptr[3]);
          accum_data_v4 = _mm512_set1_ps(bias_elem_ptr[4]);
          accum_data_v5 = _mm512_set1_ps(bias_elem_ptr[5]);
          accum_data_v6 = _mm512_set1_ps(bias_elem_ptr[6]);
          accum_data_v7 = _mm512_set1_ps(bias_elem_ptr[7]);
        } else {
          const __m512 initial_accum_data =
              _mm512_loadu_ps(bias_ptr + row * bias_ptr_block_increment);

          accum_data_v0 = initial_accum_data;
          accum_data_v1 = initial_accum_data;
          accum_data_v2 = initial_accum_data;
          accum_data_v3 = initial_accum_data;
          accum_data_v4 = initial_accum_data;
          accum_data_v5 = initial_accum_data;
          accum_data_v6 = initial_accum_data;
          accum_data_v7 = initial_accum_data;
        }

        const float* lhs_ptr = lhs_col_ptr;
        const float* rhs_ptr = rhs_col_ptr + 8 * mmm;
        for (int d = 0; d < (params.depth - 1); ++d) {
          const __m512 lhs_data = _mm512_loadu_ps(lhs_ptr);
          const float* rhs_data = rhs_ptr;
          lhs_ptr += 16;
          rhs_ptr += 16;
          // GCC and clang can fuse set1+FMA into an FMA with EVEX broadcast:
          // https://gcc.godbolt.org/z/xbfqWYfn1. Clang is more likely to do
          // so if given an rvalue.
          accum_data_v0 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[0]),
                                          accum_data_v0);
          accum_data_v1 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[1]),
                                          accum_data_v1);
          accum_data_v2 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[2]),
                                          accum_data_v2);
          accum_data_v3 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[3]),
                                          accum_data_v3);
          accum_data_v4 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[4]),
                                          accum_data_v4);
          accum_data_v5 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[5]),
                                          accum_data_v5);
          accum_data_v6 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[6]),
                                          accum_data_v6);
          accum_data_v7 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[7]),
                                          accum_data_v7);
        }
        {
          const __m512 lhs_data = _mm512_loadu_ps(lhs_ptr);
          const float* rhs_data = rhs_ptr;
          accum_data_v0 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[0]),
                                          accum_data_v0);
          accum_data_v1 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[1]),
                                          accum_data_v1);
          accum_data_v2 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[2]),
                                          accum_data_v2);
          accum_data_v3 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[3]),
                                          accum_data_v3);
          accum_data_v4 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[4]),
                                          accum_data_v4);
          accum_data_v5 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[5]),
                                          accum_data_v5);
          accum_data_v6 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[6]),
                                          accum_data_v6);
          accum_data_v7 = _mm512_fmadd_ps(lhs_data, _mm512_set1_ps(rhs_data[7]),
                                          accum_data_v7);
          {
            float* block_ptr = dst_ptr + (mmm * 8 + 0) * dst_stride;
            accum_data_v0 = _mm512_min_ps(accum_data_v0, clamp_max_v);
            accum_data_v0 = _mm512_max_ps(accum_data_v0, clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr + 0 * dst_stride, row_mask,
                                  accum_data_v0);
            accum_data_v1 = _mm512_min_ps(accum_data_v1, clamp_max_v);
            accum_data_v1 = _mm512_max_ps(accum_data_v1, clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr + 1 * dst_stride, row_mask,
                                  accum_data_v1);
            accum_data_v2 = _mm512_min_ps(accum_data_v2, clamp_max_v);
            accum_data_v2 = _mm512_max_ps(accum_data_v2, clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr + 2 * dst_stride, row_mask,
                                  accum_data_v2);
            accum_data_v3 = _mm512_min_ps(accum_data_v3, clamp_max_v);
            accum_data_v3 = _mm512_max_ps(accum_data_v3, clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr + 3 * dst_stride, row_mask,
                                  accum_data_v3);
            accum_data_v4 = _mm512_min_ps(accum_data_v4, clamp_max_v);
            accum_data_v4 = _mm512_max_ps(accum_data_v4, clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr + 4 * dst_stride, row_mask,
                                  accum_data_v4);
            accum_data_v5 = _mm512_min_ps(accum_data_v5, clamp_max_v);
            accum_data_v5 = _mm512_max_ps(accum_data_v5, clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr + 5 * dst_stride, row_mask,
                                  accum_data_v5);
            accum_data_v6 = _mm512_min_ps(accum_data_v6, clamp_max_v);
            accum_data_v6 = _mm512_max_ps(accum_data_v6, clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr + 6 * dst_stride, row_mask,
                                  accum_data_v6);
            accum_data_v7 = _mm512_min_ps(accum_data_v7, clamp_max_v);
            accum_data_v7 = _mm512_max_ps(accum_data_v7, clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr + 7 * dst_stride, row_mask,
                                  accum_data_v7);
          }
        }
      }  // Inner half-block loop.
    }    // Residual rows, main col-block loop.
  }      // End col-block loop.

  if (col < end_col) {
    RUY_DCHECK_GE(end_col - col, 0);
    RUY_DCHECK_LT(end_col - col, 16);

    __m512 accum_data_v[8];

    const float* rhs_col_ptr = adj_rhs_col_ptr + col * rhs_stride;
    float* dst_col_ptr = adj_dst_col_ptr + col * dst_stride;

    for (int row = params.start_row; row < end_row; row += 16) {
      const int residual_rows = std::min(end_row - row, 16);

      const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
      float* dst_ptr = dst_col_ptr + row;

      const __mmask16 row_mask =
          (static_cast<std::uint32_t>(1) << residual_rows) - 1;

      // Process block in two halves, split by columns.
      for (int mmm = 0; mmm < 2; ++mmm) {
        // Initialize with bias.
        if (channel_dimension_is_col) {
          const float* bias_elem_ptr =
              bias_ptr + (col + 8 * mmm) * bias_ptr_block_increment;
          for (int j = 0; j < 8; ++j) {
            accum_data_v[j] = _mm512_set1_ps(bias_elem_ptr[j]);
          }
        } else {
          const __m512 initial_accum_data =
              _mm512_loadu_ps(bias_ptr + row * bias_ptr_block_increment);
          for (int j = 0; j < 8; ++j) {
            accum_data_v[j] = initial_accum_data;
          }
        }

        const float* lhs_ptr = lhs_col_ptr;
        const float* rhs_ptr = rhs_col_ptr + 8 * mmm;
        for (int d = 0; d < params.depth; ++d) {
          const __m512 lhs_data = _mm512_loadu_ps(lhs_ptr);
          const float* rhs_data = rhs_ptr;

          for (int j = 0; j < 8; ++j) {
            const __m512 dup_rhs_element_j = _mm512_set1_ps(rhs_data[j]);
            accum_data_v[j] =
                _mm512_fmadd_ps(lhs_data, dup_rhs_element_j, accum_data_v[j]);
          }
          lhs_ptr += 16;
          rhs_ptr += 16;
        }

        const int residual_cols = std::min(end_col - col - 8 * mmm, 8);

        if (residual_rows == 16) {
          if (residual_cols == 8) {
            for (int j = 0; j < 8; ++j) {
              float* block_ptr = dst_ptr + (mmm * 8 + j) * dst_stride;
              accum_data_v[j] = _mm512_min_ps(accum_data_v[j], clamp_max_v);
              accum_data_v[j] = _mm512_max_ps(accum_data_v[j], clamp_min_v);
              _mm512_storeu_ps(block_ptr, accum_data_v[j]);
            }
          } else {
            for (int j = 0; j < residual_cols; ++j) {
              float* block_ptr = dst_ptr + (mmm * 8 + j) * dst_stride;
              accum_data_v[j] = _mm512_min_ps(accum_data_v[j], clamp_max_v);
              accum_data_v[j] = _mm512_max_ps(accum_data_v[j], clamp_min_v);
              _mm512_storeu_ps(block_ptr, accum_data_v[j]);
            }
          }
        } else {
          for (int j = 0; j < residual_cols; ++j) {
            float* block_ptr = dst_ptr + (mmm * 8 + j) * dst_stride;
            accum_data_v[j] = _mm512_min_ps(accum_data_v[j], clamp_max_v);
            accum_data_v[j] = _mm512_max_ps(accum_data_v[j], clamp_min_v);
            _mm512_mask_storeu_ps(block_ptr, row_mask, accum_data_v[j]);
          }
        }
      }  // Inner half-block loop.
    }    // End row-block loop.
  }      // Residual cols.
}

void KernelFloatAvx512SingleCol(const KernelParamsFloat<16, 16>& params) {
  profiler::ScopeLabel label("Kernel kAvx512 float GEMV");

  RUY_DCHECK_EQ(params.dst_cols, 1);
  RUY_DCHECK_EQ(params.last_col, 0);
  RUY_DCHECK_EQ(params.start_col, 0);

  // As parameters are defined, we need to scale by sizeof(float).
  const std::int64_t lhs_stride = params.lhs_stride >> 2;

  int bias_ptr_block_increment = params.flags & RUY_ASM_FLAG_HAS_BIAS ? 1 : 0;
  const int end_row = std::min(params.dst_rows, params.last_row + 16);

  float* adj_dst_col_ptr = params.dst_base_ptr - params.start_row;
  const float* adj_lhs_col_ptr =
      params.lhs_base_ptr - params.start_row * lhs_stride;
  const float* bias_col_ptr = params.bias;

  const __m512 clamp_max_v = _mm512_set1_ps(params.clamp_max);
  const __m512 clamp_min_v = _mm512_set1_ps(params.clamp_min);

  __m512 accum_data_v;

  const float* rhs_col_ptr = params.rhs_base_ptr;
  float* dst_col_ptr = adj_dst_col_ptr;

  int row = params.start_row;
  for (; row <= end_row - 16; row += 16) {
    const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
    float* dst_ptr = dst_col_ptr + row;
    const float* bias_ptr = bias_col_ptr + row * bias_ptr_block_increment;

    // Initialize with bias.
    accum_data_v = _mm512_loadu_ps(bias_ptr);

    const float* lhs_ptr = lhs_col_ptr;
    const float* rhs_ptr = rhs_col_ptr;
    for (int d = 0; d < params.depth; ++d) {
      const __m512 lhs_data = _mm512_loadu_ps(lhs_ptr);
      const float rhs_data = *rhs_ptr;

      const __m512 dup_rhs_element_j = _mm512_set1_ps(rhs_data);
      accum_data_v = _mm512_fmadd_ps(lhs_data, dup_rhs_element_j, accum_data_v);
      lhs_ptr += 16;
      rhs_ptr += 16;
    }

    accum_data_v = _mm512_min_ps(accum_data_v, clamp_max_v);
    accum_data_v = _mm512_max_ps(accum_data_v, clamp_min_v);
    _mm512_storeu_ps(dst_ptr, accum_data_v);
  }  // End row-block loop.

  if (row < end_row) {
    const int residual_rows = end_row - row;
    RUY_CHECK_GE(residual_rows, 1);
    RUY_CHECK_LT(residual_rows, 16);

    const float* lhs_col_ptr = adj_lhs_col_ptr + row * lhs_stride;
    float* dst_ptr = dst_col_ptr + row;
    const float* bias_ptr = bias_col_ptr + row * bias_ptr_block_increment;

    // Initialize with bias.
    const __mmask16 row_mask =
        (static_cast<std::uint32_t>(1) << residual_rows) - 1;
    accum_data_v = _mm512_loadu_ps(bias_ptr);

    const float* lhs_ptr = lhs_col_ptr;
    const float* rhs_ptr = rhs_col_ptr;
    for (int d = 0; d < params.depth; ++d) {
      const __m512 lhs_data = _mm512_loadu_ps(lhs_ptr);
      const float rhs_data = *rhs_ptr;

      const __m512 dup_rhs_element_j = _mm512_set1_ps(rhs_data);
      accum_data_v = _mm512_fmadd_ps(lhs_data, dup_rhs_element_j, accum_data_v);
      lhs_ptr += 16;
      rhs_ptr += 16;
    }

    accum_data_v = _mm512_min_ps(accum_data_v, clamp_max_v);
    accum_data_v = _mm512_max_ps(accum_data_v, clamp_min_v);
    _mm512_mask_storeu_ps(dst_ptr, row_mask, accum_data_v);
  }  // End handling of residual rows.
}

#endif  //  RUY_PLATFORM_AVX512 && RUY_OPT(ASM)

}  // namespace ruy
