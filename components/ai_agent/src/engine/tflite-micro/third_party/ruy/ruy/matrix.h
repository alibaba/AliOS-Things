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

#ifndef RUY_RUY_MATRIX_H_
#define RUY_RUY_MATRIX_H_

#include <cstddef>
#include <cstdint>  // IWYU pragma: keep
#include <type_traits>

#include "ruy/check_macros.h"

namespace ruy {

// Layout storage order. Here and elsewhere, 'col' is short for 'column'.
// 'column-major' means that each column is contiguous in memory.
enum class Order : std::uint8_t { kColMajor, kRowMajor };

// Describes the shape and storage layout of a matrix.
class Layout final {
 public:
  int rows() const { return rows_; }
  void set_rows(int val) { rows_ = val; }
  int cols() const { return cols_; }
  void set_cols(int val) { cols_ = val; }
  int stride() const { return stride_; }
  void set_stride(int val) { stride_ = val; }
  Order order() const { return order_; }
  void set_order(Order val) { order_ = val; }

 private:
  int rows_ = 0;
  int cols_ = 0;
  // Stride is the offset between two adjacent matrix elements
  // in the non-contiguous direction.
  int stride_ = 0;
  Order order_ = Order::kColMajor;
};

namespace detail {

// Thin wrapper around a pointer with a constness model that works for the
// purposes of the Matrix class.
//
// A typical conundrum of any C++ container class is what type constness should
// encode at compile time constancy of the contained data?
// `Matrix<const T>` or `const Matrix<T>`?
// With either approach it is very difficult to achieve perfect
// const-correctness that that can only be done with some combination of
// inconvenient interface and c++ complexity/abstraction.
//
// Here we opt for something that's entirely tailored to the needs of the Ruy
// interface. The only purpose of the Matrix class is to pass matrix data
// pointers to ruy. There is an asymmetry here: the caller of ruy::Mul only
// needs to `set` the data; ruy itself only needs to `get` the data. In the
// caller's code, it's convenient to be able to just deal with `Matrix<T>`
// without having to sprinkle `const` keywords in the right places, so we want
// to track whether the data is constant in a way that's decoupled from the
// constness of `this`, and we never want to have Matrix<const T>. Inside ruy
// code, as input matrices are passed by const-reference and output matrices are
// passed by pointer (to non-const), the constness of `this` is telling whether
// the data is constant. See the `get` and `set` methods below and the comment
// explaining the core logic that they encapsulate.
template <typename T>
class ConstCheckingPtr final {
 public:
  using element_type = T;

  // Core accessors. These encapsulate the main logic:
  // - for `set`, the constness of the argument determines whether internal
  // pointer should be tracked as const/mutable.
  // - for `get`, the constness of `this` determines whether the call
  // counts as a const or mutable use of the internal pointer.
  void set(T* ptr) {
    ptr_ = ptr;
    set_mutable(true);
  }
  void set(const T* ptr) {
    ptr_ = ptr;
    set_mutable(false);
  }
  void set(std::nullptr_t) { ptr_ = nullptr; }
  T* get() /* NOT const */ {
    assert_mutable();
    return const_cast<T*>(ptr_);
  }
  const T* get() const { return ptr_; }

 private:
  // There's never a need for Matrix<const T>.
  static_assert(!std::is_const<T>::value, "");
  const T* ptr_ = nullptr;
#ifndef NDEBUG
  bool is_mutable_ = true;
  void set_mutable(bool val) { is_mutable_ = val; }
  void assert_mutable() { RUY_DCHECK(is_mutable_); }
#else
  void set_mutable(bool) {}
  void assert_mutable() {}
#endif
};

}  // namespace detail

enum class CachePolicy : std::uint8_t {
  kNeverCache,
  kCacheIfLargeSpeedup,
  kCacheIfSignificantSpeedup,
  kAlwaysCache,
};

// A Matrix merely wraps existing data as a matrix. It doesn't own any buffer.
// The purpose of Matrix is only to be used in ruy's interface -- it's just
// a structured way for the user to pass to ruy::Mul the matrix data pointers
// together with other matrix parameters.
// Scalar may be any floating-point or integral type. When integral, it may be
// signed or unsigned. It's never const: use Matrix<T> for both input and output
// matrices, never use Matrix<const T>.
// See the comments on detail::ConstCheckingPointer.
template <typename Scalar>
class Matrix final {
 public:
  static_assert(!std::is_const<Scalar>::value,
                "Never use Matrix<const T>. Just use Matrix<T>. Constness of "
                "the data is guarded by debug-only runtime assertions. See "
                "detail::ConstCheckingPtr.");

  Scalar* data() { return data_.get(); }
  const Scalar* data() const { return data_.get(); }
  void set_data(Scalar* ptr) { data_.set(ptr); }
  void set_data(const Scalar* ptr) { data_.set(ptr); }
  void set_data(std::nullptr_t) { data_.set(nullptr); }
  const Layout& layout() const { return layout_; }
  Layout* mutable_layout() { return &layout_; }
  Scalar zero_point() const { return zero_point_; }
  void set_zero_point(Scalar value) { zero_point_ = value; }
  CachePolicy cache_policy() const { return cache_policy_; }
  void set_cache_policy(CachePolicy value) { cache_policy_ = value; }

 private:
  // The underlying buffer wrapped by this matrix.
  detail::ConstCheckingPtr<Scalar> data_;
  // The shape and data layout of this matrix.
  Layout layout_;
  // The zero_point, i.e. which Scalar value is to be interpreted as zero.
  // When Scalar is floating-point, this must be 0.
  Scalar zero_point_ = 0;
  // When the data pointed to by this matrix is constant data, so that it is
  // valid to assume that equality of pointers implies equality of data,
  // a CachePolicy may be used instead of the default kNeverCache,
  // which will enable ruy to take advantage of this constancy of the data to
  // cache the packing work, which can be a large speedup in matrix*vector
  // and other narrow shapes.
  CachePolicy cache_policy_ = CachePolicy::kNeverCache;
};

inline void MakeSimpleLayout(int rows, int cols, Order order, Layout* layout) {
  layout->set_rows(rows);
  layout->set_cols(cols);
  layout->set_order(order);
  layout->set_stride(order == Order::kColMajor ? rows : cols);
}

template <typename StreamType, typename Scalar>
StreamType& operator<<(StreamType& stream, const Matrix<Scalar>& mat) {
  for (int row = 0; row < mat.layout().rows(); row++) {
    for (int col = 0; col < mat.layout().cols(); col++) {
      stream << static_cast<double>(Element(mat, row, col)) << " ";
    }
    stream << "\n";
  }
  return stream;
}

// TODO(b/130417400) add a unit test
inline int Offset(const Layout& layout, int row, int col) {
  // TODO(benoitjacob)  - should check this but this make the _slow tests take
  // 5x longer.  Find a mitigation like in Eigen with an 'internal' variant
  // bypassing the check?
  // RUY_DCHECK_GE(row, 0);
  // RUY_DCHECK_GE(col, 0);
  // RUY_DCHECK_LT(row, layout.rows());
  // RUY_DCHECK_LT(col, layout.cols());
  int row_stride = layout.order() == Order::kColMajor ? 1 : layout.stride();
  int col_stride = layout.order() == Order::kRowMajor ? 1 : layout.stride();
  return row * row_stride + col * col_stride;
}

template <typename Scalar>
const Scalar* ElementPtr(const Matrix<Scalar>& mat, int row, int col) {
  return mat.data() + Offset(mat.layout(), row, col);
}

template <typename Scalar>
Scalar* ElementPtr(Matrix<Scalar>* mat, int row, int col) {
  return mat->data() + Offset(mat->layout(), row, col);
}

template <typename Scalar>
Scalar Element(const Matrix<Scalar>& mat, int row, int col) {
  return *ElementPtr(mat, row, col);
}

}  // namespace ruy

#endif  // RUY_RUY_MATRIX_H_
