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

#include "ruy/pack_arm.h"

#include <cstdint>

#include "ruy/asm_helpers.h"
#include "ruy/opt_set.h"
#include "ruy/pack_common.h"
#include "ruy/platform.h"
#include "ruy/profiler/instrumentation.h"

#if RUY_PLATFORM_NEON
#include <arm_neon.h>
#endif

namespace ruy {

#if RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)

void Pack8bitColMajorForNeon(const void* src_ptr0, const void* src_ptr1,
                             const void* src_ptr2, const void* src_ptr3,
                             int src_inc0, int src_inc1, int src_inc2,
                             int src_inc3, int src_rows, int src_zero_point,
                             std::int8_t* packed_ptr, std::int32_t* sums_ptr,
                             int input_xor) {
  profiler::ScopeLabel label("Pack (kNeon)");
  asm volatile(
      // clang-format off
          // v26 will be the vector to XOR input values with to perform
          // any input data type conversion (e.g. uint8 to int8).
          "dup v26.16b, %w[input_xor]\n"
          // w1 will be the number of rows already loaded.
          "mov w1, #0\n"
          // v28--v32 will be used to accumulate the sums
          "movi v28.4s, #0\n"
          "movi v29.4s, #0\n"
          "movi v30.4s, #0\n"
          "movi v31.4s, #0\n"
          // Let w2 be `rows` rounded down to multiple of 16.
          "ands w2, %w[rows], #-16\n"
          // If there are no full blocks of 16 rows to process, jump to the
          // code handling the last < 16 rows.
          "beq 3f\n"
          // Load the first block of 16 rows.
          "add w1, w1, #16\n"
          "ld1 {v0.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v1.16b}, [%[src_ptr1]], %[src_inc1]\n"
          // Check if these were the only full block of 16 rows to load.
          "cmp w1, w2\n"
          "ld1 {v2.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v3.16b}, [%[src_ptr3]], %[src_inc3]\n"
          // In that case, jump to the code handling the last loaded block of
          // 16 rows.
          "beq 2f\n"
          // Main loop processing blocks of 16 rows.
          "1:\n"
          // Load the next 16 rows, interleaved with the XOR input type
          // conversion (e.g. uint8->int8) on the already loaded inputs.
          "add w1, w1, #16\n"
          "eor v4.16b, v0.16b, v26.16b\n"
          "ld1 {v0.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "eor v5.16b, v1.16b, v26.16b\n"
          "ld1 {v1.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "eor v6.16b, v2.16b, v26.16b\n"
          "ld1 {v2.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "eor v7.16b, v3.16b, v26.16b\n"
          "ld1 {v3.16b}, [%[src_ptr3]], %[src_inc3]\n"
          // Compute the sums, interleaved with storing to the packed matrix.
          "saddlp v16.8h, v4.16b\n"
          "str q4, [%[packed_ptr], #0]\n"
          "saddlp v17.8h, v5.16b\n"
          "str q5, [%[packed_ptr], #16]\n"
          "saddlp v18.8h, v6.16b\n"
          "str q6, [%[packed_ptr], #32]\n"
          "saddlp v19.8h, v7.16b\n"
          "str q7, [%[packed_ptr], #48]\n"
          "sadalp v28.4s, v16.8h\n"
          // Was this the last block of 16 rows to load?
          "cmp w1, w2\n"
          "sadalp v29.4s, v17.8h\n"
          "add %[packed_ptr], %[packed_ptr], #64\n"
          "sadalp v30.4s, v18.8h\n"
          "sadalp v31.4s, v19.8h\n"
          // End of main loop on blocks of 16 rows.
          "bne 1b\n"

          // Code handling the last already-loaded block of 16 rows.
          "2:\n"

          // Process the last loaded full 16x4 block.
          "eor v4.16b, v0.16b, v26.16b\n"
          "eor v5.16b, v1.16b, v26.16b\n"
          "eor v6.16b, v2.16b, v26.16b\n"
          "eor v7.16b, v3.16b, v26.16b\n"

          "saddlp v16.8h, v4.16b\n"
          "str q4, [%[packed_ptr], #0]\n"
          "saddlp v17.8h, v5.16b\n"
          "str q5, [%[packed_ptr], #16]\n"
          "saddlp v18.8h, v6.16b\n"
          "str q6, [%[packed_ptr], #32]\n"
          "saddlp v19.8h, v7.16b\n"
          "str q7, [%[packed_ptr], #48]\n"
          "sadalp v28.4s, v16.8h\n"
          "sadalp v29.4s, v17.8h\n"
          "sadalp v30.4s, v18.8h\n"
          "sadalp v31.4s, v19.8h\n"

          "add %[packed_ptr], %[packed_ptr], #64\n"

          // End of code handling full blocks of 16 rows.
          // Now we handle any remaining rows.
          "3:\n"
          // Let w2 be the number of rows left to handle.
          "ands w2, %w[rows], #15\n"
          // If w2==0, there are no remaining rows, jump to the end.
          "beq 4f\n"
          // Zero out a 16x4 block in registers, which we'll partially overwrite
          // with any remaining rows.
          "dup v0.16b, %w[src_zero_point]\n"
          "dup v1.16b, %w[src_zero_point]\n"
          "dup v2.16b, %w[src_zero_point]\n"
          "dup v3.16b, %w[src_zero_point]\n"
#define RUY_LOAD_ONE_ROW(R)                   \
  "cmp w2, #" #R "\n"                         \
  "beq 5f\n"                                  \
  "ld1 { v0.b }[" #R "], [%[src_ptr0]], #1\n" \
  "ld1 { v1.b }[" #R "], [%[src_ptr1]], #1\n" \
  "ld1 { v2.b }[" #R "], [%[src_ptr2]], #1\n" \
  "ld1 { v3.b }[" #R "], [%[src_ptr3]], #1\n"

          RUY_LOAD_ONE_ROW(0)
          RUY_LOAD_ONE_ROW(1)
          RUY_LOAD_ONE_ROW(2)
          RUY_LOAD_ONE_ROW(3)
          RUY_LOAD_ONE_ROW(4)
          RUY_LOAD_ONE_ROW(5)
          RUY_LOAD_ONE_ROW(6)
          RUY_LOAD_ONE_ROW(7)
          RUY_LOAD_ONE_ROW(8)
          RUY_LOAD_ONE_ROW(9)
          RUY_LOAD_ONE_ROW(10)
          RUY_LOAD_ONE_ROW(11)
          RUY_LOAD_ONE_ROW(12)
          RUY_LOAD_ONE_ROW(13)
          RUY_LOAD_ONE_ROW(14)
          // Here we know that w2==15, so RUY_LOAD_ONE_ROW(15) would be a no-op.
#undef RUY_LOAD_ONE_ROW
          "5:\n"

          // Process the last zero-padded 16x4 block.
          "eor v4.16b, v0.16b, v26.16b\n"
          "eor v5.16b, v1.16b, v26.16b\n"
          "eor v6.16b, v2.16b, v26.16b\n"
          "eor v7.16b, v3.16b, v26.16b\n"

          "saddlp v16.8h, v4.16b\n"
          "saddlp v17.8h, v5.16b\n"
          "saddlp v18.8h, v6.16b\n"
          "saddlp v19.8h, v7.16b\n"
          "sadalp v28.4s, v16.8h\n"
          "sadalp v29.4s, v17.8h\n"
          "sadalp v30.4s, v18.8h\n"
          "sadalp v31.4s, v19.8h\n"

          "str q4, [%[packed_ptr], #0]\n"
          "str q5, [%[packed_ptr], #16]\n"
          "str q6, [%[packed_ptr], #32]\n"
          "str q7, [%[packed_ptr], #48]\n"
          "add %[packed_ptr], %[packed_ptr], #64\n"

          "4:\n"

          // Horizontal reduction of the registers used to accumulate sums.
          "addp v28.4s, v28.4s, v29.4s\n"
          "addp v30.4s, v30.4s, v31.4s\n"
          "addp v28.4s, v28.4s, v30.4s\n"

          // Store the sums.
          "cmp %[sums_ptr], #0\n"
          "beq 6f\n"
          "st1 {v28.4s}, [%[sums_ptr]], #16\n"
          "6:\n"
      // clang-format on

      : [src_ptr0] "+r"(src_ptr0), [src_ptr1] "+r"(src_ptr1),
        [src_ptr2] "+r"(src_ptr2), [src_ptr3] "+r"(src_ptr3),
        [packed_ptr] "+r"(packed_ptr), [sums_ptr] "+r"(sums_ptr)
      : [src_inc0] "r"(static_cast<std::int64_t>(src_inc0)),
        [src_inc1] "r"(static_cast<std::int64_t>(src_inc1)),
        [src_inc2] "r"(static_cast<std::int64_t>(src_inc2)),
        [src_inc3] "r"(static_cast<std::int64_t>(src_inc3)),
        [rows] "r"(src_rows), [src_zero_point] "r"(src_zero_point),
        [input_xor] "r"(input_xor)
      : "cc", "memory", "x1", "x2", "v0", "v1", "v2", "v3", "v4", "v5", "v6",
        "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16",
        "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26",
        "v27", "v28", "v29", "v30", "v31");
}
#endif

#if RUY_PLATFORM_NEON_32 && RUY_OPT(ASM)

#define RUY_OFFSET_SRC_PTR0 0
#define RUY_OFFSET_SRC_PTR1 4
#define RUY_OFFSET_SRC_PTR2 8
#define RUY_OFFSET_SRC_PTR3 12
#define RUY_OFFSET_SUMS_PTR 16
#define RUY_OFFSET_PACKED_PTR 20
#define RUY_OFFSET_SRC_INC0 24
#define RUY_OFFSET_SRC_INC1 28
#define RUY_OFFSET_SRC_INC2 32
#define RUY_OFFSET_SRC_INC3 36
#define RUY_OFFSET_SRC_ROWS 40
#define RUY_OFFSET_SRC_ZERO_POINT 44
#define RUY_OFFSET_INPUT_XOR 48

template <typename Params>
void CheckOffsetsInPackParams8bit(const Params&) {
  static_assert(offsetof(Params, src_ptr0) == RUY_OFFSET_SRC_PTR0, "");
  static_assert(offsetof(Params, src_ptr1) == RUY_OFFSET_SRC_PTR1, "");
  static_assert(offsetof(Params, src_ptr2) == RUY_OFFSET_SRC_PTR2, "");
  static_assert(offsetof(Params, src_ptr3) == RUY_OFFSET_SRC_PTR3, "");
  static_assert(offsetof(Params, sums_ptr) == RUY_OFFSET_SUMS_PTR, "");
  static_assert(offsetof(Params, packed_ptr) == RUY_OFFSET_PACKED_PTR, "");
  static_assert(offsetof(Params, src_inc0) == RUY_OFFSET_SRC_INC0, "");
  static_assert(offsetof(Params, src_inc1) == RUY_OFFSET_SRC_INC1, "");
  static_assert(offsetof(Params, src_inc2) == RUY_OFFSET_SRC_INC2, "");
  static_assert(offsetof(Params, src_inc3) == RUY_OFFSET_SRC_INC3, "");
  static_assert(offsetof(Params, src_rows) == RUY_OFFSET_SRC_ROWS, "");
  static_assert(offsetof(Params, src_zero_point) == RUY_OFFSET_SRC_ZERO_POINT,
                "");
  static_assert(offsetof(Params, input_xor) == RUY_OFFSET_INPUT_XOR, "");
}

// No attempt made at making this code efficient on A55-ish cores yet.
void Pack8bitColMajorForNeon4Cols(const PackParams8bit& params) {
  CheckOffsetsInPackParams8bit(params);
  profiler::ScopeLabel label("Pack (kNeon)");
  const void* src_ptr0 = params.src_ptr0;
  const void* src_ptr1 = params.src_ptr1;
  const void* src_ptr2 = params.src_ptr2;
  const void* src_ptr3 = params.src_ptr3;
  const int src_inc0 = params.src_inc0;
  const int src_inc1 = params.src_inc1;
  const int src_inc2 = params.src_inc2;
  const int src_inc3 = params.src_inc3;
  const std::int8_t* packed_ptr = params.packed_ptr;

  asm volatile(
      // clang-format off

          "ldr r2, [%[params], #" RUY_STR(RUY_OFFSET_INPUT_XOR) "]\n"
          "vdup.8 q11, r2\n"
          "mov r1, #0\n"
          // Zero-out the accumulators
          "vmov.i32 q12, #0\n"
          "vmov.i32 q13, #0\n"
          "vmov.i32 q14, #0\n"
          "vmov.i32 q15, #0\n"

          // Round down src_rows to nearest multiple of 16.
          "ldr r3, [%[params], #" RUY_STR(RUY_OFFSET_SRC_ROWS) "]\n"
          "and r2, r3, #-16\n"
          "cmp r1, r2\n"
          "beq 3f\n"

          "1:\n"
          "add r1, r1, #16\n"
          /* Load q0 */
          "vld1.8 {d0, d1}, [%[src_ptr0]]\n"
          "add %[src_ptr0], %[src_ptr0], %[src_inc0]\n"
          RUY_PREFETCH_LOAD("pld [%[src_ptr0]]\n")

          /* Load q1 */
          "vld1.8 {d2, d3}, [%[src_ptr1]]\n"
          "add %[src_ptr1], %[src_ptr1], %[src_inc1]\n"
          RUY_PREFETCH_LOAD("pld [%[src_ptr1]]\n")

          "veor.8 q4, q0, q11\n"
          "veor.8 q5, q1, q11\n"

          // Pairwise add in to 16b accumulators.
          "vpaddl.s8 q8, q4\n"
          "vpaddl.s8 q9, q5\n"

          "vst1.32 {q4}, [%[packed_ptr]]!\n"
          "vst1.32 {q5}, [%[packed_ptr]]!\n"

          // Pairwise add accumulate into 32b accumulators.
          // q12 and q13 contain 4x32b accumulators
          "vpadal.s16 q12, q8\n"
          "vpadal.s16 q13, q9\n"

          // Now do the same for src_ptr2 and src_ptr3.
          "vld1.8 {d0, d1}, [%[src_ptr2]]\n"
          "add %[src_ptr2], %[src_ptr2], %[src_inc2]\n"
          RUY_PREFETCH_LOAD("pld [%[src_ptr2]]\n")

          "vld1.8 {d2, d3}, [%[src_ptr3]]\n"
          "add %[src_ptr3], %[src_ptr3], %[src_inc3]\n"
          RUY_PREFETCH_LOAD("pld [%[src_ptr3]]\n")

          "veor.8 q4, q0, q11\n"
          "veor.8 q5, q1, q11\n"

          "vpaddl.s8 q8, q4\n"
          "vpaddl.s8 q9, q5\n"

          "vst1.32 {q4}, [%[packed_ptr]]!\n"
          "vst1.32 {q5}, [%[packed_ptr]]!\n"

          // Pairwise add accumulate into 32b accumulators.
          // q14 and q15 contain 4x32b accumulators
          "vpadal.s16 q14, q8\n"
          "vpadal.s16 q15, q9\n"

          "cmp r1, r2\n"
          "bne 1b\n"

          "3:\n"

          // Now pack the last (num_rows % 16) rows.
          "ldr r3, [%[params], #" RUY_STR(RUY_OFFSET_SRC_ROWS) "]\n"
          "ands r2, r3, #15\n"
          "beq 4f\n"
          "ldr r3, [%[params], #" RUY_STR(RUY_OFFSET_SRC_ZERO_POINT) "]\n"
          "vdup.8 q0, r3\n"
          "vdup.8 q1, r3\n"

// First, read/accumulate/write for src_ptr0 and src_ptr1.
#define RUY_LOAD_ONE_ROW1(I, R)            \
  "cmp r2, #" #I "\n"                      \
  "beq 5f\n"                               \
  "vld1.8 { d0[" #R "]}, [%[src_ptr0]]!\n" \
  "vld1.8 { d2[" #R "]}, [%[src_ptr1]]!\n" \

          RUY_LOAD_ONE_ROW1(0, 0)
          RUY_LOAD_ONE_ROW1(1, 1)
          RUY_LOAD_ONE_ROW1(2, 2)
          RUY_LOAD_ONE_ROW1(3, 3)
          RUY_LOAD_ONE_ROW1(4, 4)
          RUY_LOAD_ONE_ROW1(5, 5)
          RUY_LOAD_ONE_ROW1(6, 6)
          RUY_LOAD_ONE_ROW1(7, 7)
#undef RUY_LOAD_ONE_ROW1

#define RUY_LOAD_ONE_ROW2(I, R)            \
  "cmp r2, #" #I "\n"                      \
  "beq 5f\n"                               \
  "vld1.8 { d1[" #R "]}, [%[src_ptr0]]!\n" \
  "vld1.8 { d3[" #R "]}, [%[src_ptr1]]!\n" \

          RUY_LOAD_ONE_ROW2(8, 0)
          RUY_LOAD_ONE_ROW2(9, 1)
          RUY_LOAD_ONE_ROW2(10, 2)
          RUY_LOAD_ONE_ROW2(11, 3)
          RUY_LOAD_ONE_ROW2(12, 4)
          RUY_LOAD_ONE_ROW2(13, 5)
          RUY_LOAD_ONE_ROW2(14, 6)
          RUY_LOAD_ONE_ROW2(15, 7)
#undef RUY_LOAD_ONE_ROW2

          "5:\n"

          "veor.16 q4, q0, q11\n"
          "veor.16 q5, q1, q11\n"

          "vpaddl.s8 q8, q4\n"
          "vpaddl.s8 q9, q5\n"

          // Pairwise add accumulate to 4x32b accumulators.
          "vpadal.s16 q12, q8\n"
          "vpadal.s16 q13, q9\n"

          "vst1.32 {q4}, [%[packed_ptr]]!\n"
          "vst1.32 {q5}, [%[packed_ptr]]!\n"

          // Reset to src_zero for src_ptr2 and src_ptr3.
          "vdup.8 q0, r3\n"
          "vdup.8 q1, r3\n"

// Next, read/accumulate/write for src_ptr2 and src_ptr3.
#define RUY_LOAD_ONE_ROW1(I, R)            \
  "cmp r2, #" #I "\n"                      \
  "beq 5f\n"                               \
  "vld1.8 { d0[" #R "]}, [%[src_ptr2]]!\n" \
  "vld1.8 { d2[" #R "]}, [%[src_ptr3]]!\n" \

          RUY_LOAD_ONE_ROW1(0, 0)
          RUY_LOAD_ONE_ROW1(1, 1)
          RUY_LOAD_ONE_ROW1(2, 2)
          RUY_LOAD_ONE_ROW1(3, 3)
          RUY_LOAD_ONE_ROW1(4, 4)
          RUY_LOAD_ONE_ROW1(5, 5)
          RUY_LOAD_ONE_ROW1(6, 6)
          RUY_LOAD_ONE_ROW1(7, 7)
#undef RUY_LOAD_ONE_ROW1

#define RUY_LOAD_ONE_ROW2(I, R)            \
  "cmp r2, #" #I "\n"                      \
  "beq 5f\n"                               \
  "vld1.8 { d1[" #R "]}, [%[src_ptr2]]!\n" \
  "vld1.8 { d3[" #R "]}, [%[src_ptr3]]!\n" \

          RUY_LOAD_ONE_ROW2(8, 0)
          RUY_LOAD_ONE_ROW2(9, 1)
          RUY_LOAD_ONE_ROW2(10, 2)
          RUY_LOAD_ONE_ROW2(11, 3)
          RUY_LOAD_ONE_ROW2(12, 4)
          RUY_LOAD_ONE_ROW2(13, 5)
          RUY_LOAD_ONE_ROW2(14, 6)
          RUY_LOAD_ONE_ROW2(15, 7)
#undef RUY_LOAD_ONE_ROW2

          "5:\n"

          "veor.16 q4, q0, q11\n"
          "veor.16 q5, q1, q11\n"

          "vpaddl.s8 q8, q4\n"
          "vpaddl.s8 q9, q5\n"

          // Pairwise add accumulate to 4x32b accumulators.
          "vpadal.s16 q14, q8\n"
          "vpadal.s16 q15, q9\n"

          "vst1.32 {q4}, [%[packed_ptr]]!\n"
          "vst1.32 {q5}, [%[packed_ptr]]!\n"

          "4:\n"
          // Pairwise add 32-bit accumulators
          "vpadd.i32 d24, d24, d25\n"
          "vpadd.i32 d26, d26, d27\n"
          "vpadd.i32 d28, d28, d29\n"
          "vpadd.i32 d30, d30, d31\n"
          // Final 32-bit values per row
          "vpadd.i32 d25, d24, d26\n"
          "vpadd.i32 d27, d28, d30\n"

          "ldr r3, [%[params], #" RUY_STR(RUY_OFFSET_SUMS_PTR) "]\n"
          "cmp r3, #0\n"
          "beq 6f\n"
          "vst1.32 {d25}, [r3]!\n"
          "vst1.32 {d27}, [r3]!\n"
          "6:\n"
      // clang-format on

      : [ src_ptr0 ] "+r"(src_ptr0), [ src_ptr1 ] "+r"(src_ptr1),
        [ src_ptr2 ] "+r"(src_ptr2), [ src_ptr3 ] "+r"(src_ptr3)
      : [ src_inc0 ] "r"(src_inc0), [ src_inc1 ] "r"(src_inc1),
        [ src_inc2 ] "r"(src_inc2), [ src_inc3 ] "r"(src_inc3),
        [ packed_ptr ] "r"(packed_ptr), [ params ] "r"(&params)
      : "cc", "memory", "r1", "r2", "r3", "q0", "q1", "q2", "q3",
        "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11", "q12", "q13");
}

// Packing code for out-of-order ARMv7 CPUs like the Krait 400 or A9.
// No attempt made at making this code efficient on in-order cores yet.
// This version differs from the above in that we only handle two columns
// at a time.
void Pack8bitColMajorForNeon2Cols(const PackParams8bit& params) {
  CheckOffsetsInPackParams8bit(params);
  profiler::ScopeLabel label("Pack (kNeon)");
  const void* src_ptr0 = params.src_ptr0;
  const void* src_ptr1 = params.src_ptr1;
  const int src_inc0 = params.src_inc0;
  const int src_inc1 = params.src_inc1;
  const std::int8_t* packed_ptr = params.packed_ptr;

  asm volatile(
      // clang-format off

          "ldr r2, [%[params], #" RUY_STR(RUY_OFFSET_INPUT_XOR) "]\n"
          "vdup.8 q11, r2\n"
          "mov r1, #0\n"
          // Zero-out the accumulators
          "vmov.i32 q12, #0\n"
          "vmov.i32 q13, #0\n"

          // Round down src_rows to nearest multiple of 16.
          "ldr r3, [%[params], #" RUY_STR(RUY_OFFSET_SRC_ROWS) "]\n"
          "and r2, r3, #-16\n"
          "cmp r1, r2\n"
          "beq 3f\n"

          "1:\n"
          "add r1, r1, #16\n"
          /* Load q0 */
          "vld1.8 {d0, d1}, [%[src_ptr0]]\n"
          "add %[src_ptr0], %[src_ptr0], %[src_inc0]\n"

          /* Load q1 */
          "vld1.8 {d2, d3}, [%[src_ptr1]]\n"
          "add %[src_ptr1], %[src_ptr1], %[src_inc1]\n"

          "veor.8 q4, q0, q11\n"
          "veor.8 q5, q1, q11\n"

          // Pairwise add in to 16b accumulators.
          "vpaddl.s8 q8, q4\n"
          "vpaddl.s8 q9, q5\n"

          "vst1.32 {q4}, [%[packed_ptr]]!\n"
          "vst1.32 {q5}, [%[packed_ptr]]!\n"

          // Pairwise add accumulate into 32b accumulators.
          // q12 and q13 contain 4x32b accumulators
          "vpadal.s16 q12, q8\n"
          "vpadal.s16 q13, q9\n"

          "cmp r1, r2\n"

          "bne 1b\n"

          "3:\n"

          // Now pack the last (num_rows % 16) rows.
          "ldr r3, [%[params], #" RUY_STR(RUY_OFFSET_SRC_ROWS) "]\n"
          "ands r2, r3, #15\n"
          "beq 4f\n"
          "ldr r3, [%[params], #" RUY_STR(RUY_OFFSET_SRC_ZERO_POINT) "]\n"
          "vdup.8 q0, r3\n"
          "vdup.8 q1, r3\n"

// Read/accumulate/write for src_ptr0 and src_ptr1.
#define RUY_LOAD_ONE_ROW1(I, R)            \
  "cmp r2, #" #I "\n"                      \
  "beq 5f\n"                               \
  "vld1.8 { d0[" #R "]}, [%[src_ptr0]]!\n" \
  "vld1.8 { d2[" #R "]}, [%[src_ptr1]]!\n" \

          RUY_LOAD_ONE_ROW1(0, 0)
          RUY_LOAD_ONE_ROW1(1, 1)
          RUY_LOAD_ONE_ROW1(2, 2)
          RUY_LOAD_ONE_ROW1(3, 3)
          RUY_LOAD_ONE_ROW1(4, 4)
          RUY_LOAD_ONE_ROW1(5, 5)
          RUY_LOAD_ONE_ROW1(6, 6)
          RUY_LOAD_ONE_ROW1(7, 7)
#undef RUY_LOAD_ONE_ROW1

#define RUY_LOAD_ONE_ROW2(I, R)            \
  "cmp r2, #" #I "\n"                      \
  "beq 5f\n"                               \
  "vld1.8 { d1[" #R "]}, [%[src_ptr0]]!\n" \
  "vld1.8 { d3[" #R "]}, [%[src_ptr1]]!\n" \

          RUY_LOAD_ONE_ROW2(8, 0)
          RUY_LOAD_ONE_ROW2(9, 1)
          RUY_LOAD_ONE_ROW2(10, 2)
          RUY_LOAD_ONE_ROW2(11, 3)
          RUY_LOAD_ONE_ROW2(12, 4)
          RUY_LOAD_ONE_ROW2(13, 5)
          RUY_LOAD_ONE_ROW2(14, 6)
          RUY_LOAD_ONE_ROW2(15, 7)
#undef RUY_LOAD_ONE_ROW2

          "5:\n"

          "veor.16 q4, q0, q11\n"
          "veor.16 q5, q1, q11\n"

          "vpaddl.s8 q8, q4\n"
          "vpaddl.s8 q9, q5\n"


          // Pairwise add accumulate to 4x32b accumulators.
          "vpadal.s16 q12, q8\n"
          "vpadal.s16 q13, q9\n"

          "vst1.32 {q4}, [%[packed_ptr]]!\n"
          "vst1.32 {q5}, [%[packed_ptr]]!\n"

          "4:\n"

          // Pairwise add 32-bit accumulators
          "vpadd.i32 d24, d24, d25\n"
          "vpadd.i32 d26, d26, d27\n"
          // Final 32-bit values per row
          "vpadd.i32 d25, d24, d26\n"

          "ldr r3, [%[params], #" RUY_STR(RUY_OFFSET_SUMS_PTR) "]\n"
          "cmp r3, #0\n"
          "beq 6f\n"
          "vst1.32 {d25}, [r3]!\n"
          "6:\n"
      // clang-format on

      : [ src_ptr0 ] "+r"(src_ptr0), [ src_ptr1 ] "+r"(src_ptr1)
      : [ src_inc0 ] "r"(src_inc0), [ src_inc1 ] "r"(src_inc1),
        [ packed_ptr ] "r"(packed_ptr), [ params ] "r"(&params)
      : "cc", "memory", "r1", "r2", "r3", "q0", "q1", "q2", "q3",
        "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11", "q12", "q13");
}

#undef RUY_OFFSET_SRC_PTR0
#undef RUY_OFFSET_SRC_PTR1
#undef RUY_OFFSET_SRC_PTR2
#undef RUY_OFFSET_SRC_PTR32
#undef RUY_OFFSET_SUMS_PTR
#undef RUY_OFFSET_PACKED_PTR0
#undef RUY_OFFSET_SRC_INC0
#undef RUY_OFFSET_SRC_INC1
#undef RUY_OFFSET_SRC_INC2
#undef RUY_OFFSET_SRC_INC3
#undef RUY_OFFSET_SRC_ROWS
#undef RUY_OFFSET_SRC_ZERO_POINT
#undef RUY_OFFSET_INPUT_XOR

#endif  //  RUY_PLATFORM_NEON_32 && RUY_OPT(ASM)

#if RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)

void Pack8bitColMajorForNeonA55ish(const void* src_ptr0, const void* src_ptr1,
                                   const void* src_ptr2, const void* src_ptr3,
                                   int src_inc0, int src_inc1, int src_inc2,
                                   int src_inc3, int src_rows,
                                   int src_zero_point, std::int8_t* packed_ptr,
                                   std::int32_t* sums_ptr, int input_xor) {
  profiler::ScopeLabel label("Pack (kNeon, optimized for in-order cores)");
  asm volatile(
      // clang-format off
          // v26 will be the vector to XOR input values with to perform
          // any input data type conversion (e.g. uint8 to int8).
          "dup v26.16b, %w[input_xor]\n"
          // w1 will be the number of rows already loaded.
          "mov w1, #0\n"
          // v28--v32 will be used to accumulate the sums
          "movi v28.4s, #0\n"
          "movi v29.4s, #0\n"
          "movi v30.4s, #0\n"
          "movi v31.4s, #0\n"
          // Let w2 be `rows` rounded down to multiple of 16.
          "ands w2, %w[rows], #-16\n"
          // If there are no full blocks of 16 rows to process, jump to the
          // code handling the last < 16 rows.
          "beq 3f\n"
          // Load the first block of 16 rows.
          "add w1, w1, #16\n"
          "ldr x10, [%[src_ptr0], #8]\n"
          "ld1 {v0.8b}, [%[src_ptr0]], %[src_inc0]\n"
          "ldr x11, [%[src_ptr1], #8]\n"
          "ld1 {v1.8b}, [%[src_ptr1]], %[src_inc1]\n"
          "ldr x12, [%[src_ptr2], #8]\n"
          "ld1 {v2.8b}, [%[src_ptr2]], %[src_inc2]\n"
          "ldr x13, [%[src_ptr3], #8]\n"
          "ld1 {v3.8b}, [%[src_ptr3]], %[src_inc3]\n"
          // Check if these were the only full block of 16 rows to load.
          "cmp w1, w2\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #192]\n")
          // In that case, jump to the code handling the last loaded block of
          // 16 rows.
          "beq 2f\n"
          // Main loop processing blocks of 16 rows.
          "1:\n"
          // Load the next 16 rows, interleaved with the XOR input type
          // conversion (e.g. uint8->int8) on the already loaded inputs.
          "add w1, w1, #16\n"
          "ins v0.d[1], x10\n"
          "ldr x10, [%[src_ptr0], #8]\n"
          "ins v1.d[1], x11\n"
          "ldr x11, [%[src_ptr1], #8]\n"
          "ins v2.d[1], x12\n"
          "ldr x12, [%[src_ptr2], #8]\n"
          "ins v3.d[1], x13\n"
          "ldr x13, [%[src_ptr3], #8]\n"
          "eor v4.16b, v0.16b, v26.16b\n"
          "ld1 {v0.8b}, [%[src_ptr0]], %[src_inc0]\n"
          "eor v5.16b, v1.16b, v26.16b\n"
          "ld1 {v1.8b}, [%[src_ptr1]], %[src_inc1]\n"
          "eor v6.16b, v2.16b, v26.16b\n"
          "ld1 {v2.8b}, [%[src_ptr2]], %[src_inc2]\n"
          "eor v7.16b, v3.16b, v26.16b\n"
          "ld1 {v3.8b}, [%[src_ptr3]], %[src_inc3]\n"
          // Compute the sums, interleaved with storing to the packed matrix.
          "saddlp v16.8h, v4.16b\n"
          "str q4, [%[packed_ptr], #0]\n"
          "saddlp v17.8h, v5.16b\n"
          "str q5, [%[packed_ptr], #16]\n"
          "saddlp v18.8h, v6.16b\n"
          "str q6, [%[packed_ptr], #32]\n"
          "saddlp v19.8h, v7.16b\n"
          "str q7, [%[packed_ptr], #48]\n"
          "sadalp v28.4s, v16.8h\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #240]\n")
          // Was this the last block of 16 rows to load?
          "cmp w1, w2\n"
          "sadalp v29.4s, v17.8h\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #240]\n")
          "add %[packed_ptr], %[packed_ptr], #64\n"
          "sadalp v30.4s, v18.8h\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #240]\n")
          "sadalp v31.4s, v19.8h\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #240]\n")
          // End of main loop on blocks of 16 rows.
          "bne 1b\n"

          // Code handling the last already-loaded block of 16 rows.
          "2:\n"
          // Process the last loaded full 16x4 block.
          "ins v0.d[1], x10\n"
          "ins v1.d[1], x11\n"
          "ins v2.d[1], x12\n"
          "ins v3.d[1], x13\n"
          "eor v4.16b, v0.16b, v26.16b\n"
          "eor v5.16b, v1.16b, v26.16b\n"
          "eor v6.16b, v2.16b, v26.16b\n"
          "eor v7.16b, v3.16b, v26.16b\n"

          "saddlp v16.8h, v4.16b\n"
          "str q4, [%[packed_ptr], #0]\n"
          "saddlp v17.8h, v5.16b\n"
          "str q5, [%[packed_ptr], #16]\n"
          "saddlp v18.8h, v6.16b\n"
          "str q6, [%[packed_ptr], #32]\n"
          "saddlp v19.8h, v7.16b\n"
          "str q7, [%[packed_ptr], #48]\n"
          "sadalp v28.4s, v16.8h\n"
          "sadalp v29.4s, v17.8h\n"
          "sadalp v30.4s, v18.8h\n"
          "sadalp v31.4s, v19.8h\n"

          "add %[packed_ptr], %[packed_ptr], #64\n"

          // End of code handling full blocks of 16 rows.
          // Now we handle any remaining rows.
          "3:\n"
          // Let w2 be the number of rows left to handle.
          "ands w2, %w[rows], #15\n"
          // If w2==0, there are no remaining rows, jump to the end.
          "beq 4f\n"
          // Zero out a 16x4 block in registers, which we'll partially overwrite
          // with any remaining rows.
          "dup v0.16b, %w[src_zero_point]\n"
          "dup v1.16b, %w[src_zero_point]\n"
          "dup v2.16b, %w[src_zero_point]\n"
          "dup v3.16b, %w[src_zero_point]\n"
#define RUY_LOAD_ONE_ROW(R)                   \
  "cmp w2, #" #R "\n"                         \
  "beq 5f\n"                                  \
  "ld1 { v0.b }[" #R "], [%[src_ptr0]], #1\n" \
  "ld1 { v1.b }[" #R "], [%[src_ptr1]], #1\n" \
  "ld1 { v2.b }[" #R "], [%[src_ptr2]], #1\n" \
  "ld1 { v3.b }[" #R "], [%[src_ptr3]], #1\n"

          RUY_LOAD_ONE_ROW(0)
          RUY_LOAD_ONE_ROW(1)
          RUY_LOAD_ONE_ROW(2)
          RUY_LOAD_ONE_ROW(3)
          RUY_LOAD_ONE_ROW(4)
          RUY_LOAD_ONE_ROW(5)
          RUY_LOAD_ONE_ROW(6)
          RUY_LOAD_ONE_ROW(7)
          RUY_LOAD_ONE_ROW(8)
          RUY_LOAD_ONE_ROW(9)
          RUY_LOAD_ONE_ROW(10)
          RUY_LOAD_ONE_ROW(11)
          RUY_LOAD_ONE_ROW(12)
          RUY_LOAD_ONE_ROW(13)
          RUY_LOAD_ONE_ROW(14)
          // Here we know that w2==15, so RUY_LOAD_ONE_ROW(15) would be a no-op.
#undef RUY_LOAD_ONE_ROW
          "5:\n"

          // Process the last zero-padded 16x4 block.
          "eor v4.16b, v0.16b, v26.16b\n"
          "eor v5.16b, v1.16b, v26.16b\n"
          "eor v6.16b, v2.16b, v26.16b\n"
          "eor v7.16b, v3.16b, v26.16b\n"

          "saddlp v16.8h, v4.16b\n"
          "saddlp v17.8h, v5.16b\n"
          "saddlp v18.8h, v6.16b\n"
          "saddlp v19.8h, v7.16b\n"
          "sadalp v28.4s, v16.8h\n"
          "sadalp v29.4s, v17.8h\n"
          "sadalp v30.4s, v18.8h\n"
          "sadalp v31.4s, v19.8h\n"

          "str q4, [%[packed_ptr], #0]\n"
          "str q5, [%[packed_ptr], #16]\n"
          "str q6, [%[packed_ptr], #32]\n"
          "str q7, [%[packed_ptr], #48]\n"
          "add %[packed_ptr], %[packed_ptr], #64\n"

          "4:\n"

          // Horizontal reduction of the registers used to accumulate sums.
          "addp v28.4s, v28.4s, v29.4s\n"
          "addp v30.4s, v30.4s, v31.4s\n"
          "addp v28.4s, v28.4s, v30.4s\n"

          // Store the sums.
          "cmp %[sums_ptr], #0\n"
          "beq 6f\n"
          "st1 {v28.4s}, [%[sums_ptr]], #16\n"
          "6:\n"
          // clang-format on

          : [ src_ptr0 ] "+r"(src_ptr0), [ src_ptr1 ] "+r"(src_ptr1),
            [ src_ptr2 ] "+r"(src_ptr2), [ src_ptr3 ] "+r"(src_ptr3),
            [ packed_ptr ] "+r"(packed_ptr), [ sums_ptr ] "+r"(sums_ptr)
          : [ src_inc0 ] "r"(static_cast<std::int64_t>(src_inc0)), [ src_inc1 ] "r"(static_cast<std::int64_t>(src_inc1)),
            [ src_inc2 ] "r"(static_cast<std::int64_t>(src_inc2)), [ src_inc3 ] "r"(static_cast<std::int64_t>(src_inc3)),
            [ rows ] "r"(src_rows),
            [ src_zero_point ] "r"(src_zero_point),
            [input_xor] "r"(input_xor)
          : "cc", "memory", "x1", "x2", "x10", "x11", "x12", "x13", "v0", "v1", "v2", "v3", "v4", "v5",
            "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",
            "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24",
            "v25", "v26", "v27", "v28", "v29", "v30", "v31");
}

void Pack8bitColMajorForNeonDotprodA55ish(
    const void* src_ptr0, const void* src_ptr1, const void* src_ptr2,
    const void* src_ptr3, int src_inc0, int src_inc1, int src_inc2,
    int src_inc3, int src_rows, int src_zero_point, std::int8_t* packed_ptr,
    std::int32_t* sums_ptr, int input_xor) {
  profiler::ScopeLabel label(
      "Pack (kNeonDotprod, optimized for in-order cores)");
  asm volatile(
          // clang-format off
          // v26 will be the vector to XOR input values with to perform
          // any input data type conversion (e.g. uint8 to int8).
          "dup v26.16b, %w[input_xor]\n"
          // v27 will be filled with 1's. It will be used as an operand
          // to SDOT to compute the sums.
          "mov w1, #1\n"
          "dup v27.16b, w1\n"
          // w1 will be the number of rows already loaded.
          "mov w1, #0\n"
          // v28--v32 will be used to accumulate the sums
          "movi v28.4s, #0\n"
          "movi v29.4s, #0\n"
          "movi v30.4s, #0\n"
          "movi v31.4s, #0\n"

          // Let w2 be `rows` rounded down to multiple of 16.
          "ands w2, %w[rows], #-16\n"
          // If there are no full blocks of 16 rows to process, jump to the
          // code handling the last < 16 rows.
          "beq 3f\n"
          // Load the first block of 16 rows.
          "add w1, w1, #16\n"
          "ldr x10, [%[src_ptr0], #8]\n"
          "ld1 {v0.8b}, [%[src_ptr0]], %[src_inc0]\n"
          "ldr x11, [%[src_ptr1], #8]\n"
          "ld1 {v1.8b}, [%[src_ptr1]], %[src_inc1]\n"
          "ldr x12, [%[src_ptr2], #8]\n"
          "ld1 {v2.8b}, [%[src_ptr2]], %[src_inc2]\n"
          "ldr x13, [%[src_ptr3], #8]\n"
          "ld1 {v3.8b}, [%[src_ptr3]], %[src_inc3]\n"
          // Check if these were the only full block of 16 rows to load.
          "cmp w1, w2\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #192]\n")
          // In that case, jump to the code handling the last loaded block of
          // 16 rows.
          "beq 2f\n"

          // Main loop processing blocks of 16 rows.
          "1:\n"
          "add w1, w1, #16\n"
          // Prepare the already-loaded 16 rows by inserting the parts
          // loaded into general purpose registers x10--x13 into the
          // NEON registers v0--v3 where the other parts had already been
          // loaded.
          "ins v0.d[1], x10\n"
          "ldr x10, [%[src_ptr0], #8]\n"
          "ins v1.d[1], x11\n"
          "ldr x11, [%[src_ptr1], #8]\n"
          "ins v2.d[1], x12\n"
          "ldr x12, [%[src_ptr2], #8]\n"
          "ins v3.d[1], x13\n"
          "ldr x13, [%[src_ptr3], #8]\n"

          // Load the next 16 rows and, interleaved with that,
          // perform the input type conversion (e.g. uint8->int8) on the
          // current 16 rows.
          "eor v4.16b, v0.16b, v26.16b\n"
          "ld1 {v0.8b}, [%[src_ptr0]], %[src_inc0]\n"
          "eor v5.16b, v1.16b, v26.16b\n"
          "ld1 {v1.8b}, [%[src_ptr1]], %[src_inc1]\n"
          "eor v6.16b, v2.16b, v26.16b\n"
          "ld1 {v2.8b}, [%[src_ptr2]], %[src_inc2]\n"
          "eor v7.16b, v3.16b, v26.16b\n"
          "ld1 {v3.8b}, [%[src_ptr3]], %[src_inc3]\n"

          // Transposition of 4x4 blocks, part 1
          "trn1 v16.4s, v4.4s, v5.4s\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #240]\n")
          "trn2 v17.4s, v4.4s, v5.4s\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #240]\n")
          "trn1 v18.4s, v6.4s, v7.4s\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #240]\n")
          "trn2 v19.4s, v6.4s, v7.4s\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #240]\n")

          // Transposition of 4x4 blocks, part 2
          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"
          "cmp w1, w2\n"

          // Store the block to the packed matrix and, interleaved with
          // that, compute sums using sdot instructions.
          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          "str q20, [%[packed_ptr], #0]\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          "str q21, [%[packed_ptr], #32]\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          "str q22, [%[packed_ptr], #64]\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"
          // End of main loop on blocks of 16 rows.
          "bne 1b\n"

          // Code handling the last already-loaded block of 16 rows.
          "2:\n"
          // Process the last loaded full 16x4 block.
          "ins v0.d[1], x10\n"
          "ins v1.d[1], x11\n"
          "ins v2.d[1], x12\n"
          "ins v3.d[1], x13\n"
          "eor v0.16b, v0.16b, v26.16b\n"
          "eor v1.16b, v1.16b, v26.16b\n"
          "eor v2.16b, v2.16b, v26.16b\n"
          "eor v3.16b, v3.16b, v26.16b\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          "str q20, [%[packed_ptr], #0]\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          "str q21, [%[packed_ptr], #32]\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          "str q22, [%[packed_ptr], #64]\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // End of code handling full blocks of 16 rows.
          // Now we handle any remaining rows.
          "3:\n"
          // Let w2 be the number of rows left to handle.
          "ands w2, %w[rows], #15\n"
          // If w2==0, there are no remaining rows, jump to the end.
          "beq 4f\n"
          // Zero out a 16x4 block in registers, which we'll partially overwrite
          // with any remaining rows.
          "dup v0.16b, %w[src_zero_point]\n"
          "dup v1.16b, %w[src_zero_point]\n"
          "dup v2.16b, %w[src_zero_point]\n"
          "dup v3.16b, %w[src_zero_point]\n"
#define RUY_LOAD_ONE_ROW(R)                   \
  "cmp w2, #" #R "\n"                         \
  "beq 5f\n"                                  \
  "ld1 { v0.b }[" #R "], [%[src_ptr0]], #1\n" \
  "ld1 { v1.b }[" #R "], [%[src_ptr1]], #1\n" \
  "ld1 { v2.b }[" #R "], [%[src_ptr2]], #1\n" \
  "ld1 { v3.b }[" #R "], [%[src_ptr3]], #1\n"

          RUY_LOAD_ONE_ROW(0)
          RUY_LOAD_ONE_ROW(1)
          RUY_LOAD_ONE_ROW(2)
          RUY_LOAD_ONE_ROW(3)
          RUY_LOAD_ONE_ROW(4)
          RUY_LOAD_ONE_ROW(5)
          RUY_LOAD_ONE_ROW(6)
          RUY_LOAD_ONE_ROW(7)
          RUY_LOAD_ONE_ROW(8)
          RUY_LOAD_ONE_ROW(9)
          RUY_LOAD_ONE_ROW(10)
          RUY_LOAD_ONE_ROW(11)
          RUY_LOAD_ONE_ROW(12)
          RUY_LOAD_ONE_ROW(13)
          RUY_LOAD_ONE_ROW(14)
          // Here we know that w2==15, so RUY_LOAD_ONE_ROW(15) would be a no-op.
#undef RUY_LOAD_ONE_ROW

          "5:\n"
          // Process the last zero-padded 16x4 block.
          "eor v0.16b, v0.16b, v26.16b\n"
          "eor v1.16b, v1.16b, v26.16b\n"
          "eor v2.16b, v2.16b, v26.16b\n"
          "eor v3.16b, v3.16b, v26.16b\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          "str q20, [%[packed_ptr], #0]\n"
          "cmp w2, #4\n"
          "ble 4f\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          "str q21, [%[packed_ptr], #32]\n"
          "cmp w2, #8\n"
          "ble 4f\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          "str q22, [%[packed_ptr], #64]\n"
          "cmp w2, #12\n"
          "ble 4f\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          "4:\n"

          // Reduction of the registers used to accumulate sums.
          "add v28.4s, v28.4s, v29.4s\n"
          "add v30.4s, v30.4s, v31.4s\n"
          "add v28.4s, v28.4s, v30.4s\n"

          // Store the sums.
          "cmp %[sums_ptr], #0\n"
          "beq 6f\n"
          "st1 {v28.4s}, [%[sums_ptr]], #16\n"
          "6:\n"
          // clang-format on

          : [ src_ptr0 ] "+r"(src_ptr0), [src_ptr1] "+r"(src_ptr1), [src_ptr2] "+r"(src_ptr2),
            [src_ptr3] "+r"(src_ptr3), [packed_ptr] "+r"(packed_ptr), [sums_ptr] "+r"(sums_ptr)
          : [ src_inc0 ] "r"(static_cast<std::int64_t>(src_inc0)), [ src_inc1 ] "r"(static_cast<std::int64_t>(src_inc1)),
            [ src_inc2 ] "r"(static_cast<std::int64_t>(src_inc2)), [ src_inc3 ] "r"(static_cast<std::int64_t>(src_inc3)),
                [rows] "r"(src_rows),
            [src_zero_point] "r"(static_cast<int>(src_zero_point)),
            [input_xor] "r"(input_xor)
          : "cc", "memory", "x1", "x2", "x10", "x11", "x12", "x13", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
            "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21",
            "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31");
}

void Pack8bitColMajorForNeonDotprod(const void* src_ptr0, const void* src_ptr1,
                                    const void* src_ptr2, const void* src_ptr3,
                                    int src_inc0, int src_inc1, int src_inc2,
                                    int src_inc3, int src_rows,
                                    int src_zero_point, std::int8_t* packed_ptr,
                                    std::int32_t* sums_ptr, int input_xor) {
  profiler::ScopeLabel label("Pack (kNeonDotprod)");
  asm volatile(
      // clang-format off
          // v26 will be the vector to XOR input values with to perform
          // any input data type conversion (e.g. uint8 to int8).
          "dup v26.16b, %w[input_xor]\n"
          // v27 will be filled with 1's. It will be used as an operand
          // to SDOT to compute the sums.
          "mov w1, #1\n"
          "dup v27.16b, w1\n"
          // w1 will be the number of rows already loaded.
          "mov w1, #0\n"
          // v28--v32 will be used to accumulate the sums
          "movi v28.4s, #0\n"
          "movi v29.4s, #0\n"
          "movi v30.4s, #0\n"
          "movi v31.4s, #0\n"

          // 4x partially unrolled code processing blocks of 64 rows.
          // Read the original loop below first, it has more comments.
#if RUY_OPT(MAX_STREAMING)
          // Let w2 be `rows` rounded down to multiple of 64.
          // Each iteration of this 4x partially unrolled loop handles
          // 64 rows.
          "ands w2, %w[rows], #-64\n"
          // If there are no full blocks of 64 rows to process, jump to
          // the main loop below handling 16 rows per iteration.
          "beq 9f\n"
          // Load the first block of 64 rows.
          "add w1, w1, #64\n"
          "ld1 {v0.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v1.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v2.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v3.16b}, [%[src_ptr3]], %[src_inc3]\n"
          "ld1 {v4.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v5.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v6.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v7.16b}, [%[src_ptr3]], %[src_inc3]\n"
          "ld1 {v8.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v9.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v10.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v11.16b}, [%[src_ptr3]], %[src_inc3]\n"
          "ld1 {v12.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v13.16b}, [%[src_ptr1]], %[src_inc1]\n"
          // Was that the last full block of 64 rows to load?
          "cmp w1, w2\n"
          "ld1 {v14.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v15.16b}, [%[src_ptr3]], %[src_inc3]\n"
          // Then jump to the end of the 64-rows-at-a-time code.
          "beq 8f\n"

          // Start of the main 4x partially unrolled loop.
          "7:\n"
          // Process rows 0 -- 15 out of 64.
          "eor v0.16b, v0.16b, v26.16b\n"
          "eor v1.16b, v1.16b, v26.16b\n"
          "eor v2.16b, v2.16b, v26.16b\n"
          "eor v3.16b, v3.16b, v26.16b\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "ld1 {v0.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v1.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v2.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v3.16b}, [%[src_ptr3]], %[src_inc3]\n"
          "add w1, w1, #16\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // Process rows 16 -- 31 out of 64.
          "eor v4.16b, v4.16b, v26.16b\n"
          "eor v5.16b, v5.16b, v26.16b\n"
          "eor v6.16b, v6.16b, v26.16b\n"
          "eor v7.16b, v7.16b, v26.16b\n"

          "trn1 v16.4s, v4.4s, v5.4s\n"
          "trn2 v17.4s, v4.4s, v5.4s\n"
          "trn1 v18.4s, v6.4s, v7.4s\n"
          "trn2 v19.4s, v6.4s, v7.4s\n"

          "ld1 {v4.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v5.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v6.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v7.16b}, [%[src_ptr3]], %[src_inc3]\n"
          "add w1, w1, #16\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // Process rows 32 -- 47 out of 64.
          "eor v8.16b, v8.16b, v26.16b\n"
          "eor v9.16b, v9.16b, v26.16b\n"
          "eor v10.16b, v10.16b, v26.16b\n"
          "eor v11.16b, v11.16b, v26.16b\n"

          "trn1 v16.4s, v8.4s, v9.4s\n"
          "trn2 v17.4s, v8.4s, v9.4s\n"
          "trn1 v18.4s, v10.4s, v11.4s\n"
          "trn2 v19.4s, v10.4s, v11.4s\n"

          "ld1 {v8.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v9.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v10.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v11.16b}, [%[src_ptr3]], %[src_inc3]\n"
          "add w1, w1, #16\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // Process rows 48 -- 63 out of 64.
          "eor v12.16b, v12.16b, v26.16b\n"
          "eor v13.16b, v13.16b, v26.16b\n"
          "eor v14.16b, v14.16b, v26.16b\n"
          "eor v15.16b, v15.16b, v26.16b\n"

          "trn1 v16.4s, v12.4s, v13.4s\n"
          "trn2 v17.4s, v12.4s, v13.4s\n"
          "trn1 v18.4s, v14.4s, v15.4s\n"
          "trn2 v19.4s, v14.4s, v15.4s\n"

          "ld1 {v12.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v13.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v14.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v15.16b}, [%[src_ptr3]], %[src_inc3]\n"
          "add w1, w1, #16\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "cmp w1, w2\n"
          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // End of main 4x partially unrolled loop.
          "bne 7b\n"

          // Last part of the 4x partially unrolled code:
          // handle the last already-loaded 64 rows.
          "8:\n"

          // Process rows 0 -- 15 out of 64.
          "eor v0.16b, v0.16b, v26.16b\n"
          "eor v1.16b, v1.16b, v26.16b\n"
          "eor v2.16b, v2.16b, v26.16b\n"
          "eor v3.16b, v3.16b, v26.16b\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // Process rows 16 -- 31 out of 64.
          "eor v4.16b, v4.16b, v26.16b\n"
          "eor v5.16b, v5.16b, v26.16b\n"
          "eor v6.16b, v6.16b, v26.16b\n"
          "eor v7.16b, v7.16b, v26.16b\n"

          "trn1 v16.4s, v4.4s, v5.4s\n"
          "trn2 v17.4s, v4.4s, v5.4s\n"
          "trn1 v18.4s, v6.4s, v7.4s\n"
          "trn2 v19.4s, v6.4s, v7.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // Process rows 32 -- 47 out of 64.
          "eor v8.16b, v8.16b, v26.16b\n"
          "eor v9.16b, v9.16b, v26.16b\n"
          "eor v10.16b, v10.16b, v26.16b\n"
          "eor v11.16b, v11.16b, v26.16b\n"

          "trn1 v16.4s, v8.4s, v9.4s\n"
          "trn2 v17.4s, v8.4s, v9.4s\n"
          "trn1 v18.4s, v10.4s, v11.4s\n"
          "trn2 v19.4s, v10.4s, v11.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // Process rows 48 -- 63 out of 64.
          "eor v12.16b, v12.16b, v26.16b\n"
          "eor v13.16b, v13.16b, v26.16b\n"
          "eor v14.16b, v14.16b, v26.16b\n"
          "eor v15.16b, v15.16b, v26.16b\n"

          "trn1 v16.4s, v12.4s, v13.4s\n"
          "trn2 v17.4s, v12.4s, v13.4s\n"
          "trn1 v18.4s, v14.4s, v15.4s\n"
          "trn2 v19.4s, v14.4s, v15.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          "9:\n"
#endif  // #if RUY_OPT(MAX_STREAMING)
          // End of 4x partially unrolled code processing blocks of 64 rows.

          // Main part of the code, processing blocks of 16 rows.

          // Let w2 be `rows` rounded down to multiple of 16.
          "and w2, %w[rows], #-16\n"
          // If there are no full blocks of 16 rows to process, jump to the
          // code handling the last < 16 rows.
          "cmp w1, w2\n"
          "beq 3f\n"

          // Load the first block of 16 rows.
          "add w1, w1, #16\n"
          "ld1 {v0.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v1.16b}, [%[src_ptr1]], %[src_inc1]\n"
          // Check if these were the only full block of 16 rows to load.
          "cmp w1, w2\n"
          "ld1 {v2.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v3.16b}, [%[src_ptr3]], %[src_inc3]\n"
          // In that case, jump to the code handling the last loaded block of
          // 16 rows.
          "beq 2f\n"
          // Main loop processing blocks of 16 rows.
          "1:\n"
          // Input type conversion (e.g. uint8->int8).
          "eor v0.16b, v0.16b, v26.16b\n"
          "eor v1.16b, v1.16b, v26.16b\n"
          "eor v2.16b, v2.16b, v26.16b\n"
          "eor v3.16b, v3.16b, v26.16b\n"
          // Transposition of 4x4 blocks, part 1
          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"
          // Load the next 16 rows
          "ld1 {v0.16b}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v1.16b}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v2.16b}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v3.16b}, [%[src_ptr3]], %[src_inc3]\n"
          "add w1, w1, #16\n"
          // Transposition of 4x4 blocks, part 2
          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"
          // Compute sums using sdot instructions.
          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"
          // Store the block to the packed matrix.
          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "cmp w1, w2\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"
          // End of main loop on blocks of 16 rows.
          "bne 1b\n"

          // Code handling the last already-loaded block of 16 rows.
          "2:\n"

          // Process the last loaded full 16x4 block.
          "eor v0.16b, v0.16b, v26.16b\n"
          "eor v1.16b, v1.16b, v26.16b\n"
          "eor v2.16b, v2.16b, v26.16b\n"
          "eor v3.16b, v3.16b, v26.16b\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // End of code handling full blocks of 16 rows.
          // Now we handle any remaining rows.
          "3:\n"
          // Let w2 be the number of rows left to handle.
          "ands w2, %w[rows], #15\n"
          // If w2==0, there are no remaining rows, jump to the end.
          "beq 4f\n"
          // Zero out a 16x4 block in registers, which we'll partially overwrite
          // with any remaining rows.
          "dup v0.16b, %w[src_zero_point]\n"
          "dup v1.16b, %w[src_zero_point]\n"
          "dup v2.16b, %w[src_zero_point]\n"
          "dup v3.16b, %w[src_zero_point]\n"
#define RUY_LOAD_ONE_ROW(R)                   \
  "cmp w2, #" #R "\n"                         \
  "beq 5f\n"                                  \
  "ld1 { v0.b }[" #R "], [%[src_ptr0]], #1\n" \
  "ld1 { v1.b }[" #R "], [%[src_ptr1]], #1\n" \
  "ld1 { v2.b }[" #R "], [%[src_ptr2]], #1\n" \
  "ld1 { v3.b }[" #R "], [%[src_ptr3]], #1\n"

          RUY_LOAD_ONE_ROW(0)
          RUY_LOAD_ONE_ROW(1)
          RUY_LOAD_ONE_ROW(2)
          RUY_LOAD_ONE_ROW(3)
          RUY_LOAD_ONE_ROW(4)
          RUY_LOAD_ONE_ROW(5)
          RUY_LOAD_ONE_ROW(6)
          RUY_LOAD_ONE_ROW(7)
          RUY_LOAD_ONE_ROW(8)
          RUY_LOAD_ONE_ROW(9)
          RUY_LOAD_ONE_ROW(10)
          RUY_LOAD_ONE_ROW(11)
          RUY_LOAD_ONE_ROW(12)
          RUY_LOAD_ONE_ROW(13)
          RUY_LOAD_ONE_ROW(14)
          // Here we know that w2==15, so RUY_LOAD_ONE_ROW(15) would be a no-op.
#undef RUY_LOAD_ONE_ROW

          "5:\n"
          // Process the last zero-padded 16x4 block.
          "eor v0.16b, v0.16b, v26.16b\n"
          "eor v1.16b, v1.16b, v26.16b\n"
          "eor v2.16b, v2.16b, v26.16b\n"
          "eor v3.16b, v3.16b, v26.16b\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          ".word 0x4e9b969c  // sdot v28.4s, v20.16b, v27.16b\n"
          "str q20, [%[packed_ptr], #0]\n"
          "cmp w2, #4\n"
          "ble 4f\n"
          ".word 0x4e9b96be  // sdot v30.4s, v21.16b, v27.16b\n"
          "str q21, [%[packed_ptr], #32]\n"
          "cmp w2, #8\n"
          "ble 4f\n"
          ".word 0x4e9b96dd  // sdot v29.4s, v22.16b, v27.16b\n"
          "str q22, [%[packed_ptr], #64]\n"
          "cmp w2, #12\n"
          "ble 4f\n"
          ".word 0x4e9b96ff  // sdot v31.4s, v23.16b, v27.16b\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          "4:\n"

          // Reduction of the registers used to accumulate sums.
          "add v28.4s, v28.4s, v29.4s\n"
          "add v30.4s, v30.4s, v31.4s\n"
          "add v28.4s, v28.4s, v30.4s\n"

          // Store the sums.
          "cmp %[sums_ptr], #0\n"
          "beq 6f\n"
          "st1 {v28.4s}, [%[sums_ptr]], #16\n"
          "6:\n"
      // clang-format on

      : [src_ptr0] "+r"(src_ptr0), [src_ptr1] "+r"(src_ptr1),
        [src_ptr2] "+r"(src_ptr2), [src_ptr3] "+r"(src_ptr3),
        [packed_ptr] "+r"(packed_ptr), [sums_ptr] "+r"(sums_ptr)
      : [src_inc0] "r"(static_cast<std::int64_t>(src_inc0)),
        [src_inc1] "r"(static_cast<std::int64_t>(src_inc1)),
        [src_inc2] "r"(static_cast<std::int64_t>(src_inc2)),
        [src_inc3] "r"(static_cast<std::int64_t>(src_inc3)),
        [rows] "r"(src_rows),
        [src_zero_point] "r"(static_cast<int>(src_zero_point)),
        [input_xor] "r"(input_xor)
      : "cc", "memory", "x1", "x2", "v0", "v1", "v2", "v3", "v4", "v5", "v6",
        "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16",
        "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26",
        "v27", "v28", "v29", "v30", "v31");
}

void Pack8bitRowMajorForNeonDotprod(const void* src_ptr0, const void* src_ptr1,
                                    const void* src_ptr2, const void* src_ptr3,
                                    int src_inc0, int src_inc1, int src_inc2,
                                    int src_inc3, int src_cols,
                                    int src_zero_point, std::int8_t* packed_ptr,
                                    int packed_stride, std::int32_t* sums_ptr,
                                    int input_xor) {
  profiler::ScopeLabel label("Pack (kNeonDotprod, from row-major)");
  asm(
      // clang-format off
          // Prefetch data. This was tuned on Cortex-A55-rev1 cores.
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0]]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1]]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2]]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3]]\n")
          // Let w0 = (number of columns to compute) - 8.
          "subs w0, %w[src_cols], 8\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], 64]\n")
          // Let v26 duplicate the input_xor value in all lanes.
          "dup v26.16b, %w[input_xor]\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], 64]\n")
          // Let v27 be 1 in all lanes. Used with sdot to compute sums.
          "movi v27.16b, 1\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], 64]\n")
          // If there isn't a full block of 8 columns to load from, jump to the
          // code after the loop handling leftovers.
          "blt 2f\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], 64]\n")
          // Main loop, each iteration handles a full block of 8 cols.
          "1:\n"
          // Load the 4x8 block from the source matrix, or zero if we're
          // past the bottom of the source matrix.
          "ld1 {v0.8b}, [%[src_ptr0]]\n"
          "ld1 {v1.8b}, [%[src_ptr1]]\n"
          "ld1 {v2.8b}, [%[src_ptr2]]\n"
          "ld1 {v3.8b}, [%[src_ptr3]]\n"
          // Load values from the sums buffer, and start the reordering
          // of the loaded 4x8 block by interleaving 8bit values.
          "zip1 v0.16b, v0.16b, v1.16b\n"
          "ldr q8, [%[sums_ptr], 0]\n"
          "zip1 v1.16b, v2.16b, v3.16b\n"
          "ldr q9, [%[sums_ptr], 16]\n"
          // Finish the reordering of the 4x8 block, putting it into
          // column-major order.
          "zip1 v2.8h, v0.8h, v1.8h\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], 128]\n")
          "zip2 v3.8h, v0.8h, v1.8h\n"
          // Apply input_xor, i.e. convert source values from uint8 to int8
          // if needed.
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], 128]\n")
          "eor v2.16b, v2.16b, v26.16b\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], 128]\n")
          "eor v3.16b, v3.16b, v26.16b\n"
          // Update the sums.
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], 128]\n")
          ".word 0x4e9b9448  // sdot v8.4s, v2.16b, v27.16b\n"
          ".word 0x4e9b9469  // sdot v9.4s, v3.16b, v27.16b\n"
          // Store the column-major 4x8 block to the packed matrix, and
          // increment some source pointers.
          "str q2, [%[packed_ptr], 0]\n"
          "add %[src_ptr0], %[src_ptr0], %w[src_inc0], sxtw\n"
          "str q3, [%[packed_ptr], 16]\n"
          "add %[src_ptr1], %[src_ptr1], %w[src_inc1], sxtw\n"
          // Store the updated sums, and increment the remaining pointers
          // and the block_col loop index.
          "st1 {v8.4s}, [%[sums_ptr]], 16\n"
          "add %[packed_ptr], %[packed_ptr], %[packed_stride], lsl 3\n"
          "st1 {v9.4s}, [%[sums_ptr]], 16\n"
          // Advance by 8 columns and set the condition code.
          "subs w0, w0, 8\n"
          "add %[src_ptr2], %[src_ptr2], %w[src_inc2], sxtw\n"
          "add %[src_ptr3], %[src_ptr3], %w[src_inc3], sxtw\n"
          // End of the main loop.
          "bge 1b\n"

          "2:\n"
          // We add back 8 to w0 so that w0 is the number of columns remaining
          // to handle.
          "adds w0, w0, 8\n"
          // Nothing left? Then jump to the end.
          "beq 3f\n"
          // Here w0 is between 1 and 7. We zero-initialize v0--v3 ...
          "dup v0.8b, %w[src_zero_point]\n"
          "dup v1.8b, %w[src_zero_point]\n"
          "dup v2.8b, %w[src_zero_point]\n"
          "dup v3.8b, %w[src_zero_point]\n"
          // ... and now we fill lanes one by one with leftover columns.
#define RUY_LOAD_ONE_COL(C)\
  "cmp w0, " #C "\n" \
  "beq 4f\n"                                  \
  "ld1 { v0.b }[" #C "], [%[src_ptr0]], #1\n" \
  "ld1 { v1.b }[" #C "], [%[src_ptr1]], #1\n" \
  "ld1 { v2.b }[" #C "], [%[src_ptr2]], #1\n" \
  "ld1 { v3.b }[" #C "], [%[src_ptr3]], #1\n"

          RUY_LOAD_ONE_COL(0)
          RUY_LOAD_ONE_COL(1)
          RUY_LOAD_ONE_COL(2)
          RUY_LOAD_ONE_COL(3)
          RUY_LOAD_ONE_COL(4)
          RUY_LOAD_ONE_COL(5)
          RUY_LOAD_ONE_COL(6)
          // Here we know that w0==7, so RUY_LOAD_ONE_COL(7) would be a no-op.
#undef RUY_LOAD_ONE_COL

          "4:\n"
          // The leftovers source data is loaded, now we can perform the
          // computation as usual.
          // Load values from the sums buffer, and start the reordering
          // of the loaded 4x8 block by interleaving 8bit values.
          "zip1 v0.16b, v0.16b, v1.16b\n"
          "ldr q8, [%[sums_ptr], 0]\n"
          "zip1 v1.16b, v2.16b, v3.16b\n"
          "ldr q9, [%[sums_ptr], 16]\n"
          // Finish the reordering of the 4x8 block, putting it into
          // column-major order.
          "zip1 v2.8h, v0.8h, v1.8h\n"
          "zip2 v3.8h, v0.8h, v1.8h\n"
          // Apply input_xor, i.e. convert source values from uint8 to int8
          // if needed.
          "eor v2.16b, v2.16b, v26.16b\n"
          "eor v3.16b, v3.16b, v26.16b\n"
          // Update the sums.
          ".word 0x4e9b9448  // sdot v8.4s, v2.16b, v27.16b\n"
          ".word 0x4e9b9469  // sdot v9.4s, v3.16b, v27.16b\n"
          // Store the column-major 4x8 block to the packed matrix, and
          // increment some source pointers.
          "str q2, [%[packed_ptr], 0]\n"
          "str q3, [%[packed_ptr], 16]\n"
          // Store the updated sums, and increment the remaining pointers
          // and the block_col loop index.
          "st1 {v8.4s}, [%[sums_ptr]], 16\n"
          "st1 {v9.4s}, [%[sums_ptr]], 16\n"

          // End label.
          "3:\n"
      // clang-format on
      : [packed_ptr] "+r"(packed_ptr), [sums_ptr] "+r"(sums_ptr),
        [src_ptr0] "+r"(src_ptr0), [src_ptr1] "+r"(src_ptr1),
        [src_ptr2] "+r"(src_ptr2), [src_ptr3] "+r"(src_ptr3)
      : [src_inc0] "r"(src_inc0), [src_inc1] "r"(src_inc1),
        [src_inc2] "r"(src_inc2), [src_inc3] "r"(src_inc3),
        [input_xor] "r"(input_xor), [src_zero_point] "r"(src_zero_point),
        [packed_stride] "r"(static_cast<std::int64_t>(packed_stride)),
        [src_cols] "r"(src_cols)
      : "cc", "memory", "x0", "x1", "x2", "v0", "v1", "v2", "v3", "v4", "v5",
        "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16",
        "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26",
        "v27", "v28", "v29", "v30", "v31");
}

void PackFloatColMajorForNeon(const float* src_ptr0, const float* src_ptr1,
                              const float* src_ptr2, const float* src_ptr3,
                              int src_inc0, int src_inc1, int src_inc2,
                              int src_inc3, int src_rows, float* packed_ptr) {
  profiler::ScopeLabel label("Pack (kNeon)");
  asm volatile(
      // clang-format off
          // w1 will be the number of rows already loaded.
          "mov w1, #0\n"
          // Let w2 be `rows` rounded down to multiple of 4.
          "ands w2, %w[rows], #-4\n"
          // If there are no full blocks of 4 rows to process, jump to the
          // code handling the last < 4 rows.
          "beq 3f\n"
          // Load the first block of 16 rows.
          "add w1, w1, #4\n"
          "ld1 {v0.4s}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v1.4s}, [%[src_ptr1]], %[src_inc1]\n"
          // Check if these were the only full block of 4 rows to load.
          "cmp w1, w2\n"
          "ld1 {v2.4s}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v3.4s}, [%[src_ptr3]], %[src_inc3]\n"
          // In that case, jump to the code handling the last loaded block of
          // 4 rows.
          "beq 2f\n"
          // Main loop processing blocks of 4 rows.
          "1:\n"
          // Advance by 4 rows.
          "add w1, w1, #4\n"
          // Transposition of the already-loaded 4x4 block, part 1.
          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"
          // Load the next 4x4 block.
          "ld1 {v0.4s}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v1.4s}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v2.4s}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v3.4s}, [%[src_ptr3]], %[src_inc3]\n"
          // Transposition of the already-loaded 4x4 block, part 2.
          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"
          // Was this the last full 4x4 block to load?
          "cmp w1, w2\n"
          // Store the transposed 4x4 block.
          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"
          // End of main loop on 4x4 blocks.
          "bne 1b\n"

          // Code handling the last already-loaded 4x4 block.
          "2:\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          // End of code handling full 4x4 blocks.
          // Now we handle any remaining rows.
          "3:\n"
          // Let w2 be the number of rows left to handle.
          "ands w2, %w[rows], #3\n"
          // If w2==0, there are no remaining rows, jump to the end.
          "beq 4f\n"
          // Zero out a 4x4 block in registers, which we'll partially overwrite
          // with any remaining rows.
          "movi v0.16b, #0\n"
          "movi v1.16b, #0\n"
          "movi v2.16b, #0\n"
          "movi v3.16b, #0\n"
#define RUY_LOAD_ONE_ROW(R)                   \
  "cmp w2, #" #R "\n"                         \
  "beq 5f\n"                                  \
  "ld1 { v0.s }[" #R "], [%[src_ptr0]], #4\n" \
  "ld1 { v1.s }[" #R "], [%[src_ptr1]], #4\n" \
  "ld1 { v2.s }[" #R "], [%[src_ptr2]], #4\n" \
  "ld1 { v3.s }[" #R "], [%[src_ptr3]], #4\n"

          RUY_LOAD_ONE_ROW(0)
          RUY_LOAD_ONE_ROW(1)
          RUY_LOAD_ONE_ROW(2)
          // Here we know that w2==3, so RUY_LOAD_ONE_ROW(3) would be a no-op.
#undef RUY_LOAD_ONE_ROW
          "5:\n"

          // Transpose that last zero-padded 4x4 block.
          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          // Store that last zero-padded block to the packed matrix.
          "mov x1, #32\n"
#define RUY_STORE_ONE_ROW(ROW, REGISTER)                  \
          "cmp w2, #" #ROW "\n"                           \
          "beq 4f\n"                                      \
          "st1 {" #REGISTER ".4s}, [%[packed_ptr]], x1\n"

          RUY_STORE_ONE_ROW(0, v20)
          RUY_STORE_ONE_ROW(1, v21)
          RUY_STORE_ONE_ROW(2, v22)
          RUY_STORE_ONE_ROW(3, v23)

#undef RUY_STORE_ONE_ROW

          "4:\n"

      // clang-format on

      : [src_ptr0] "+r"(src_ptr0), [src_ptr1] "+r"(src_ptr1),
        [src_ptr2] "+r"(src_ptr2), [src_ptr3] "+r"(src_ptr3),
        [packed_ptr] "+r"(packed_ptr)
      : [src_inc0] "r"(static_cast<std::int64_t>(src_inc0)),
        [src_inc1] "r"(static_cast<std::int64_t>(src_inc1)),
        [src_inc2] "r"(static_cast<std::int64_t>(src_inc2)),
        [src_inc3] "r"(static_cast<std::int64_t>(src_inc3)),
        [rows] "r"(src_rows)
      : "cc", "memory", "x1", "x2", "x10", "x11", "x12", "x13", "v0", "v1",
        "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12",
        "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22",
        "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31");
}
#endif

#if RUY_PLATFORM_NEON_32 && RUY_OPT(ASM)
void PackFloatColMajorForNeon(const float* src_ptr0, const float* src_ptr1,
                              const float* src_ptr2, const float* src_ptr3,
                              int src_inc, int src_rows, float* packed_ptr,
                              int output_stride) {
  profiler::ScopeLabel label("Pack (kNeon)");
  asm volatile(
      // clang-format off
          "mov r1, #0\n"
          "and r2, %[rows], #-4\n"
          "cmp r1, r2\n"
          "beq 3f\n"
#define RUY_LOAD_FOUR_BY_FOUR()               \
  /* Load q0 */                               \
  "vld1.32 {d0, d1}, [%[src_ptr0]]\n"         \
  /* if src_inc0 != 0, add 16 to src_ptr0 */  \
  "and r3, %[src_inc], #1\n"                  \
  "add %[src_ptr0], %[src_ptr0], r3, lsl #4\n"\
  /* Load q1 */                               \
  "vld1.32 {d2, d3}, [%[src_ptr1]]\n"         \
  /* if src_inc1 != 0, add 16 to src_ptr0 */  \
  "and r3, %[src_inc], #2\n"                  \
  "add %[src_ptr1], %[src_ptr1], r3, lsl #3\n"\
  /* Load q2 */                               \
  "vld1.32 {d4, d5}, [%[src_ptr2]]\n"         \
  /* if src_inc2 != 0, add 16 to src_ptr0 */  \
  "and r3, %[src_inc], #4\n"                  \
  "add %[src_ptr2], %[src_ptr2], r3, lsl #2\n"\
  /* Load q3 */                               \
  "vld1.32 {d6, d7}, [%[src_ptr3]]\n"         \
  /* if src_inc3 != 0, add 16 to src_ptr0 */  \
  "and r3, %[src_inc], #8\n"                  \
  "add %[src_ptr3], %[src_ptr3], r3, lsl #1\n"\

          RUY_LOAD_FOUR_BY_FOUR()
          "add r1, r1, #4\n"
          "cmp r1, r2\n"

          "beq 2f\n"

          "1:\n"
          "add r1, r1, #4\n"

          // Transpose 4x4 matrix.
          "vzip.32 q0, q1\n"
          "vzip.32 q2, q3\n"

          "vtrn.32 q0, q2\n"
          "vtrn.32 q1, q3\n"

          "vzip.32 q0, q2\n"
          "vzip.32 q1, q3\n"

          "vmov q8, q0\n"
          "vmov q9, q1\n"
          "vmov q10, q2\n"
          "vmov q11, q3\n"

          RUY_LOAD_FOUR_BY_FOUR()
#undef RUY_LOAD_FOUR_BY_FOUR

#define RUY_STORE_FOUR_BY_FOUR()                  \
  /* Store q8, q10, q9, q11 */                    \
  /* q8 = d16, d17 */                             \
  "vst1.32 {d16, d17}, [%[packed_ptr]]\n"         \
  /* q10 = d20, d21 */                            \
  "add %[packed_ptr], %[packed_ptr], %[stride]\n" \
  "vst1.32 {d20, d21}, [%[packed_ptr]]\n"         \
  /* q9 = d18, d19 */                             \
  "add %[packed_ptr], %[packed_ptr], %[stride]\n" \
  "vst1.32 {d18, d19}, [%[packed_ptr]]\n"         \
  /* q11 = d22, d23 */                            \
  "add %[packed_ptr], %[packed_ptr], %[stride]\n" \
  "vst1.32 {d22, d23}, [%[packed_ptr]]\n"         \
  "add %[packed_ptr], %[packed_ptr], %[stride]\n" \

          RUY_STORE_FOUR_BY_FOUR()
          "cmp r1, r2\n"

          "bne 1b\n"

          "2:\n"

          // Transpose 4x4 matrix.
          "vzip.32 q0, q1\n"
          "vzip.32 q2, q3\n"

          "vtrn.32 q0, q2\n"
          "vtrn.32 q1, q3\n"

          "vzip.32 q0, q2\n"
          "vzip.32 q1, q3\n"

          "vmov q8, q0\n"
          "vmov q9, q1\n"
          "vmov q10, q2\n"
          "vmov q11, q3\n"

          RUY_STORE_FOUR_BY_FOUR()
#undef RUY_STORE_FOUR_BY_FOUR
          "3:\n"

          "ands r2, %[rows], #3\n"
          "beq 4f\n"
          "mov r0, #0\n"
          // Zero out q0 - q3
          "vdup.32 q0, r0\n"
          "vdup.32 q1, r0\n"
          "vdup.32 q2, r0\n"
          "vdup.32 q3, r0\n"
#define RUY_LOAD_ONE_ROW_FIRST_HALF(R, I)    \
  "cmp r2, #" #R "\n"                        \
  "beq 5f\n"                                 \
  "vld1.32 { d0[" #I "] }, [%[src_ptr0]]!\n" \
  "vld1.32 { d2[" #I "] }, [%[src_ptr1]]!\n" \
  "vld1.32 { d4[" #I "] }, [%[src_ptr2]]!\n" \
  "vld1.32 { d6[" #I "] }, [%[src_ptr3]]!\n"

#define RUY_LOAD_ONE_ROW_SECOND_HALF(R, I)      \
  "cmp r2, #" #R "\n"                        \
  "beq 5f\n"                                 \
  "vld1.32 { d1[" #I "] }, [%[src_ptr0]]!\n" \
  "vld1.32 { d3[" #I "] }, [%[src_ptr1]]!\n" \
  "vld1.32 { d5[" #I "] }, [%[src_ptr2]]!\n" \
  "vld1.32 { d7[" #I "] }, [%[src_ptr3]]!\n"

          RUY_LOAD_ONE_ROW_FIRST_HALF(0, 0)
          RUY_LOAD_ONE_ROW_FIRST_HALF(1, 1)
          RUY_LOAD_ONE_ROW_SECOND_HALF(2, 0)
          RUY_LOAD_ONE_ROW_SECOND_HALF(3, 1)
#undef RUY_LOAD_ONE_ROW_SECOND_HALF
#undef RUY_LOAD_ONE_ROW_FIRST_HALF
          "5:\n"

          // Transpose 4x4 matrix.
          "vzip.32 q0, q1\n"
          "vzip.32 q2, q3\n"

          "vtrn.32 q0, q2\n"
          "vtrn.32 q1, q3\n"

          "vzip.32 q0, q2\n"
          "vzip.32 q1, q3\n"

          "vmov q8, q0\n"
          "vmov q9, q1\n"
          "vmov q10, q2\n"
          "vmov q11, q3\n"

          "mov r1, #32\n"

#define RUY_STORE_ONE_ROW(ROW, REGISTER)      \
          "cmp r2, #" #ROW "\n"                           \
          "beq 4f\n"                                      \
          "vst1.32 {" #REGISTER "}, [%[packed_ptr]]\n"    \
          "add %[packed_ptr], %[packed_ptr], %[stride]\n"

          // Store q8
          RUY_STORE_ONE_ROW(0, q8)
          // Store q10
          RUY_STORE_ONE_ROW(1, q10)
          // Store q9
          RUY_STORE_ONE_ROW(2, q9)
          // Store q11
          RUY_STORE_ONE_ROW(3, q11)

#undef RUY_STORE_ONE_ROW

          "4:\n"

      // clang-format on
      : [ src_ptr0 ] "+r"(src_ptr0), [ src_ptr1 ] "+r"(src_ptr1),
        [ src_ptr2 ] "+r"(src_ptr2), [ src_ptr3 ] "+r"(src_ptr3),
        [ packed_ptr ] "+r"(packed_ptr)
      : [ src_inc ] "r"(static_cast<std::int64_t>(src_inc)),
        [ rows ] "r"(src_rows), [ stride ] "r"(output_stride)
      : "cc", "memory", "r0", "r1", "r2", "r3", "q0", "q1", "q2", "q3",
        "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11");
}

#endif  // (RUY_PLATFORM_NEON_32

#if RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)
void PackFloatColMajorForNeonA55ish(const float* src_ptr0,
                                    const float* src_ptr1,
                                    const float* src_ptr2,
                                    const float* src_ptr3, int src_inc0,
                                    int src_inc1, int src_inc2, int src_inc3,
                                    int src_rows, float* packed_ptr) {
  profiler::ScopeLabel label("Pack (kNeon, optimized for in-order cores)");

  asm volatile(
          // clang-format off
          "mov w1, #0\n"

          "and w2, %w[rows], #-4\n"
          "cmp w1, w2\n"
          "beq 3f\n"
          "ld1 {v0.4s}, [%[src_ptr0]], %[src_inc0]\n"
          "ld1 {v1.4s}, [%[src_ptr1]], %[src_inc1]\n"
          "ld1 {v2.4s}, [%[src_ptr2]], %[src_inc2]\n"
          "ld1 {v3.4s}, [%[src_ptr3]], %[src_inc3]\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #64]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #128]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #192]\n")
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #192]\n")
          "add w1, w1, #4\n"
          "cmp w1, w2\n"

          "beq 2f\n"

          "1:\n"
          "add w1, w1, #4\n"

          "ldr x10, [%[src_ptr0], #8]\n"
          "trn1 v16.4s, v0.4s, v1.4s\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr0], #240]\n")
          "ldr x11, [%[src_ptr1], #8]\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr1], #240]\n")
          "ldr x12, [%[src_ptr2], #8]\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr2], #240]\n")
          "ldr x13, [%[src_ptr3], #8]\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"
          RUY_PREFETCH_LOAD("prfm pldl1strm, [%[src_ptr3], #240]\n")

          "ld1 {v0.2s}, [%[src_ptr0]], %[src_inc0]\n"
          "trn1 v20.2d, v16.2d, v18.2d\n"
          "ld1 {v1.2s}, [%[src_ptr1]], %[src_inc1]\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "ld1 {v2.2s}, [%[src_ptr2]], %[src_inc2]\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "ld1 {v3.2s}, [%[src_ptr3]], %[src_inc3]\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"
          "cmp w1, w2\n"

          "ins v0.d[1], x10\n"
          "str q20, [%[packed_ptr], #0]\n"
          "ins v1.d[1], x11\n"
          "str q21, [%[packed_ptr], #32]\n"
          "ins v2.d[1], x12\n"
          "str q22, [%[packed_ptr], #64]\n"
          "ins v3.d[1], x13\n"
          "str q23, [%[packed_ptr], #96]\n"

          "add %[packed_ptr], %[packed_ptr], #128\n"

          "bne 1b\n"

          "2:\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          "str q20, [%[packed_ptr], #0]\n"
          "str q21, [%[packed_ptr], #32]\n"
          "str q22, [%[packed_ptr], #64]\n"
          "str q23, [%[packed_ptr], #96]\n"
          "add %[packed_ptr], %[packed_ptr], #128\n"

          "3:\n"

          "ands w2, %w[rows], #3\n"
          "beq 4f\n"
          "movi v0.16b, #0\n"
          "movi v1.16b, #0\n"
          "movi v2.16b, #0\n"
          "movi v3.16b, #0\n"
#define RUY_LOAD_ONE_ROW(R)                   \
  "cmp w2, #" #R "\n"                         \
  "beq 5f\n"                                  \
  "ld1 { v0.s }[" #R "], [%[src_ptr0]], #4\n" \
  "ld1 { v1.s }[" #R "], [%[src_ptr1]], #4\n" \
  "ld1 { v2.s }[" #R "], [%[src_ptr2]], #4\n" \
  "ld1 { v3.s }[" #R "], [%[src_ptr3]], #4\n"

          RUY_LOAD_ONE_ROW(0)
          RUY_LOAD_ONE_ROW(1)
          RUY_LOAD_ONE_ROW(2)
          RUY_LOAD_ONE_ROW(3)
#undef RUY_LOAD_ONE_ROW
          "5:\n"

          "trn1 v16.4s, v0.4s, v1.4s\n"
          "trn2 v17.4s, v0.4s, v1.4s\n"
          "trn1 v18.4s, v2.4s, v3.4s\n"
          "trn2 v19.4s, v2.4s, v3.4s\n"

          "trn1 v20.2d, v16.2d, v18.2d\n"
          "trn2 v22.2d, v16.2d, v18.2d\n"
          "trn1 v21.2d, v17.2d, v19.2d\n"
          "trn2 v23.2d, v17.2d, v19.2d\n"

          "mov x1, #32\n"

#define RUY_STORE_ONE_ROW(ROW, REGISTER)                  \
          "cmp w2, #" #ROW "\n"                           \
          "beq 4f\n"                                      \
          "st1 {" #REGISTER ".4s}, [%[packed_ptr]], x1\n"

          RUY_STORE_ONE_ROW(0, v20)
          RUY_STORE_ONE_ROW(1, v21)
          RUY_STORE_ONE_ROW(2, v22)
          RUY_STORE_ONE_ROW(3, v23)

#undef RUY_STORE_ONE_ROW

          "4:\n"

          // clang-format on

          : [ src_ptr0 ] "+r"(src_ptr0), [src_ptr1] "+r"(src_ptr1), [src_ptr2] "+r"(src_ptr2),
            [src_ptr3] "+r"(src_ptr3), [packed_ptr] "+r"(packed_ptr)
          : [ src_inc0 ] "r"(static_cast<std::int64_t>(src_inc0)), [src_inc1] "r"(static_cast<std::int64_t>(src_inc1)), [src_inc2] "r"(static_cast<std::int64_t>(src_inc2)),
            [src_inc3] "r"(static_cast<std::int64_t>(src_inc3)), [rows] "r"(src_rows)
          : "cc", "memory", "x1", "x2", "x10", "x11", "x12", "x13", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
            "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21",
            "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31");
}
#endif  // RUY_PLATFORM_NEON_64 && RUY_OPT(ASM)

#if RUY_PLATFORM_NEON

namespace {
// transpose_*bit_vals are wrappers around ARM TRN1 instructions, allowing
// to use these instructions like we would in assembly --- this is one instance
// where assembly is more idiomatic than intrinsics.
//
// The way that TRN1 is exposed by vtrn_* intrinsics makes its usage very
// cumbersome. The issue is that transposing grouped of values has been exposed
// only as transposing values of a wider type, so this requires many
// vreinterpret's, and to make it worse, vtrn_* return NEON array types like
// int8x8x2_t for which vreinterpret's are not defined!
void transpose_8bit_vals(int8x8_t& a, int8x8_t& b) {
  int8x8x2_t t = vtrn_s8(a, b);
  a = t.val[0];
  b = t.val[1];
}

void transpose_16bit_vals(int8x8_t& a, int8x8_t& b) {
  int16x4x2_t t = vtrn_s16(vreinterpret_s16_s8(a), vreinterpret_s16_s8(b));
  a = vreinterpret_s8_s16(t.val[0]);
  b = vreinterpret_s8_s16(t.val[1]);
}

void transpose_32bit_vals(int8x8_t& a, int8x8_t& b) {
  int32x2x2_t t = vtrn_s32(vreinterpret_s32_s8(a), vreinterpret_s32_s8(b));
  a = vreinterpret_s8_s32(t.val[0]);
  b = vreinterpret_s8_s32(t.val[1]);
}
}  // namespace

void Pack8bitRowMajorForNeon(const std::uint8_t* src_ptr, int src_stride,
                             int src_rows, int src_cols, int block_row,
                             int start_col, int end_col,
                             std::int8_t* packed_ptr, int packed_stride,
                             int packed_zero_point, std::int32_t* sums,
                             int input_xor, int kernel_cols) {
  profiler::ScopeLabel label("Pack (kNeon, from row-major)");

  int src_end_col = std::min(end_col, src_cols);
  int col = start_col;
  for (; col <= src_end_col - 8; col += 8) {
    // Each iteration of this loop handles 8 columns, and the kernel format
    // has 16 rows, so each iteration handles a 16x8 block.
    //
    // Since the source is row-major, handling 8 columns at a time means
    // loading only 8 bytes i.e. 64bit from each row. This may seem surprising
    // on 128bit SIMD like NEON. While we could handle 16 columns at a time,
    // we prefer to stick with 8 for the following reasons:
    // 1. The arithmetic (computing sums and transposing data) done on these
    //    values is such that even though we initially start from 64bit vectors,
    //    most of our NEON instructions are full 128bit instructions. For the
    //    sums computation, that is because summing 8bit values requires
    //    expansion to 16bit anyway. For the matrix transposition code, that is
    //    because the ARM ZIP instructions take 64bit of data from two input
    //    registers and zip it into a 128bit output. If we had 128bit of data
    //    in each input registers, we would need 2x more ARM NEON instructions
    //    to zip it.
    // 2. The main optimization target for this (ARM, 8bit, non-dotprod)
    //    code path is in-order ARM cores such as the Cortex-A53, which prefer
    //    64bit loads anyway.
    // 3. Handling only 8 columns at a time limits the size of the final
    //    leftover columns handled with slow scalar code.
    //
    // This code is not very optimized anyway, as evidenced from the facts that
    // (1) it's written in intrinsics, (2) it's not using separate versions
    // tuned for different types of CPU cores. At the level of optimization that
    // it's working at, this seems like a fair compromise. If one wanted to
    // maximize performance at the cost of more code complexity/size, one could
    // have code handling 16 columns at a time (maybe limited to
    // Tuning::kGeneric), then 8, then 4 to minimize the amount of slow
    // leftovers.
    //
    // Load 8 sums in sums0, sums1.
    int32x4_t sums0 = vld1q_s32(sums + col);
    int32x4_t sums1 = vld1q_s32(sums + col + 4);
    // Load the 8x16 block from the source matrix.
    // Each val* here is the data from one row.
    int8x8_t val0, val1, val2, val3, val4, val5, val6, val7, val8, val9, val10,
        val11, val12, val13, val14, val15;
    // Even though this function takes a uint8_t* src_ptr, that's only a
    // type-erased pointer (using uint8_t* so that pointer arithmetic is
    // allowed). The actual type may be either uint8_t or int8_t. The only
    // difference it makes is that if it's uint8_t then we need to flip the
    // sign bit. This is specified by the input_xor value (which is 0x80 if the
    // input data is uint8_t, and 0x0 otherwise).
    auto load_and_convert = [=](const std::uint8_t* from) {
      return vreinterpret_s8_u8(veor_u8(vdup_n_u8(input_xor), vld1_u8(from)));
    };
    if (block_row <= src_rows - 16) {
      // Load data in the regular case: there are still 16 rows to be read from
      // the source matrix.
      val0 = load_and_convert(src_ptr + 0 * src_stride);
      val1 = load_and_convert(src_ptr + 1 * src_stride);
      val2 = load_and_convert(src_ptr + 2 * src_stride);
      val3 = load_and_convert(src_ptr + 3 * src_stride);
      val4 = load_and_convert(src_ptr + 4 * src_stride);
      val5 = load_and_convert(src_ptr + 5 * src_stride);
      val6 = load_and_convert(src_ptr + 6 * src_stride);
      val7 = load_and_convert(src_ptr + 7 * src_stride);
      val8 = load_and_convert(src_ptr + 8 * src_stride);
      val9 = load_and_convert(src_ptr + 9 * src_stride);
      val10 = load_and_convert(src_ptr + 10 * src_stride);
      val11 = load_and_convert(src_ptr + 11 * src_stride);
      val12 = load_and_convert(src_ptr + 12 * src_stride);
      val13 = load_and_convert(src_ptr + 13 * src_stride);
      val14 = load_and_convert(src_ptr + 14 * src_stride);
      val15 = load_and_convert(src_ptr + 15 * src_stride);
    } else {
      // Boundary case: there are fewer than 16 rows to be read from the source
      // matrix. We pad by the zero_point.
      val0 = vdup_n_s8(packed_zero_point);
      val1 = val0;
      val2 = val0;
      val3 = val0;
      val4 = val0;
      val5 = val0;
      val6 = val0;
      val7 = val0;
      val8 = val0;
      val9 = val0;
      val10 = val0;
      val11 = val0;
      val12 = val0;
      val13 = val0;
      val14 = val0;
      val15 = val0;
      if (block_row + 0 < src_rows)
        val0 = load_and_convert(src_ptr + 0 * src_stride);
      if (block_row + 1 < src_rows)
        val1 = load_and_convert(src_ptr + 1 * src_stride);
      if (block_row + 2 < src_rows)
        val2 = load_and_convert(src_ptr + 2 * src_stride);
      if (block_row + 3 < src_rows)
        val3 = load_and_convert(src_ptr + 3 * src_stride);
      if (block_row + 4 < src_rows)
        val4 = load_and_convert(src_ptr + 4 * src_stride);
      if (block_row + 5 < src_rows)
        val5 = load_and_convert(src_ptr + 5 * src_stride);
      if (block_row + 6 < src_rows)
        val6 = load_and_convert(src_ptr + 6 * src_stride);
      if (block_row + 7 < src_rows)
        val7 = load_and_convert(src_ptr + 7 * src_stride);
      if (block_row + 8 < src_rows)
        val8 = load_and_convert(src_ptr + 8 * src_stride);
      if (block_row + 9 < src_rows)
        val9 = load_and_convert(src_ptr + 9 * src_stride);
      if (block_row + 10 < src_rows)
        val10 = load_and_convert(src_ptr + 10 * src_stride);
      if (block_row + 11 < src_rows)
        val11 = load_and_convert(src_ptr + 11 * src_stride);
      if (block_row + 12 < src_rows)
        val12 = load_and_convert(src_ptr + 12 * src_stride);
      if (block_row + 13 < src_rows)
        val13 = load_and_convert(src_ptr + 13 * src_stride);
      if (block_row + 14 < src_rows)
        val14 = load_and_convert(src_ptr + 14 * src_stride);
      if (block_row + 15 < src_rows)
        val15 = load_and_convert(src_ptr + 15 * src_stride);
    }
    src_ptr += 8;
    // Compute sums.
    int16x8_t sums16_0 = vaddl_s8(val0, val1);
    int16x8_t sums16_1 = vaddl_s8(val2, val3);
    sums16_0 = vaddq_s16(sums16_0, vaddl_s8(val4, val5));
    sums16_1 = vaddq_s16(sums16_1, vaddl_s8(val6, val7));
    sums16_0 = vaddq_s16(sums16_0, vaddl_s8(val8, val9));
    sums16_1 = vaddq_s16(sums16_1, vaddl_s8(val10, val11));
    sums16_0 = vaddq_s16(sums16_0, vaddl_s8(val12, val13));
    sums16_1 = vaddq_s16(sums16_1, vaddl_s8(val14, val15));
    int16x8_t sums16 = vaddq_s16(sums16_0, sums16_1);
    sums0 = vaddw_s16(sums0, vget_low_s16(sums16));
    sums1 = vaddw_s16(sums1, vget_high_s16(sums16));
    // Store sums.
    vst1q_s32(sums + col, sums0);
    vst1q_s32(sums + col + 4, sums1);

    // Transpose the data, i.e. change the storage order of the
    // 16x8 block, to convert from the row-major source to the
    // column-major packed format.
    //
    // Before, for i in [0, 15], val<i> is the i-th row.
    // After, for i in [0, 7], { val<i> val<i+8> } is the i-th column.
    transpose_8bit_vals(val0, val1);
    transpose_8bit_vals(val2, val3);
    transpose_8bit_vals(val4, val5);
    transpose_8bit_vals(val6, val7);
    transpose_8bit_vals(val8, val9);
    transpose_8bit_vals(val10, val11);
    transpose_8bit_vals(val12, val13);
    transpose_8bit_vals(val14, val15);
    transpose_16bit_vals(val0, val2);
    transpose_16bit_vals(val1, val3);
    transpose_16bit_vals(val4, val6);
    transpose_16bit_vals(val5, val7);
    transpose_16bit_vals(val8, val10);
    transpose_16bit_vals(val9, val11);
    transpose_16bit_vals(val12, val14);
    transpose_16bit_vals(val13, val15);
    transpose_32bit_vals(val0, val4);
    transpose_32bit_vals(val1, val5);
    transpose_32bit_vals(val2, val6);
    transpose_32bit_vals(val3, val7);
    transpose_32bit_vals(val8, val12);
    transpose_32bit_vals(val9, val13);
    transpose_32bit_vals(val10, val14);
    transpose_32bit_vals(val11, val15);
    // Store to the packed_matrix.
    std::int8_t* dst_ptr = packed_ptr;
    vst1q_s8(dst_ptr, vcombine_s8(val0, val8));
    vst1q_s8(dst_ptr + 16, vcombine_s8(val1, val9));
    dst_ptr += (kernel_cols == 2) ? 2 * packed_stride : 32;
    vst1q_s8(dst_ptr, vcombine_s8(val2, val10));
    vst1q_s8(dst_ptr + 16, vcombine_s8(val3, val11));
    packed_ptr += 4 * packed_stride;
    dst_ptr = packed_ptr;
    vst1q_s8(dst_ptr, vcombine_s8(val4, val12));
    vst1q_s8(dst_ptr + 16, vcombine_s8(val5, val13));
    dst_ptr += (kernel_cols == 2) ? 2 * packed_stride : 32;
    vst1q_s8(dst_ptr, vcombine_s8(val6, val14));
    vst1q_s8(dst_ptr + 16, vcombine_s8(val7, val15));
    packed_ptr += 4 * packed_stride;
  }
  // Handle remaining columns, not fitting in a full block of 8 columns, but
  // still true columns frome the source matrix (as opposed to the final columns
  // below).
  for (; col < src_end_col; col++) {
    std::int32_t accum = 0;
    std::int8_t* dst_ptr = packed_ptr + (col & (kernel_cols - 1)) * 16;
    for (int r = 0; r < 16; r++) {
      std::int8_t packed_val = (block_row + r < src_rows)
                                   ? (src_ptr[r * src_stride] ^ input_xor)
                                   : packed_zero_point;
      accum += packed_val;
      dst_ptr[r] = packed_val;
    }
    if (sums) {
      sums[col] += accum;
    }
    src_ptr++;
    if (((col + 1) & (kernel_cols - 1)) == 0) {
      packed_ptr += kernel_cols * packed_stride;
    }
  }
  // Handle the final columns of the packed matrix, beyond the last column of
  // the source matrix. The values here don't matter, we just want to avoid
  // leaving uninitialized data. Since the sums are already initialized above,
  // we don't need to do anything about them here.
  for (; col < end_col; col++) {
    std::int8_t* dst_ptr = packed_ptr + (col & (kernel_cols - 1)) * 16;
    std::memset(dst_ptr, 0, 16);
    if (((col + 1) & (kernel_cols - 1)) == 0) {
      packed_ptr += kernel_cols * packed_stride;
    }
  }
}

#endif

}  // namespace ruy
