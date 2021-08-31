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

#include "ruy/block_map.h"

#include <algorithm>
#include <cstdint>
#include <limits>

#ifdef RUY_MAKEBLOCKMAP_DEBUG
#include <cstdio>
#include <cstdlib>
#include <string>
#endif

#include "ruy/check_macros.h"
#include "ruy/opt_set.h"
#include "ruy/profiler/instrumentation.h"
#include "ruy/size_util.h"
#include "ruy/trace.h"

namespace ruy {

namespace {

void DecodeTraversalLinear(int size_log2, std::uint32_t square_index,
                           SidePair<int>* local_pos) {
  (*local_pos)[Side::kLhs] = square_index & ((1 << size_log2) - 1);
  (*local_pos)[Side::kRhs] = square_index >> size_log2;
}

void DecodeTraversalFractalZ(std::uint32_t square_index,
                             SidePair<int>* local_pos) {
  const std::uint32_t n1 = square_index;
  const std::uint32_t n2 = (n1 & 0x99999999u) | ((n1 & 0x44444444u) >> 1) |
                           ((n1 & 0x22222222u) << 1);
  const std::uint32_t n4 = (n2 & 0xc3c3c3c3u) | ((n2 & 0x30303030u) >> 2) |
                           ((n2 & 0x0c0c0c0cu) << 2);
  const std::uint32_t n8 = (n4 & 0xf00ff00fu) | ((n4 & 0x0f000f00u) >> 4) |
                           ((n4 & 0x00f000f0u) << 4);
  const std::uint32_t n16 = (n8 & 0xff0000ffu) | ((n8 & 0x00ff0000u) >> 8) |
                            ((n8 & 0x0000ff00u) << 8);
  (*local_pos)[Side::kLhs] = n16 & 0xffff;
  (*local_pos)[Side::kRhs] = n16 >> 16;
}

void DecodeTraversalFractalU(std::uint32_t square_index,
                             SidePair<int>* local_pos) {
  DecodeTraversalFractalZ(square_index, local_pos);
  // Change fractal z-order to u-order
  (*local_pos)[Side::kLhs] ^= (*local_pos)[Side::kRhs];
}

// Code inspired by the sample code in
//   https://en.wikipedia.org/wiki/Hilbert_curve
// The main optimization is to avoid hard-to-predict conditional branches
// based on the bits of the square_index parameter.
void DecodeTraversalFractalHilbert(int size_log2, std::uint32_t square_index,
                                   SidePair<int>* local_pos) {
  std::uint32_t t = square_index;
  std::uint32_t x = 0;
  std::uint32_t y = 0;
  // Easy-to-predict for loop, the number of iterations is the same for
  // an entire GEMM.
  for (int sb = 0; sb < size_log2; sb++) {
    std::uint32_t s = 1 << sb;
    bool rx = t & 2;
    bool ry = (t & 1) ^ rx;
    std::uint32_t tmp = rx ? (s - 1 - x) : x;
    x = ry ? x : rx ? (s - 1 - y) : y;
    y = ry ? (y + s) : tmp;
    x = rx ? (x + s) : x;
    t >>= 2;
  }
  (*local_pos)[Side::kLhs] = y;
  (*local_pos)[Side::kRhs] = x;
}

}  // end anonymous namespace

void GetBlockByIndex(const BlockMap& block_map, int index,
                     SidePair<int>* block) {
  profiler::ScopeLabel label("GetBlockByIndex");
  const std::uint32_t index_u32 = index;

  const std::uint32_t num_blocks_per_local_curve =
      1u << (2 * block_map.num_blocks_base_log2);
  const std::uint32_t square_index =
      index_u32 & (num_blocks_per_local_curve - 1);

  const int size_log2 = block_map.num_blocks_base_log2;
  SidePair<int> local_pos;
  switch (block_map.traversal_order) {
    case BlockMapTraversalOrder::kFractalZ:
      DecodeTraversalFractalZ(square_index, &local_pos);
      break;
    case BlockMapTraversalOrder::kFractalU:
      DecodeTraversalFractalU(square_index, &local_pos);
      break;
    case BlockMapTraversalOrder::kFractalHilbert:
      DecodeTraversalFractalHilbert(size_log2, square_index, &local_pos);
      break;
    default:
      RUY_DCHECK(block_map.traversal_order == BlockMapTraversalOrder::kLinear);
      DecodeTraversalLinear(size_log2, square_index, &local_pos);
      break;
  }

  const std::uint32_t rectangular_index =
      index_u32 >> 2 * block_map.num_blocks_base_log2;
  for (Side side : {Side::kLhs, Side::kRhs}) {
    const std::uint32_t mask = (1u << block_map.rectangularness_log2[side]) - 1;
    const int rectangular_offset = (rectangular_index & mask)
                                   << block_map.num_blocks_base_log2;
    (*block)[side] = local_pos[side] + rectangular_offset;
  }
}

namespace {

BlockMapTraversalOrder GetTraversalOrder(
    int rows_after_rectangularness_division,
    int cols_after_rectangularness_division, int depth, int lhs_scalar_size,
    int rhs_scalar_size, const CpuCacheParams& cpu_cache_params) {
  static constexpr bool kAnyFractal =
      RUY_OPT(FRACTAL_Z) | RUY_OPT(FRACTAL_U) | RUY_OPT(FRACTAL_HILBERT);
  const int working_set_size =
      (lhs_scalar_size * rows_after_rectangularness_division +
       rhs_scalar_size * cols_after_rectangularness_division) *
      depth;
  if (kAnyFractal && (working_set_size > cpu_cache_params.local_cache_size)) {
    if (RUY_OPT(FRACTAL_HILBERT) &&
        (working_set_size > cpu_cache_params.last_level_cache_size)) {
      return BlockMapTraversalOrder::kFractalHilbert;
    } else if (RUY_OPT(FRACTAL_U)) {
      return BlockMapTraversalOrder::kFractalU;
    } else {
      return BlockMapTraversalOrder::kFractalZ;
    }
  } else {
    return BlockMapTraversalOrder::kLinear;
  }
}

int floor_log2_quotient(int num, int denom) {
  if (num <= denom) {
    return 0;
  }
  int log2_quotient = floor_log2(num) - ceil_log2(denom);
  if ((denom << (log2_quotient + 1)) <= num) {
    log2_quotient++;
  }
  return log2_quotient;
}

// Computes the rectangularness of the matrix shape (rows, cols). This is
// essentially just the log2 of the quotient (rows / cols). The kernel_rows and
// kernel_cols only get into the picture for clamping bounds but don't affect
// the generic computation.
void GetRectangularness(int rows, int cols, int kernel_rows, int kernel_cols,
                        int* rows_rectangularness_log2,
                        int* cols_rectangularness_log2) {
  *rows_rectangularness_log2 = 0;
  *cols_rectangularness_log2 = 0;

  // In GEMV-ish cases, that is when kernel blocks are as narrow as the kernel
  // itself, we risk having too small kernel blocks for good kernel
  // amortization. We avoid that by limiting recangularness so that kernel
  // blocks are not too tiny at least in that dimension. Specifically, we try to
  // have at least (2^min_kernel_inner_loop_runs_log2) kernels fitting in each
  // kernel block along the large dimension.
  const int min_kernel_inner_loop_runs_log2 = 3;
  if (rows > cols) {
    int cols_of_kernel_inner_loop_runs_log2 =
        ceil_log2(cols) - pot_log2(kernel_cols);
    int min_rows_of_kernel_inner_loop_runs_log2 =
        std::max(0, min_kernel_inner_loop_runs_log2 -
                        cols_of_kernel_inner_loop_runs_log2);
    *rows_rectangularness_log2 =
        std::min(floor_log2_quotient(rows, cols),
                 std::max(0, floor_log2(rows) - pot_log2(kernel_rows) -
                                 min_rows_of_kernel_inner_loop_runs_log2));
    // Sanity check that we did not over-estimate rows_rectangularness_log2.
    RUY_DCHECK_GE(rows >> *rows_rectangularness_log2, cols);
  } else if (cols > rows) {
    int rows_of_kernel_inner_loop_runs_log2 =
        ceil_log2(rows) - pot_log2(kernel_rows);
    int min_cols_of_kernel_inner_loop_runs_log2 =
        std::max(0, min_kernel_inner_loop_runs_log2 -
                        rows_of_kernel_inner_loop_runs_log2);
    *cols_rectangularness_log2 =
        std::min(floor_log2_quotient(cols, rows),
                 std::max(0, floor_log2(cols) - pot_log2(kernel_cols) -
                                 min_cols_of_kernel_inner_loop_runs_log2));
    // Sanity check that we did not over-estimate cols_rectangularness_log2.
    RUY_DCHECK_GE(cols >> *cols_rectangularness_log2, rows);
  }
  RUY_DCHECK(!*rows_rectangularness_log2 || !*cols_rectangularness_log2);
}

// Computes a 'multithreading score'. When multithreading, we need there to
// be at least as many tiles as there are threads, and hopefully
// substantially more than that, so we benefit from ruy's ability to
// dispatch fine-grained workloads to threads.
int GetMultithreadingScore(int block_size_log2, int rows, int cols,
                           int tentative_thread_count) {
  const int num_full_blocks_of_rows = rows >> block_size_log2;
  const int num_full_blocks_of_cols = cols >> block_size_log2;
  const int candidate_num_full_blocks_log2 = floor_log2(
      std::max(1, num_full_blocks_of_rows * num_full_blocks_of_cols));

  // The values here have been tuned on ARM Cortex-A55.
  // We expect this to have to be tuned differently for other CPUs.
  if (tentative_thread_count == 1) {
    return 0;
  } else {
    const int blocks_per_thread_log2 =
        candidate_num_full_blocks_log2 - ceil_log2(tentative_thread_count);
    if (blocks_per_thread_log2 < 0) {
      return -64;
    } else if (blocks_per_thread_log2 == 0) {
      return -16;
    } else if (blocks_per_thread_log2 == 1) {
      return -8;
    } else if (blocks_per_thread_log2 == 2) {
      return 0;
    } else if (blocks_per_thread_log2 == 3) {
      return 8;
    } else {
      return 16;
    }
  }
}

// Computes a 'cache locality score'.
int GetCacheLocalityScore(int block_size_log2, int rows, int cols, int depth,
                          int kernel_rows_log2, int kernel_cols_log2,
                          int lhs_scalar_size, int rhs_scalar_size,
                          const CpuCacheParams& cpu_cache_params) {
  // In the narrow case (e.g. matrix*vector), each byte of the big operand
  // matrix (either LHS or RHS) is traversed only once, so any notion of data
  // locality is irrelevant. Ignore the 'cache locality score' by forcing it to
  // be 0 in that case.
  if (rows <= (1 << kernel_rows_log2) || cols <= (1 << kernel_cols_log2)) {
    return 0;
  }
  const int block_rows = std::min(1 << block_size_log2, rows);
  const int block_cols = std::min(1 << block_size_log2, cols);
  const int total_read_bytes =
      (lhs_scalar_size * block_rows + rhs_scalar_size * block_cols) * depth;
  const int total_read_bytes_log2 = ceil_log2(total_read_bytes);
  const int nonlocality_log2 =
      total_read_bytes_log2 - floor_log2(cpu_cache_params.local_cache_size);
  // The values here have been tuned on ARM Cortex-A55.
  // We expect this to have to be tuned differently for other CPUs.
  if (nonlocality_log2 < -1) {
    return 64;
  } else if (nonlocality_log2 == -1) {
    return 56;
  } else if (nonlocality_log2 == 0) {
    return 48;
  } else if (nonlocality_log2 == 1) {
    return 32;
  } else if (nonlocality_log2 == 2) {
    return 16;
  } else if (nonlocality_log2 == 3) {
    return 0;
  } else {
    return -64;
  }
}

// Compute a 'kernel amortization score'. This is the notion that very small
// tiles result in more overhead outside of kernels, more complex memory
// access patterns and less benefits from ruy's fat kernels, so we reward
// larger blocks more than smaller ones.
int GetKernelAmortizationScore(int block_size_log2, int rows, int cols,
                               int kernel_rows_log2, int kernel_cols_log2) {
  const int block_rows = std::min(1 << block_size_log2, rows);
  const int block_cols = std::min(1 << block_size_log2, cols);
  const int kernels_per_block_log2 =
      floor_log2(block_rows * block_cols) - kernel_rows_log2 - kernel_cols_log2;
  RUY_DCHECK_GE(kernels_per_block_log2, 0);
  // The values here have been tuned on ARM Cortex-A55.
  // We expect this to have to be tuned differently for other CPUs.
  if (kernels_per_block_log2 == 0) {
    return 0;
  } else if (kernels_per_block_log2 == 1) {
    return 8;
  } else if (kernels_per_block_log2 == 2) {
    return 16;
  } else if (kernels_per_block_log2 == 3) {
    return 24;
  } else if (kernels_per_block_log2 == 4) {
    return 32;
  } else if (kernels_per_block_log2 == 5) {
    return 40;
  } else if (kernels_per_block_log2 == 6) {
    return 48;
  } else if (kernels_per_block_log2 == 7) {
    return 56;
  } else {
    return 64;
  }
}

}  // namespace

bool IsObviouslyLinearTraversal(int rows, int cols, int depth,
                                int lhs_scalar_size, int rhs_scalar_size,
                                const CpuCacheParams& cpu_cache_params) {
  if (rows == 1 || cols == 1) {
    return true;
  }
  // Normally, GetTraversalOrder wants the dimensions (rows x cols) divided
  // by the rectangularness factors, since any non-linear traversal order will
  // be local to each subdivision. In the present function, we don't know the
  // rectangularness factors yet, and we can't just call GetRectangularness
  // as that requires knowing the kernel block layout. Since we just want
  // a coarse estimate with only the guarantee that if we return `true` then
  // linear traversal will be used, it is OK here to over-estimate `rows` and
  // `cols`, by omitting to divide them by the rectangularness factors.
  return GetTraversalOrder(rows, cols, depth, lhs_scalar_size, rhs_scalar_size,
                           cpu_cache_params) == BlockMapTraversalOrder::kLinear;
}

void MakeBlockMap(int rows, int cols, int depth, int kernel_rows,
                  int kernel_cols, int lhs_scalar_size, int rhs_scalar_size,
                  int tentative_thread_count,
                  const CpuCacheParams& cpu_cache_params, BlockMap* block_map) {
  RUY_TRACE_SCOPE;
  profiler::ScopeLabel label("MakeBlockMap");

  RUY_DCHECK_GE(rows, kernel_rows);
  RUY_DCHECK_GE(cols, kernel_cols);
  RUY_DCHECK_EQ(rows % kernel_rows, 0);
  RUY_DCHECK_EQ(cols % kernel_cols, 0);

  // Estimate the 'rectangularness', the first level of subdivision bringing
  // the shape to within 2x of a square shape.
  int rows_rectangularness_log2 = 0;
  int cols_rectangularness_log2 = 0;
  GetRectangularness(rows, cols, kernel_rows, kernel_cols,
                     &rows_rectangularness_log2, &cols_rectangularness_log2);

  const int kernel_rows_log2 = pot_log2(kernel_rows);
  const int kernel_cols_log2 = pot_log2(kernel_cols);
  const int kernel_size_log2 = std::max(kernel_cols_log2, kernel_rows_log2);

  const int size = std::min(rows, cols);
  const int size_log2 = std::max(kernel_size_log2, floor_log2(size));

  RUY_DCHECK_GE(size_log2, kernel_size_log2);

  // Heuristic selecting the power-of-two grid subdivision insider of each
  // square-ish region (past the above subdivision by 'rectangularness').
  // Note that it is the number of subdivisions, not the resulting block size,
  // that will be a power of two. But inside of that heuristic, it simplifies
  // code to talk in terms of 'block_size_log2', as if it were the block size
  // that were a power of two. This 'block_size_log2' is to be interpreted as
  // "log2 rounded below", e.g. when block_size_log2=8 we might have a block
  // size in [256, 511]. When the shape is non-square, rows!=cols, this
  // refers to the smaller of the two, so the other might be as large as
  // 1021 (can't be 1022 because following the above 'rectangularness'
  // subdivision, the aspect ratio is already < 2).

  // We are going to try candidate values for block_size_log2 ranging from
  // kernel_size_log2 to (kernel_size_log2 + kMaxKernelsPerBlockLog2).
  // For each of them we will compute a 'score' by adding individual scores
  // for a few different considerations, all of which is entirely empirical.
  // The values (and possibly the logic) around here are all subject to tuning
  // based on benchmarks on different hardware. The current values are based
  // on benchmarking on Qualcomm S855 (big and little cores), arm64,
  // kNeonDotprod, 8bit quantized path. Don't read too much into it, go ahead
  // and tune this as needed to achieve good performance elsewhere. Use
  // the unit test, block_map_test, to encode values that should be preserved
  // on specific architectures. Use RUY_TRACE to debug the current heuristics
  // and RUY_MAKEBLOCKMAP_EXPLICIT_BLOCK_SIZE_LOG2 to test the impact of a
  // different block_size_log2 choice, to empirically find the optimal value
  // before getting to updating the heuristic so that it produces that value.
  static constexpr int kMaxKernelsPerBlockLog2 = 6;
  const int max_block_size_log2 =
      std::min(size_log2, kernel_size_log2 + kMaxKernelsPerBlockLog2);
  int best_score = std::numeric_limits<int>::min();
  int best_score_block_size_log2 = -1;
  RUY_TRACE_INFO(MAKE_BLOCK_MAP_START);
  for (int block_size_log2 = kernel_size_log2;
       block_size_log2 <= max_block_size_log2; block_size_log2++) {
    const int multithreading_score = GetMultithreadingScore(
        block_size_log2, rows, cols, tentative_thread_count);
    const int cache_locality_score = GetCacheLocalityScore(
        block_size_log2, rows, cols, depth, kernel_rows_log2, kernel_cols_log2,
        lhs_scalar_size, rhs_scalar_size, cpu_cache_params);
    const int kernel_amortization_score = GetKernelAmortizationScore(
        block_size_log2, rows, cols, kernel_rows_log2, kernel_cols_log2);
    const int score =
        multithreading_score + cache_locality_score + kernel_amortization_score;
    if (score >= best_score) {
      best_score = score;
      best_score_block_size_log2 = block_size_log2;
    }
    RUY_TRACE_INFO(MAKE_BLOCK_MAP_EACH_TENTATIVE_BLOCK_SIZE);
  }

#ifdef RUY_MAKEBLOCKMAP_EXPLICIT_BLOCK_SIZE_LOG2
  // Useful for tuning.
  best_score_block_size_log2 = RUY_MAKEBLOCKMAP_EXPLICIT_BLOCK_SIZE_LOG2;
#endif

  // As explained in the above comment, phrasing the above code in terms of
  // block_size_log2 was only convenience inside of that heuristic. Now we
  // revert to talking in terms of grid subdivision. That is what will actually
  // be powers of two.
  int num_blocks_base_log2 = size_log2 - best_score_block_size_log2;
  RUY_DCHECK_GE(num_blocks_base_log2, 0);
  const int num_blocks_of_rows_log2 =
      num_blocks_base_log2 + rows_rectangularness_log2;
  const int num_blocks_of_cols_log2 =
      num_blocks_base_log2 + cols_rectangularness_log2;

  // Now that we know the grid subdivision, we can pinpoint the exact block
  // sizes. They can't be powers of two in general; they can't even be all
  // equal in general; so the following few parameters will govern how blocks
  // of slightly different shapes are put together in the block map.
  const int small_block_rows =
      round_down_pot(rows >> num_blocks_of_rows_log2, kernel_rows);
  const int small_block_cols =
      round_down_pot(cols >> num_blocks_of_cols_log2, kernel_cols);
  const int rows_of_large_blocks =
      round_up_pot(rows - (small_block_rows << num_blocks_of_rows_log2),
                   kernel_rows) >>
      pot_log2(kernel_rows);
  const int cols_of_large_blocks =
      round_up_pot(cols - (small_block_cols << num_blocks_of_cols_log2),
                   kernel_cols) >>
      pot_log2(kernel_cols);

  // We have everything! Write out to the destination block_map.
  block_map->dims[Side::kLhs] = rows;
  block_map->dims[Side::kRhs] = cols;
  block_map->kernel_dims[Side::kLhs] = kernel_rows;
  block_map->kernel_dims[Side::kRhs] = kernel_cols;
  block_map->num_blocks_base_log2 = num_blocks_base_log2;
  block_map->rectangularness_log2[Side::kLhs] = rows_rectangularness_log2;
  block_map->rectangularness_log2[Side::kRhs] = cols_rectangularness_log2;
  block_map->small_block_dims[Side::kLhs] = small_block_rows;
  block_map->small_block_dims[Side::kRhs] = small_block_cols;
  block_map->large_blocks[Side::kLhs] = rows_of_large_blocks;
  block_map->large_blocks[Side::kRhs] = cols_of_large_blocks;
  // See the comment on GetTraversalOrder for why we are dividing `rows` and
  // `cols` by the rectangularness subdivision parameters here.
  block_map->traversal_order = GetTraversalOrder(
      rows >> rows_rectangularness_log2, cols >> cols_rectangularness_log2,
      depth, lhs_scalar_size, rhs_scalar_size, cpu_cache_params);
  // Done last: NumBlocks needs some of the block_map fields to be already set.
  block_map->thread_count =
      std::min(tentative_thread_count, NumBlocks(*block_map));
  RUY_TRACE_INFO(MAKE_BLOCK_MAP_END);
}

void GetBlockMatrixCoords(Side side, const BlockMap& block_map, int block,
                          int* start, int* end) {
  profiler::ScopeLabel label("GetBlockMatrixCoords");
  *start = block * block_map.small_block_dims[side] +
           std::min(block, block_map.large_blocks[side]) *
               block_map.kernel_dims[side];
  *end =
      *start + block_map.small_block_dims[side] +
      (block < block_map.large_blocks[side] ? block_map.kernel_dims[side] : 0);

  RUY_DCHECK_EQ(0, *start % block_map.kernel_dims[side]);
  RUY_DCHECK_EQ(0, *end % block_map.kernel_dims[side]);
  RUY_DCHECK_LE(*end, block_map.dims[side]);
  RUY_DCHECK_LT(*start, *end);
  RUY_DCHECK_GE(*start, 0);
}

void GetBlockMatrixCoords(const BlockMap& block_map, const SidePair<int>& block,
                          SidePair<int>* start, SidePair<int>* end) {
  for (Side side : {Side::kLhs, Side::kRhs}) {
    GetBlockMatrixCoords(side, block_map, block[side], &(*start)[side],
                         &(*end)[side]);
  }
}

}  // namespace ruy
