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

// Internal types and helpers for matrices.
// "Mat" is the name we use to refer to our internal matrix classes; it can be
// thought of as a shorter version of "InternalMatrix"`
//
// Ruy has four internal matrix classes, besides the
// Matrix<T> class that we expose to the user-facing API.
//
// TODO(silvasean): Put parts of this architecture description somewhere more
// prominent.
//
// The 4 internal matrix classes are named Mat, EMat, PMat, PEMat, where:
// - "E" indicates a type-erased class, storing a void* pointer and a runtime
//   enum value to track the scalar type, as opposed to being templatized
//   on a Scalar type and storing a Scalar* pointer.
// - "P" indicates a packed matrix class, the output of the packing code and
//   input of the kernel code. See comments in pack.h regarding packing.
//
// In other words:
//
//                Plain matrices   Packed matrices
//             +----------------------------------
// Templated   |  Mat, Matrix      PMat
// Type-erased |  EMat             PEMat
//
// Note that Matrix<T> is *not* implemented in terms of the internal types. It
// is an independent, simple, and user-facing type. Matrix<T> is functionally
// equivalent to Mat, but we keep it separate to insulate internals from
// interface and to be able to make different compromises in internals
// vs interface: in internals we prefer Mat to be a C-style struct with
// raw data member access and to be similar to the other PMat/EMat/PEMat
// classes for consistency.
//
// The use of type-erasure might seem surprising for a library like Ruy with a
// heavily-templated entry point, but it is motivated by the desire for most of
// Ruy's "middle-end" to be non-templated. Ruy can be thought of as having 3
// main parts:
// - "entry-point" (ruy.h) - this is the highly templated ruy::Mul entry
// point.
// - "front-end" (frontend.*, validate.*, create_trmul_params.*,
// prepare_packed_matrices.*) - the work to handle the entry-point call down to
// the point where it can be handed off to the middle/back ends below. That
// includes routines that select RunKernel and RunPack
// implementations statically based on those template parameters.
// - "back-end" (kernel_*.*, pack_*.*)- this consists of the implementations of
// RunKernel and RunPack, often in assembly code, which are the building blocks
// that Ruy calls to perform matrix multiplication.  These are templated so that
// only the requested types/Path's are actually emitted by the compiler.
// - "middle-end" (trmul.*) - this is the part of Ruy that orchestrates the
// calls to the "back-end" optimized building blocks. This layer has to deal
// with issues like cache locality and low-overhead multi-threading.
//
// There is a desire for the "middle-end" to be non-templated in order to
// simplify the implementation and reduce code-size. We type-erase when going
// from the "front-end" to the "middle-end", and un-type-erase going from the
// "middle-end" to the "back-end". The un-type-erasure is possible because the
// "front-end" is responsible for instantiating the needed "back-end" templates,
// and thus the static type information is still present.
//
// Each layer of Ruy uses matrix types:
// - "entry-point": Matrix<T>
// - "front-end": Mat
// - "middle-end": EMat, PEMat
// - "back-end": Mat, PMat
//
// The use of separate types for packed matrices is not essential, but makes it
// obvious at a glance whether a matrix is a packed matrix or not. We would
// reconsider this decision if there was significant duplication between packed
// and unpacked matrices, but that doesn't seem to be the case at the moment.
//
// Another goal is to keep the user-facing Matrix<T> as simple and
// understandable as possible. Ideally, a user should be able to read the struct
// definition for Matrix<T> and see a very simple definition with no internal
// details like sums and kernel block layout.

#ifndef RUY_RUY_INTERNAL_MATRIX_H_
#define RUY_RUY_INTERNAL_MATRIX_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "ruy/check_macros.h"
#include "ruy/matrix.h"
#include "ruy/size_util.h"

namespace ruy {

// Internal counterpart of Layout, used by Mat.
struct MatLayout final {
  std::int32_t rows = 0;
  std::int32_t cols = 0;
  // Stride is the offset between two adjacent matrix elements
  // in the non-contiguous direction.
  std::int32_t stride = 0;
  Order order = Order::kColMajor;
};

inline MatLayout ToInternal(const Layout& src) {
  MatLayout ret;
  ret.rows = src.rows();
  ret.cols = src.cols();
  ret.stride = src.stride();
  ret.order = src.order();
  return ret;
}

// Internal counterpart of Matrix
template <typename Scalar>
struct Mat final {
  detail::ConstCheckingPtr<Scalar> data;
  MatLayout layout;
  Scalar zero_point = 0;
  CachePolicy cache_policy = CachePolicy::kNeverCache;
};

template <typename Scalar>
inline Mat<Scalar> ToInternal(const Matrix<Scalar>& src) {
  Mat<Scalar> ret;
  ret.data.set(src.data());
  ret.layout = ToInternal(src.layout());
  ret.zero_point = src.zero_point();
  ret.cache_policy = src.cache_policy();
  return ret;
}

template <typename Scalar>
inline Mat<Scalar> ToInternal(Matrix<Scalar>& src) {
  Mat<Scalar> ret;
  ret.data.set(src.data());
  ret.layout = ToInternal(src.layout());
  ret.zero_point = src.zero_point();
  ret.cache_policy = src.cache_policy();
  return ret;
}

// KernelLayout describes small-scale block structure in a packed matrix layout.
// It's a runtime (as opposed to compile-time-constant) version of the
// FixedKernelLayout struct used to declare kernel layouts.
//
// This is is sometimes known as "tiling" in other contexts.
//
// For example, consider a packed matrix in column-major format with a
// column-major KernelLayout. The matrix logically has a shape of
// `[cols, rows]`. However, the matrix is laid out as though it were a 4D array
// of shape `[cols / kcols, rows / krows, kcols, krows]`.
//
// Note that in the case of kcols=1, krows=1, this degenerates to
// `[cols, rows, 1, 1]` which is equivalent to having no small-scale block
// structure.
struct KernelLayout final {
  Order order = Order::kColMajor;
  std::uint8_t rows = 1;
  std::uint8_t cols = 1;
};

// A packed matrix has a small-scale block structure that is not present in in
// the input matrices. This block structure is necessary for the kernels to
// process data efficiently.
//
// This struct is very similar to MatLayout, but has the extra KernelLayout
// field.
struct PMatLayout final {
  std::int32_t rows = 0;
  std::int32_t cols = 0;
  // Stride is the offset between two adjacent matrix elements
  // in the non-contiguous direction.
  std::int32_t stride = 0;
  Order order = Order::kColMajor;
  // Small scale layout shuffling, potentially departing from
  // linear row-major or column-major storage. See KernelLayout.
  KernelLayout kernel;
};

inline bool operator==(const PMatLayout& a, const PMatLayout& b) {
  return a.cols == b.cols && a.rows == b.rows && a.stride == b.stride &&
         a.order == b.order && a.kernel.rows == b.kernel.rows &&
         a.kernel.cols == b.kernel.cols && a.kernel.order == b.kernel.order;
}

// Dynamic representation for a type.
//
// The most important field in this struct is the size, which Ruy uses to know
// how much memory to allocate without having to be templated on a type.
// Signed-ness and floating-point-ness are mainly present as debugging checks.
//
// Note: Ruy does not use this struct to to dynamically dispatch between
// different typed implementations. As described in the comment at the top of
// this file, Ruy's "front-end", which is templated, instantiates all the
// necessary "back-end" routines with complete static knowledge of all the
// types.
struct Type final {
  template <typename T>
  static Type Create() {
    Type ret;
    ret.is_signed = std::is_signed<T>::value;
    ret.is_floating_point = std::is_floating_point<T>::value;
    ret.size = sizeof(T);
    return ret;
  }

  template <typename T>
  void AssertIs() const {
    RUY_DCHECK_EQ(is_signed, Create<T>().is_signed);
    RUY_DCHECK_EQ(is_floating_point, Create<T>().is_floating_point);
    RUY_DCHECK_EQ(size, Create<T>().size);
  }

  bool is_signed = false;
  bool is_floating_point = false;
  std::uint8_t size = 0;
};

inline bool operator==(const Type& type1, const Type& type2) {
  return type1.is_signed == type2.is_signed &&
         type1.is_floating_point == type2.is_floating_point &&
         type1.size == type2.size;
}

// Type-erased matrix.
struct EMat final {
  Type data_type;
  void* data = nullptr;
  MatLayout layout;
  std::int32_t zero_point = 0;
  CachePolicy cache_policy = CachePolicy::kNeverCache;
};

// Type-erased packed matrix.
struct PEMat final {
  Type data_type;
  void* data = nullptr;
  Type sums_type;
  void* sums = nullptr;
  PMatLayout layout;
  std::int32_t zero_point = 0;
};

// Convenient typed helper for packed matrices.
template <typename Scalar>
struct PMat final {
  // The row/column sums needed for quantized matrix multiplication when
  // the opposite operand of the multiplication uses a non-symmetric zero
  // point.
  // This member is only relevant for packed matrices.
  // Additionally, Ruy always uses 32-bit signed accumulators for quantized
  // matrix multiplication.
  // For floating point types, there is no quantization, so this pointer
  // will always be null. We still need code referencing it to compile
  // though, even if it is always branched around. Hence we use Scalar*
  // itself as the type in that case.
  using SumsType =
      typename std::conditional<std::is_floating_point<Scalar>::value, Scalar,
                                std::int32_t>::type;

  Scalar* data = nullptr;
  SumsType* sums = nullptr;
  PMatLayout layout;
  std::int32_t zero_point = 0;
};

template <typename T>
EMat EraseType(const Mat<T>& matrix) {
  EMat ret;
  ret.data_type = Type::Create<T>();
  ret.data = const_cast<void*>(static_cast<const void*>(matrix.data.get()));
  ret.layout = matrix.layout;
  ret.zero_point = matrix.zero_point;
  ret.cache_policy = matrix.cache_policy;
  return ret;
}

template <typename T>
Mat<T> UneraseType(const EMat& matrix) {
  matrix.data_type.AssertIs<T>();
  Mat<T> ret;
  ret.data.set(static_cast<T*>(matrix.data));
  ret.layout = matrix.layout;
  ret.zero_point = matrix.zero_point;
  ret.cache_policy = matrix.cache_policy;
  return ret;
}

template <typename T>
PMat<T> UneraseType(const PEMat& matrix) {
  using SumsType = typename PMat<T>::SumsType;
  matrix.data_type.AssertIs<T>();
  matrix.sums_type.AssertIs<SumsType>();
  PMat<T> ret;
  ret.data = static_cast<T*>(matrix.data);
  ret.sums = static_cast<SumsType*>(matrix.sums);
  ret.layout = matrix.layout;
  ret.zero_point = matrix.zero_point;
  return ret;
}

// Helpers for MatLayout / PMatLayout.

inline bool IsUnstrided(const MatLayout& layout) {
  if (layout.order == Order::kColMajor) {
    return layout.stride == layout.rows;
  } else {
    return layout.stride == layout.cols;
  }
}

inline bool IsRowMajor(const MatLayout& layout) {
  return layout.order == Order::kRowMajor;
}

inline bool IsColMajor(const MatLayout& layout) {
  return layout.order == Order::kColMajor;
}

inline std::ptrdiff_t FlatSize(const MatLayout& layout) {
  const int outerdim =
      layout.order == Order::kColMajor ? layout.cols : layout.rows;
  return layout.stride * outerdim;
}

inline bool IsUnstrided(const PMatLayout& layout) {
  if (layout.order == Order::kColMajor) {
    return layout.stride == layout.rows;
  } else {
    return layout.stride == layout.cols;
  }
}

inline bool IsRowMajor(const PMatLayout& layout) {
  return layout.order == Order::kRowMajor;
}

inline bool IsColMajor(const PMatLayout& layout) {
  return layout.order == Order::kColMajor;
}

inline std::ptrdiff_t FlatSize(const PMatLayout& layout) {
  const int outerdim =
      layout.order == Order::kColMajor ? layout.cols : layout.rows;
  return layout.stride * outerdim;
}

// TODO(b/130417400) add a unit test
inline int Offset(const MatLayout& layout, int row, int col) {
  // TODO(benoitjacob)  - should check this but this make the _slow tests take
  // 5x longer.  Find a mitigation like in Eigen with an 'internal' variant
  // bypassing the check?
  // RUY_DCHECK_GE(row, 0);
  // RUY_DCHECK_GE(col, 0);
  // RUY_DCHECK_LT(row, layout.rows);
  // RUY_DCHECK_LT(col, layout.cols);
  int row_stride = layout.order == Order::kColMajor ? 1 : layout.stride;
  int col_stride = layout.order == Order::kRowMajor ? 1 : layout.stride;
  return row * row_stride + col * col_stride;
}

// TODO(b/130417400) add a unit test
inline int Offset(const PMatLayout& layout, int row, int col) {
  RUY_DCHECK(is_pot(layout.kernel.rows));
  RUY_DCHECK(is_pot(layout.kernel.cols));
  int row_outer = row & ~(layout.kernel.rows - 1);
  int col_outer = col & ~(layout.kernel.cols - 1);
  int row_stride_outer =
      layout.order == Order::kColMajor ? layout.kernel.cols : layout.stride;
  int col_stride_outer =
      layout.order == Order::kRowMajor ? layout.kernel.rows : layout.stride;
  int offset_outer =
      row_outer * row_stride_outer + col_outer * col_stride_outer;
  int row_inner = row - row_outer;
  int col_inner = col - col_outer;
  int row_stride_inner =
      layout.kernel.order == Order::kColMajor ? 1 : layout.kernel.cols;
  int col_stride_inner =
      layout.kernel.order == Order::kRowMajor ? 1 : layout.kernel.rows;
  int offset_inner =
      row_inner * row_stride_inner + col_inner * col_stride_inner;
  return offset_outer + offset_inner;
}

// Helpers for Mat<T>.

template <typename Scalar>
const Scalar* ElementPtr(const Mat<Scalar>& mat, int row, int col) {
  return mat.data.get() + Offset(mat.layout, row, col);
}

template <typename Scalar>
Scalar* ElementPtr(Mat<Scalar>* mat, int row, int col) {
  return mat->data.get() + Offset(mat->layout, row, col);
}

template <typename Scalar>
Scalar Element(const Mat<Scalar>& mat, int row, int col) {
  return *ElementPtr(mat, row, col);
}

// Helpers for PMat<T>.
// Duplicated from Matrix<T>, but the duplication seems acceptable.

template <typename Scalar>
const Scalar* ElementPtr(const PMat<Scalar>& mat, int row, int col) {
  return mat.data + Offset(mat.layout, row, col);
}

template <typename Scalar>
Scalar* ElementPtr(PMat<Scalar>* mat, int row, int col) {
  return mat->data + Offset(mat->layout, row, col);
}

template <typename Scalar>
Scalar Element(const PMat<Scalar>& mat, int row, int col) {
  return *ElementPtr(mat, row, col);
}

// Helpers for PEMat.

inline std::ptrdiff_t DataBytes(const PEMat& packed) {
  return FlatSize(packed.layout) * packed.data_type.size;
}

inline std::ptrdiff_t SumsBytes(const PEMat& packed) {
  // Packed matrices are only relevant for Ruy's TrMul implementations. For
  // TrMul, the number of sums is always equal to the number of columns.
  return packed.layout.cols * packed.sums_type.size;
}

// Transpose helpers.

inline Order Transpose(Order order) {
  return order == Order::kColMajor ? Order::kRowMajor : Order::kColMajor;
}

inline MatLayout Transpose(const MatLayout& layout) {
  MatLayout result(layout);
  result.order = Transpose(result.order);
  std::swap(result.rows, result.cols);
  return result;
}

template <typename Scalar>
Mat<Scalar> Transpose(const Mat<Scalar>& matrix) {
  Mat<Scalar> result(matrix);
  result.layout = Transpose(result.layout);
  return result;
}

// Compile-time version of KernelLayout, used to declare kernel layouts in a
// way that can be consumed by compile-time logic.
template <Order tOrder, int tRows, int tCols>
struct FixedKernelLayout {
  static constexpr Order kOrder = tOrder;
  static constexpr int kRows = tRows;
  static constexpr int kCols = tCols;
};

template <typename FixedKernelLayout>
KernelLayout ToKernelLayout() {
  KernelLayout ret;
  ret.order = FixedKernelLayout::kOrder;
  ret.rows = FixedKernelLayout::kRows;
  ret.cols = FixedKernelLayout::kCols;
  return ret;
}

#if (__cplusplus < 201703L)
// A static constexpr data member is automatically inline and should not require
// redeclaration without an initializer. This is actually deprecated from C++17
// onwards. Clang with -O0 without this can fail to link.
template <Order tOrder, int tRows, int tCols>
constexpr int FixedKernelLayout<tOrder, tRows, tCols>::kCols;
template <Order tOrder, int tRows, int tCols>
constexpr int FixedKernelLayout<tOrder, tRows, tCols>::kRows;
#endif

}  // namespace ruy

#endif  // RUY_RUY_INTERNAL_MATRIX_H_
