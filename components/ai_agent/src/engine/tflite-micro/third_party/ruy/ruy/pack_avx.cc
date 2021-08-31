/* Copyright 2020 Google LLC. All Rights Reserved.

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

#include <cstdint>
#include <cstring>

#include "ruy/check_macros.h"
#include "ruy/opt_set.h"
#include "ruy/pack_x86.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"

#if RUY_PLATFORM_AVX && RUY_OPT(INTRINSICS)
#include <immintrin.h>  // IWYU pragma: keep
#endif

namespace ruy {

#if !(RUY_PLATFORM_AVX && RUY_OPT(ASM))

void Pack8bitColMajorForAvx(const std::int8_t*, std::int8_t, const std::int8_t*,
                            int, int, int, std::int8_t*, std::int32_t*) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

void PackFloatColMajorForAvx(const float*, const float*, int, int, int,
                             float*) {
  // CPU-ID-based checks should disable the path that would reach this point.
  RUY_DCHECK(false);
}

void Pack8bitRowMajorForAvx(const std::uint8_t*, int, int, std::int8_t*, int,
                            int, int, int, int, int, int, std::int32_t*) {
  RUY_DCHECK(false);
}

#else  // RUY_PLATFORM_AVX && RUY_OPT(ASM)

// The first int8_t template parameter is arbitrary: this routine is common to
// all 8-bit source matrix types.
using PackImpl8bitAvx =
    PackImpl<Path::kAvx, FixedKernelLayout<Order::kColMajor, 4, 8>, std::int8_t,
             std::int8_t, std::int32_t, Order::kColMajor>;

using PackImplFloatAvx =
    PackImpl<Path::kAvx, FixedKernelLayout<Order::kRowMajor, 1, 8>, float,
             float, float, Order::kColMajor>;

namespace {

// Perform the equivalent of mm256_permutevar8x32 with
// a second argument of {7, 5, 3, 1, 6, 4, 2, 0}
inline __m256i PermuteEpi32EvenOdds(const __m256i& a) {
  // a_lo = 3 2 1 0
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  // a_hi = 7 6 5 4
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  // shuffle a_lo to get 3 1 2 0
  __m128i tmp_lo = _mm_shuffle_epi32(a_lo, 0xd8);
  // shuffle a_hi to get 7 5 6 4
  __m128i tmp_hi = _mm_shuffle_epi32(a_hi, 0xd8);
  // unpack lo 64 of res_lo and res hi to get 6 4 2 0
  __m128i res_lo = _mm_unpacklo_epi64(tmp_lo, tmp_hi);
  // unpack hi 64 of res_lo and res hi to get 7 5 3 1
  __m128i res_hi = _mm_unpackhi_epi64(tmp_lo, tmp_hi);
  return _mm256_set_m128i(res_hi, res_lo);
}

inline __m128i mm256_extracti128_si256(const __m256i& a, const int imm) {
  switch (imm) {
    case 0:
      return _mm256_extractf128_si256(a, 0);
    case 1:
      return _mm256_extractf128_si256(a, 1);
    default:
      RUY_DCHECK_LT(imm, 2);
      return _mm_setzero_si128();
  }
}

inline __m256i mm256_cvtepi8_epi16(const __m128i& a) {
  // Take the upper 64 bits of a and put in the first 64 bits of 'hi'
  __m128i hi = _mm_unpackhi_epi64(a, _mm_setzero_si128());
  return _mm256_set_m128i(_mm_cvtepi8_epi16(hi), _mm_cvtepi8_epi16(a));
}

inline __m256i mm256_cvtepi16_epi32(const __m128i& a) {
  // Take the upper 64 bits of a and put in the first 64 bits of 'hi'
  __m128i hi = _mm_unpackhi_epi64(a, _mm_setzero_si128());
  return _mm256_set_m128i(_mm_cvtepi16_epi32(hi), _mm_cvtepi16_epi32(a));
}

inline __m256i mm256_xor_si256(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_xor_si128(a_lo, b_lo);
  __m128i hi = _mm_xor_si128(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m256i mm256_unpacklo_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_unpacklo_epi32(a_lo, b_lo);
  __m128i hi = _mm_unpacklo_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m256i mm256_unpacklo_epi64(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_unpacklo_epi64(a_lo, b_lo);
  __m128i hi = _mm_unpacklo_epi64(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m256i mm256_unpackhi_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_unpackhi_epi32(a_lo, b_lo);
  __m128i hi = _mm_unpackhi_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m256i mm256_unpackhi_epi64(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_unpackhi_epi64(a_lo, b_lo);
  __m128i hi = _mm_unpackhi_epi64(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m256i mm256_add_epi32(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_add_epi32(a_lo, b_lo);
  __m128i hi = _mm_add_epi32(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m256i mm256_add_epi16(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_add_epi16(a_lo, b_lo);
  __m128i hi = _mm_add_epi16(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m256i mm256_madd_epi16(const __m256i& a, const __m256i& b) {
  __m128i a_lo = _mm256_extractf128_si256(a, 0);
  __m128i a_hi = _mm256_extractf128_si256(a, 1);
  __m128i b_lo = _mm256_extractf128_si256(b, 0);
  __m128i b_hi = _mm256_extractf128_si256(b, 1);
  __m128i lo = _mm_madd_epi16(a_lo, b_lo);
  __m128i hi = _mm_madd_epi16(a_hi, b_hi);
  return _mm256_set_m128i(hi, lo);
}

inline __m128i mm_permute_helper(const __m256i& a, const __m256i& b,
                                 const int imm) {
  __m128i tmp = _mm_setzero_si128();
  if (!(imm & 8)) {
    switch (imm & 3) {
      case 0:
        return _mm256_extractf128_si256(a, 0);
      case 1:
        return _mm256_extractf128_si256(a, 1);
      case 2:
        return _mm256_extractf128_si256(b, 0);
      case 3:
        return _mm256_extractf128_si256(b, 1);
    }
  }
  return tmp;
}

inline __m256i mm256_permute2x128_si256(const __m256i& a, const __m256i& b,
                                        const int imm) {
  const int lo_imm = imm & 15;
  __m128i lo = mm_permute_helper(a, b, lo_imm);
  const int hi_imm = (imm >> 4) & 15;
  __m128i hi = mm_permute_helper(a, b, hi_imm);
  return _mm256_set_m128i(hi, lo);
}

inline void Pack8bitColMajorForAvxPacker(const std::int8_t* src_ptr,
                                         std::int8_t input_xor,
                                         const std::int8_t* zerobuf,
                                         int src_stride, int remaining_src_cols,
                                         int src_rows, std::int8_t* packed_ptr,
                                         std::int32_t* sums_ptr,
                                         std::int8_t* trailing_buf) {
  using Layout = PackImpl8bitAvx::Layout;
  RUY_DCHECK_EQ(Layout::kCols, 8);
  RUY_DCHECK_EQ(Layout::kRows, 4);
  // Each Layout::Rows is 4 contiguous input, contiguous packed elements.
  // We process 8 of these chunks at a time, padding short input chunks.
  constexpr int kNumRowChunks = 8;
  constexpr int kNumChunkedSrcRows = kNumRowChunks * Layout::kRows;

  const std::int8_t* src_ptr0 = src_ptr;
  const std::int8_t* src_ptr1 = src_ptr0 + src_stride;
  const std::int8_t* src_ptr2 = src_ptr1 + src_stride;
  const std::int8_t* src_ptr3 = src_ptr2 + src_stride;
  const std::int8_t* src_ptr4 = src_ptr3 + src_stride;
  const std::int8_t* src_ptr5 = src_ptr4 + src_stride;
  const std::int8_t* src_ptr6 = src_ptr5 + src_stride;
  const std::int8_t* src_ptr7 = src_ptr6 + src_stride;
  std::int64_t src_inc0 = kNumChunkedSrcRows;
  std::int64_t src_inc1 = kNumChunkedSrcRows;
  std::int64_t src_inc2 = kNumChunkedSrcRows;
  std::int64_t src_inc3 = kNumChunkedSrcRows;
  std::int64_t src_inc4 = kNumChunkedSrcRows;
  std::int64_t src_inc5 = kNumChunkedSrcRows;
  std::int64_t src_inc6 = kNumChunkedSrcRows;
  std::int64_t src_inc7 = kNumChunkedSrcRows;
  // Handle cases where source does not have Layout::kCols (8) columns.
  if (remaining_src_cols < 8) {
    if (remaining_src_cols <= 0) {
      src_ptr0 = zerobuf;
      src_inc0 = 0;
    }
    if (remaining_src_cols <= 1) {
      src_ptr1 = zerobuf;
      src_inc1 = 0;
    }
    if (remaining_src_cols <= 2) {
      src_ptr2 = zerobuf;
      src_inc2 = 0;
    }
    if (remaining_src_cols <= 3) {
      src_ptr3 = zerobuf;
      src_inc3 = 0;
    }
    if (remaining_src_cols <= 4) {
      src_ptr4 = zerobuf;
      src_inc4 = 0;
    }
    if (remaining_src_cols <= 5) {
      src_ptr5 = zerobuf;
      src_inc5 = 0;
    }
    if (remaining_src_cols <= 6) {
      src_ptr6 = zerobuf;
      src_inc6 = 0;
    }
    src_ptr7 = zerobuf;
    src_inc7 = 0;
  }

  const std::int8_t zero_point = zerobuf[0];

  if (sums_ptr) {
    // i: Layout::kCols.
    for (int i = 0; i < 8; ++i) {
      sums_ptr[i] = 0;
    }
  }
  std::int32_t sums_adjustment = 0;
  const __m256i ones_16bit = _mm256_set1_epi16(1);
  __m256i sums_4x2_32bit_lo = _mm256_set1_epi32(0);
  __m256i sums_4x2_32bit_hi = _mm256_set1_epi32(0);

  // The overall packing effectively pads the source rows to
  // (src_rows + 63) & ~63. The iteration over k may skip when m=1, and then we
  // only pack for (src_rows + 31) & ~31. When there is an incomplete
  // destination block, this is stored into trailing_buf instead of packed_ptr.
  for (int k = 0; k < src_rows; k += kNumChunkedSrcRows) {
    // Available source rows.
    // If this is less than 0 (for m=1), we skip, having filled trailing
    // buffer for m=0. Also, if source rows is zero on m=1, then we filled
    // exactly to the end of the column in the packed buffer.
    const int available_src_rows = src_rows - k;
    // Effectively,
    // available rows = std::max(0, std::min(8, src_rows - k));
    // treat each case separately.
    if (available_src_rows >= kNumChunkedSrcRows) {
      if (sums_ptr) {
        __m256i t0, t1, t2, t3, t4, t5, t6, t7;
        __m256i r0, r1, r2, r3, r4, r5, r6, r7;
        const __m256i input_xor_v = _mm256_set1_epi8(input_xor);

        t0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr0));
        t4 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr4));
        t1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr1));
        t5 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr5));
        t2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr2));
        t6 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr6));
        t3 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr3));
        t7 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr7));

        r0 = mm256_unpacklo_epi32(t0, t1);
        r4 = mm256_unpacklo_epi32(t4, t5);
        r2 = mm256_unpackhi_epi32(t0, t1);
        r6 = mm256_unpackhi_epi32(t4, t5);
        r1 = mm256_unpacklo_epi32(t2, t3);
        r5 = mm256_unpacklo_epi32(t6, t7);
        r3 = mm256_unpackhi_epi32(t2, t3);
        r7 = mm256_unpackhi_epi32(t6, t7);

        t0 = mm256_unpacklo_epi64(r0, r1);
        t4 = mm256_unpacklo_epi64(r4, r5);
        t2 = mm256_unpackhi_epi64(r0, r1);
        t6 = mm256_unpackhi_epi64(r4, r5);
        t1 = mm256_unpacklo_epi64(r2, r3);
        t5 = mm256_unpacklo_epi64(r6, r7);
        t3 = mm256_unpackhi_epi64(r2, r3);
        t7 = mm256_unpackhi_epi64(r6, r7);

        // The preceding sets of rearrangement operations interleaved by 4 bytes
        // and then by 8 bytes *within* lanes. The following set interleave by
        // 16 bytes (128-bit), operating *between* AVX lanes. For instance (t0,
        // t4) are interleaved to create (r0, r1). This complexity follows from
        // the way that AVX is centered around MM 128-bit lanes.
        r0 = mm256_permute2x128_si256(t0, t4, 0x20);
        r4 = mm256_permute2x128_si256(t1, t5, 0x20);
        r1 = mm256_permute2x128_si256(t0, t4, 0x31);
        r5 = mm256_permute2x128_si256(t1, t5, 0x31);
        r2 = mm256_permute2x128_si256(t2, t6, 0x20);
        r6 = mm256_permute2x128_si256(t3, t7, 0x20);
        r3 = mm256_permute2x128_si256(t2, t6, 0x31);
        r7 = mm256_permute2x128_si256(t3, t7, 0x31);

        r0 = mm256_xor_si256(r0, input_xor_v);
        r1 = mm256_xor_si256(r1, input_xor_v);
        r2 = mm256_xor_si256(r2, input_xor_v);
        r3 = mm256_xor_si256(r3, input_xor_v);
        r4 = mm256_xor_si256(r4, input_xor_v);
        r5 = mm256_xor_si256(r5, input_xor_v);
        r6 = mm256_xor_si256(r6, input_xor_v);
        r7 = mm256_xor_si256(r7, input_xor_v);

        __m256i sums_4x4_16bit_lo;
        sums_4x4_16bit_lo = mm256_cvtepi8_epi16(_mm256_castsi256_si128(r0));
        sums_4x4_16bit_lo = mm256_add_epi16(
            sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r1)));
        sums_4x4_16bit_lo = mm256_add_epi16(
            sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r2)));
        sums_4x4_16bit_lo = mm256_add_epi16(
            sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r3)));
        sums_4x4_16bit_lo = mm256_add_epi16(
            sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r4)));
        sums_4x4_16bit_lo = mm256_add_epi16(
            sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r5)));
        sums_4x4_16bit_lo = mm256_add_epi16(
            sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r6)));
        sums_4x4_16bit_lo = mm256_add_epi16(
            sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r7)));

        // The sums have been performed across columns, and now we have 4x16-bit
        // sums packed together. We use madd for pairwise 32-bit sums.
        const __m256i sums_4x2_32bit_lo_new =
            mm256_madd_epi16(sums_4x4_16bit_lo, ones_16bit);
        sums_4x2_32bit_lo =
            mm256_add_epi32(sums_4x2_32bit_lo, sums_4x2_32bit_lo_new);

        __m256i sums_4x4_16bit_hi;
        sums_4x4_16bit_hi = mm256_cvtepi8_epi16(mm256_extracti128_si256(r0, 1));
        sums_4x4_16bit_hi = mm256_add_epi16(
            sums_4x4_16bit_hi,
            mm256_cvtepi8_epi16(mm256_extracti128_si256(r1, 1)));
        sums_4x4_16bit_hi = mm256_add_epi16(
            sums_4x4_16bit_hi,
            mm256_cvtepi8_epi16(mm256_extracti128_si256(r2, 1)));
        sums_4x4_16bit_hi = mm256_add_epi16(
            sums_4x4_16bit_hi,
            mm256_cvtepi8_epi16(mm256_extracti128_si256(r3, 1)));
        sums_4x4_16bit_hi = mm256_add_epi16(
            sums_4x4_16bit_hi,
            mm256_cvtepi8_epi16(mm256_extracti128_si256(r4, 1)));
        sums_4x4_16bit_hi = mm256_add_epi16(
            sums_4x4_16bit_hi,
            mm256_cvtepi8_epi16(mm256_extracti128_si256(r5, 1)));
        sums_4x4_16bit_hi = mm256_add_epi16(
            sums_4x4_16bit_hi,
            mm256_cvtepi8_epi16(mm256_extracti128_si256(r6, 1)));
        sums_4x4_16bit_hi = mm256_add_epi16(
            sums_4x4_16bit_hi,
            mm256_cvtepi8_epi16(mm256_extracti128_si256(r7, 1)));

        const __m256i sums_4x2_32bit_hi_new =
            mm256_madd_epi16(sums_4x4_16bit_hi, ones_16bit);
        sums_4x2_32bit_hi =
            mm256_add_epi32(sums_4x2_32bit_hi, sums_4x2_32bit_hi_new);

        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 0 * 8 * 4),
                            r0);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 2 * 8 * 4),
                            r4);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 4 * 8 * 4),
                            r1);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 6 * 8 * 4),
                            r5);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 1 * 8 * 4),
                            r2);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 3 * 8 * 4),
                            r6);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 5 * 8 * 4),
                            r3);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 7 * 8 * 4),
                            r7);
      } else {
        __m256i t0, t1, t2, t3, t4, t5, t6, t7;
        __m256i r0, r1, r2, r3, r4, r5, r6, r7;
        const __m256i input_xor_v = _mm256_set1_epi8(input_xor);

        t0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr0));
        t4 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr4));
        t1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr1));
        t5 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr5));
        t2 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr2));
        t6 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr6));
        t3 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr3));
        t7 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src_ptr7));

        r0 = mm256_unpacklo_epi32(t0, t1);
        r4 = mm256_unpacklo_epi32(t4, t5);
        r2 = mm256_unpackhi_epi32(t0, t1);
        r6 = mm256_unpackhi_epi32(t4, t5);
        r1 = mm256_unpacklo_epi32(t2, t3);
        r5 = mm256_unpacklo_epi32(t6, t7);
        r3 = mm256_unpackhi_epi32(t2, t3);
        r7 = mm256_unpackhi_epi32(t6, t7);

        t0 = mm256_unpacklo_epi64(r0, r1);
        t4 = mm256_unpacklo_epi64(r4, r5);
        t2 = mm256_unpackhi_epi64(r0, r1);
        t6 = mm256_unpackhi_epi64(r4, r5);
        t1 = mm256_unpacklo_epi64(r2, r3);
        t5 = mm256_unpacklo_epi64(r6, r7);
        t3 = mm256_unpackhi_epi64(r2, r3);
        t7 = mm256_unpackhi_epi64(r6, r7);

        // The preceding sets of rearrangement operations interleaved by 4 bytes
        // and then by 8 bytes *within* lanes. The following set interleave by
        // 16 bytes (128-bit), operating *between* AVX lanes. For instance (t0,
        // t4) are interleaved to create (r0, r1). This complexity follows from
        // the way that AVX is centered around MM 128-bit lanes.
        r0 = mm256_permute2x128_si256(t0, t4, 0x20);
        r4 = mm256_permute2x128_si256(t1, t5, 0x20);
        r1 = mm256_permute2x128_si256(t0, t4, 0x31);
        r5 = mm256_permute2x128_si256(t1, t5, 0x31);
        r2 = mm256_permute2x128_si256(t2, t6, 0x20);
        r6 = mm256_permute2x128_si256(t3, t7, 0x20);
        r3 = mm256_permute2x128_si256(t2, t6, 0x31);
        r7 = mm256_permute2x128_si256(t3, t7, 0x31);

        r0 = mm256_xor_si256(r0, input_xor_v);
        r1 = mm256_xor_si256(r1, input_xor_v);
        r2 = mm256_xor_si256(r2, input_xor_v);
        r3 = mm256_xor_si256(r3, input_xor_v);
        r4 = mm256_xor_si256(r4, input_xor_v);
        r5 = mm256_xor_si256(r5, input_xor_v);
        r6 = mm256_xor_si256(r6, input_xor_v);
        r7 = mm256_xor_si256(r7, input_xor_v);

        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 0 * 8 * 4),
                            r0);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 2 * 8 * 4),
                            r4);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 4 * 8 * 4),
                            r1);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 6 * 8 * 4),
                            r5);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 1 * 8 * 4),
                            r2);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 3 * 8 * 4),
                            r6);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 5 * 8 * 4),
                            r3);
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(packed_ptr + 7 * 8 * 4),
                            r7);
      }
    } else if (available_src_rows > 0) {
      RUY_DCHECK_LT(available_src_rows, kNumChunkedSrcRows);
      // We do not care what goes into the trailing buffer, but we want
      // in_data[...] ^ input_xor == 0 for irrelevant values in the summation.
      //
      // We compensate for padding-with-zero_point by initializing the
      // summations with the compensating offset, effectively
      // ((input_xor ^ input_xor) - (zero_point ^ input_xor)) *
      //                         4 * (8 - ((available_src_rows + 3) >> 2)).
      //
      // Note that (zero_point ^ input_xor) is performed in 8-bits and then
      // cast.
      sums_adjustment +=
          -(zero_point ^ input_xor) * 4 * (8 - ((available_src_rows + 3) >> 2));

      __m256i t0, t1, t2, t3, t4, t5, t6, t7;
      __m256i r0, r1, r2, r3, r4, r5, r6, r7;
      const __m256i input_xor_v = _mm256_set1_epi8(input_xor);

      t0 = MaskLoadu<Path::kAvx>(available_src_rows, zero_point, src_ptr0);
      t4 = MaskLoadu<Path::kAvx>(available_src_rows, zero_point, src_ptr4);
      t1 = MaskLoadu<Path::kAvx>(available_src_rows, zero_point, src_ptr1);
      t5 = MaskLoadu<Path::kAvx>(available_src_rows, zero_point, src_ptr5);
      t2 = MaskLoadu<Path::kAvx>(available_src_rows, zero_point, src_ptr2);
      t6 = MaskLoadu<Path::kAvx>(available_src_rows, zero_point, src_ptr6);
      t3 = MaskLoadu<Path::kAvx>(available_src_rows, zero_point, src_ptr3);
      t7 = MaskLoadu<Path::kAvx>(available_src_rows, zero_point, src_ptr7);

      r0 = mm256_unpacklo_epi32(t0, t1);
      r4 = mm256_unpacklo_epi32(t4, t5);
      r2 = mm256_unpackhi_epi32(t0, t1);
      r6 = mm256_unpackhi_epi32(t4, t5);
      r1 = mm256_unpacklo_epi32(t2, t3);
      r5 = mm256_unpacklo_epi32(t6, t7);
      r3 = mm256_unpackhi_epi32(t2, t3);
      r7 = mm256_unpackhi_epi32(t6, t7);

      t0 = mm256_unpacklo_epi64(r0, r1);
      t4 = mm256_unpacklo_epi64(r4, r5);
      t2 = mm256_unpackhi_epi64(r0, r1);
      t6 = mm256_unpackhi_epi64(r4, r5);
      t1 = mm256_unpacklo_epi64(r2, r3);
      t5 = mm256_unpacklo_epi64(r6, r7);
      t3 = mm256_unpackhi_epi64(r2, r3);
      t7 = mm256_unpackhi_epi64(r6, r7);

      // The preceding sets of rearrangement operations interleaved by 4 bytes
      // and then by 8 bytes *within* lanes. The following set interleave by
      // 16 bytes (128-bit), operating *between* AVX lanes. For instance (t0,
      // t4) are interleaved to create (r0, r1). This complexity follows from
      // the way that AVX is centered around MM 128-bit lanes.
      r0 = mm256_permute2x128_si256(t0, t4, 0x20);
      r4 = mm256_permute2x128_si256(t1, t5, 0x20);
      r1 = mm256_permute2x128_si256(t0, t4, 0x31);
      r5 = mm256_permute2x128_si256(t1, t5, 0x31);
      r2 = mm256_permute2x128_si256(t2, t6, 0x20);
      r6 = mm256_permute2x128_si256(t3, t7, 0x20);
      r3 = mm256_permute2x128_si256(t2, t6, 0x31);
      r7 = mm256_permute2x128_si256(t3, t7, 0x31);

      r0 = mm256_xor_si256(r0, input_xor_v);
      r1 = mm256_xor_si256(r1, input_xor_v);
      r2 = mm256_xor_si256(r2, input_xor_v);
      r3 = mm256_xor_si256(r3, input_xor_v);
      r4 = mm256_xor_si256(r4, input_xor_v);
      r5 = mm256_xor_si256(r5, input_xor_v);
      r6 = mm256_xor_si256(r6, input_xor_v);
      r7 = mm256_xor_si256(r7, input_xor_v);

      __m256i sums_4x4_16bit_lo;
      sums_4x4_16bit_lo = mm256_cvtepi8_epi16(_mm256_castsi256_si128(r0));
      sums_4x4_16bit_lo = mm256_add_epi16(
          sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r1)));
      sums_4x4_16bit_lo = mm256_add_epi16(
          sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r2)));
      sums_4x4_16bit_lo = mm256_add_epi16(
          sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r3)));
      sums_4x4_16bit_lo = mm256_add_epi16(
          sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r4)));
      sums_4x4_16bit_lo = mm256_add_epi16(
          sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r5)));
      sums_4x4_16bit_lo = mm256_add_epi16(
          sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r6)));
      sums_4x4_16bit_lo = mm256_add_epi16(
          sums_4x4_16bit_lo, mm256_cvtepi8_epi16(_mm256_castsi256_si128(r7)));

      // The sums have been performed across columns, and now we have 4x16-bit
      // sums packed together. We use madd for pairwise 32-bit sums.
      const __m256i sums_4x2_32bit_lo_new =
          mm256_madd_epi16(sums_4x4_16bit_lo, ones_16bit);
      sums_4x2_32bit_lo =
          mm256_add_epi32(sums_4x2_32bit_lo, sums_4x2_32bit_lo_new);

      __m256i sums_4x4_16bit_hi;
      sums_4x4_16bit_hi = mm256_cvtepi8_epi16(mm256_extracti128_si256(r0, 1));
      sums_4x4_16bit_hi =
          mm256_add_epi16(sums_4x4_16bit_hi,
                          mm256_cvtepi8_epi16(mm256_extracti128_si256(r1, 1)));
      sums_4x4_16bit_hi =
          mm256_add_epi16(sums_4x4_16bit_hi,
                          mm256_cvtepi8_epi16(mm256_extracti128_si256(r2, 1)));
      sums_4x4_16bit_hi =
          mm256_add_epi16(sums_4x4_16bit_hi,
                          mm256_cvtepi8_epi16(mm256_extracti128_si256(r3, 1)));
      sums_4x4_16bit_hi =
          mm256_add_epi16(sums_4x4_16bit_hi,
                          mm256_cvtepi8_epi16(mm256_extracti128_si256(r4, 1)));
      sums_4x4_16bit_hi =
          mm256_add_epi16(sums_4x4_16bit_hi,
                          mm256_cvtepi8_epi16(mm256_extracti128_si256(r5, 1)));
      sums_4x4_16bit_hi =
          mm256_add_epi16(sums_4x4_16bit_hi,
                          mm256_cvtepi8_epi16(mm256_extracti128_si256(r6, 1)));
      sums_4x4_16bit_hi =
          mm256_add_epi16(sums_4x4_16bit_hi,
                          mm256_cvtepi8_epi16(mm256_extracti128_si256(r7, 1)));

      const __m256i sums_4x2_32bit_hi_new =
          mm256_madd_epi16(sums_4x4_16bit_hi, ones_16bit);
      sums_4x2_32bit_hi =
          mm256_add_epi32(sums_4x2_32bit_hi, sums_4x2_32bit_hi_new);

      _mm256_storeu_si256(reinterpret_cast<__m256i*>(trailing_buf + 0 * 8 * 4),
                          r0);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(trailing_buf + 2 * 8 * 4),
                          r4);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(trailing_buf + 4 * 8 * 4),
                          r1);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(trailing_buf + 6 * 8 * 4),
                          r5);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(trailing_buf + 1 * 8 * 4),
                          r2);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(trailing_buf + 3 * 8 * 4),
                          r6);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(trailing_buf + 5 * 8 * 4),
                          r3);
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(trailing_buf + 7 * 8 * 4),
                          r7);
    }

    packed_ptr += 8 * kNumChunkedSrcRows;
    src_ptr0 += src_inc0;
    src_ptr1 += src_inc1;
    src_ptr2 += src_inc2;
    src_ptr3 += src_inc3;
    src_ptr4 += src_inc4;
    src_ptr5 += src_inc5;
    src_ptr6 += src_inc6;
    src_ptr7 += src_inc7;
  }

  if (sums_ptr) {
    const __m256i sums_adjustment_v = _mm256_set1_epi32(sums_adjustment);

    __m256i sums =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(sums_ptr));

    // We earlier used madd for pairwise 32-bit sums, and now we deinterlace the
    // neighbours, finshing up by adding them to the stored accumulated sums.
    const __m256i sums_2x4_32bit_lo = PermuteEpi32EvenOdds(sums_4x2_32bit_lo);
    const __m256i sums_2x4_32bit_hi = PermuteEpi32EvenOdds(sums_4x2_32bit_hi);
    const __m256i sums_2x4_32bit_a =
        mm256_permute2x128_si256(sums_2x4_32bit_lo, sums_2x4_32bit_hi, 0x20);
    const __m256i sums_2x4_32bit_b =
        mm256_permute2x128_si256(sums_2x4_32bit_lo, sums_2x4_32bit_hi, 0x31);
    sums = mm256_add_epi32(sums, sums_adjustment_v);
    sums = mm256_add_epi32(sums, sums_2x4_32bit_a);
    sums = mm256_add_epi32(sums, sums_2x4_32bit_b);

    _mm256_storeu_si256(reinterpret_cast<__m256i*>(sums_ptr), sums);
  }
}

// Use a generic AVX intrinsic for greater-than comparison.
template <>
inline __m256i CompareGreaterThan<Path::kAvx>(const __m256i& a,
                                              const __m256i& b) {
  constexpr int kGreaterThanSignalling = 14;
  const __m256 v = _mm256_cmp_ps(_mm256_cvtepi32_ps(a), _mm256_cvtepi32_ps(b),
                                 kGreaterThanSignalling);
  return _mm256_cvtps_epi32(v);
}

}  // namespace.

void Pack8bitColMajorForAvx(const std::int8_t* src_ptr, std::int8_t input_xor,
                            const std::int8_t* zerobuf, int src_stride,
                            int remaining_src_cols, int src_rows,
                            std::int8_t* packed_ptr, std::int32_t* sums_ptr) {
  profiler::ScopeLabel label("Pack kAvx 8bit");

  using Layout = PackImpl8bitAvx::Layout;
  RUY_DCHECK_EQ(Layout::kCols, 8);
  RUY_DCHECK_EQ(Layout::kRows, 4);

  // Each Layout::Rows is 4 contiguous input, contiguous packed elements.
  // We process 8 of these chunks at a time, padding short input chunks.
  static constexpr int kNumRowChunks = 8;  // Short input is padded.

  // Each packed block is 4*8, and there are normally 8. The trailing block is
  // only slightly shorter.
  constexpr int kTrailingBufSize =
      kNumRowChunks * Layout::kCols * Layout::kRows;
  std::int8_t trailing_buf[kTrailingBufSize];
  memset(trailing_buf, 0, kTrailingBufSize * sizeof(std::int8_t));

  Pack8bitColMajorForAvxPacker(src_ptr, input_xor, zerobuf, src_stride,
                               remaining_src_cols, src_rows, packed_ptr,
                               sums_ptr, trailing_buf);

  constexpr int kChunkedRowMask = kNumRowChunks * Layout::kRows - 1;
  const bool trailing_data = (src_rows & kChunkedRowMask) > 0;
  // If the number of source rows is not a multiple of kChunkedRowMask, there
  // will be data in the trailing buffer,
  if (trailing_data) {
    const int non_trailing_rows = src_rows & ~kChunkedRowMask;
    // Destination "rows" are padded to next highest multiple of Layout::kRows.
    const int dst_rows = (src_rows + 3) & ~3;
    const int trailing_rows = dst_rows - non_trailing_rows;
    memcpy(packed_ptr + Layout::kCols * non_trailing_rows, trailing_buf,
           Layout::kCols * trailing_rows * sizeof(std::int8_t));
  }
}

void PackFloatColMajorForAvx(const float* src_ptr, const float* zerobuf,
                             int src_stride, int remaining_src_cols,
                             int src_rows, float* packed_ptr) {
  profiler::ScopeLabel label("Pack kAvx float");
  static constexpr int kPackCols = 8;  // Source cols packed together.
  static constexpr int kPackRows = 8;  // Short input is padded.
  float trailing_buf[(kPackRows - 1) * kPackCols];
  if (remaining_src_cols < 8) {
    memset(trailing_buf, 0, sizeof(trailing_buf));
  }
  PackFloatColMajorForAvxCommonPacker<PackImplFloatAvx, Path::kAvx>(
      src_ptr, zerobuf, src_stride, remaining_src_cols, src_rows, packed_ptr,
      trailing_buf);

  const int trailing_rows = src_rows & (kPackRows - 1);
  if (trailing_rows > 0) {
    const int non_trailing_rows = src_rows & ~(kPackRows - 1);
    memcpy(packed_ptr + kPackCols * non_trailing_rows, trailing_buf,
           kPackCols * trailing_rows * sizeof(float));
  }
}

void Pack8bitRowMajorForAvx(const std::uint8_t* src_ptr, int src_stride,
                            int src_zero_point, std::int8_t* packed_ptr,
                            int packed_stride, int start_col, int end_col,
                            int src_cols, int block_row, int src_rows,
                            int input_xor, std::int32_t* sums) {
  int col = start_col;
  int src_end_col = std::min(end_col, src_cols);

  for (; col <= src_end_col - 8; col += 8) {
    std::int8_t* dst_ptr = packed_ptr;
    __m128i val0, val1, val2, val3;
    __m128i input_xor_dup = _mm_set1_epi8(input_xor);
    // Load a 4x8 block.
    if (block_row + 4 <= src_rows) {
      val0 = _mm_loadu_si64(src_ptr + 0 * src_stride);
      val1 = _mm_loadu_si64(src_ptr + 1 * src_stride);
      val2 = _mm_loadu_si64(src_ptr + 2 * src_stride);
      val3 = _mm_loadu_si64(src_ptr + 3 * src_stride);
    } else {
      val0 = _mm_set1_epi8(src_zero_point);
      val1 = val0;
      val2 = val0;
      val3 = val0;
      if (block_row + 0 < src_rows)
        val0 = _mm_loadu_si64(src_ptr + 0 * src_stride);
      if (block_row + 1 < src_rows)
        val1 = _mm_loadu_si64(src_ptr + 1 * src_stride);
      if (block_row + 2 < src_rows)
        val2 = _mm_loadu_si64(src_ptr + 2 * src_stride);
      if (block_row + 3 < src_rows)
        val3 = _mm_loadu_si64(src_ptr + 3 * src_stride);
    }
    // Maybe xor the sign bit to convert from uint8 to int8.
    val0 = _mm_xor_si128(val0, input_xor_dup);
    val1 = _mm_xor_si128(val1, input_xor_dup);
    val2 = _mm_xor_si128(val2, input_xor_dup);
    val3 = _mm_xor_si128(val3, input_xor_dup);
    // Update the sums.
    __m128i val16_0 = _mm_cvtepi8_epi16(val0);
    __m128i val16_1 = _mm_cvtepi8_epi16(val1);
    __m128i val16_2 = _mm_cvtepi8_epi16(val2);
    __m128i val16_3 = _mm_cvtepi8_epi16(val3);
    __m128i new_sum16 = _mm_add_epi16(_mm_add_epi16(val16_0, val16_1),
                                      _mm_add_epi16(val16_2, val16_3));
    __m256i sum =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(sums + col));
    sum = mm256_add_epi32(sum, mm256_cvtepi16_epi32(new_sum16));
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(sums + col), sum);
    // Perform the transposition of 4x4 blocks
    __m128i t2_val0 = _mm_unpacklo_epi8(val0, val1);
    __m128i t2_val1 = _mm_unpacklo_epi8(val2, val3);
    __m128i t4_val0 = _mm_unpacklo_epi16(t2_val0, t2_val1);
    __m128i t4_val1 = _mm_unpackhi_epi16(t2_val0, t2_val1);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst_ptr), t4_val0);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(dst_ptr + 16), t4_val1);
    src_ptr += 8;
    packed_ptr += packed_stride * 8;
  }
  for (; col < src_end_col; col++) {
    std::int32_t accum = 0;
    for (int r = 0; r < 4; r++) {
      std::int8_t packed_val;
      if (block_row + r < src_rows) {
        packed_val = input_xor ^ src_ptr[r * src_stride];
      } else {
        packed_val = input_xor ^ src_zero_point;
      }
      accum += packed_val;
      *packed_ptr++ = packed_val;
    }
    if (sums) {
      sums[col] += accum;
    }
    src_ptr++;
  }
  for (; col < end_col; col++) {
    std::memset(packed_ptr, 0, 4);
    packed_ptr += 4;
  }
}

#endif  // RUY_PLATFORM_AVX && RUY_OPT(INTRINSICS)

}  // namespace ruy
