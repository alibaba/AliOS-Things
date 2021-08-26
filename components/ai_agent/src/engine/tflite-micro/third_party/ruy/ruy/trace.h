/* Copyright 2021 Google LLC. All Rights Reserved.

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

#ifndef RUY_RUY_TRACE_H_
#define RUY_RUY_TRACE_H_

#ifdef RUY_TRACE

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "ruy/mat.h"
#include "ruy/matrix.h"
#include "ruy/path.h"
#include "ruy/platform.h"
#include "ruy/side_pair.h"

namespace ruy {

// Helper for `formatted` so we don't have to put .c_str() on strings.
template <typename T>
T value_for_snprintf(T value) {
  return value;
}

inline const char* value_for_snprintf(const std::string& s) {
  return s.c_str();
}

// A sprintf-like function returning a std::string.
// Remove this once we can rely on std::format (c++20).
template <typename... Args>
std::string formatted(const char* format, Args... args) {
  char buf[1024];
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Wformat-security"
  int size = snprintf(buf, sizeof buf, format, value_for_snprintf(args)...);
#pragma GCC diagnostic pop
  if (size <= 0) {
    abort();
  }
  return std::string(buf);
}

// An entry in the trace.
struct ThreadTraceEntry final {
  std::string text;
  int indent = 0;
  const char* source_file = nullptr;
  int source_line = 0;
};

// Trace for one thread.
class ThreadTrace final {
 public:
  ~ThreadTrace() {}

  void set_thread_id(int thread_id) { thread_id_ = thread_id; }
  int thread_id() const { return thread_id_; }

  bool is_in_run_ahead_packing_loop() const {
    return is_in_run_ahead_packing_loop_;
  }
  void set_is_in_run_ahead_packing_loop(bool value) {
    is_in_run_ahead_packing_loop_ = value;
  }

  void set_current_source_file(const char* source_file) {
    current_source_file_ = source_file;
  }

  void set_current_source_line(int source_line) {
    current_source_line_ = source_line;
  }

  const std::vector<ThreadTraceEntry>& entries() const { return entries_; }

  template <typename... Args>
  void Write(const char* format, Args... args) {
    ThreadTraceEntry entry;
    entry.text = formatted(format, args...);
    entry.indent = indent_;
    entry.source_file = current_source_file_;
    entry.source_line = current_source_line_;
    entries_.emplace_back(std::move(entry));
  }

  template <typename... Args>
  void EnterScope(const char* scope_name) {
    Write("%s {", scope_name);
    indent_++;
  }
  void LeaveScope(const char* scope_name) {
    indent_--;
    Write("}  // end of %s", scope_name);
  }

 private:
  // The trace contents
  std::vector<ThreadTraceEntry> entries_;

  // Current indentation level.
  int indent_ = 0;
  // Thread's ID as set by Ruy, e.g. [0,N-1]. Not OS TID.
  int thread_id_ = -1;
  // The run-ahead loop in `EnsurePacked` may run many iterations when the
  // thread is waiting for a block to be packed by another thread --- it's
  // a busy wait. We track whether we are in that mode to avoid generating
  // many uninteresting trace entries.
  bool is_in_run_ahead_packing_loop_ = false;
  // Last recorded value of __FILE__ and __LINE__, as a convenience so we don't
  // have to pass these in every call to `Write`.
  const char* current_source_file_ = nullptr;
  int current_source_line_ = 0;
};

// Main components of ruy. Used for trace colorization.
enum class Component { kNone, kFrontEnd, kMiddleEnd, kBackEnd, kThreadPool };

// Output format for the trace.
enum class TraceOutputFormat { kNone, kTerminal, kHtml };

inline std::string IndentString(int indent) {
  std::string s;
  for (int i = 0; i < indent; i++) {
    s += "  ";
  }
  return s;
}

// Returns the text to write to the trace to open a colored section.
inline const char* ColorSectionStart(TraceOutputFormat output_format,
                                     Component component) {
  switch (output_format) {
    case TraceOutputFormat::kTerminal:
      switch (component) {
        case Component::kFrontEnd:
          return "\x1b[36m";
        case Component::kMiddleEnd:
          return "\x1b[32m";
        case Component::kBackEnd:
          return "\x1b[31m";
        case Component::kThreadPool:
          return "\x1b[33m";
        default:
          abort();
          return nullptr;
      }
    case TraceOutputFormat::kHtml:
      switch (component) {
        case Component::kFrontEnd:
          return "<span style=\"background-color:#B2EBF2\">";
        case Component::kMiddleEnd:
          return "<span style=\"background-color:#C8E6C9\">";
        case Component::kBackEnd:
          return "<span style=\"background-color:#FFCDD2\">";
        case Component::kThreadPool:
          return "<span style=\"background-color:#FFF9C4\">";
        default:
          abort();
          return nullptr;
      }
    default:
      abort();
      return nullptr;
  }
}

// Returns the text to write to the trace to close a colored section.
inline const char* ColorSectionEnd(TraceOutputFormat output_format) {
  switch (output_format) {
    case TraceOutputFormat::kTerminal:
      return "\x1b[0m";
    case TraceOutputFormat::kHtml:
      return "</span>";
    default:
      abort();
      return nullptr;
  }
}

// Returns the output format to use for the trace.
inline TraceOutputFormat GetOutputFormat() {
  const char* html_env = getenv("RUY_TRACE_HTML");
  if (html_env && strtol(html_env, nullptr, 10) != 0) {
    return TraceOutputFormat::kHtml;
  } else {
    return TraceOutputFormat::kTerminal;
  }
}

// A `basename` function that's good enough for ruy __FILE__'s.
// Note: `basename` is POSIX-only and annoying (takes a char*, may mutate).
inline const char* GetBaseName(const char* path) {
  std::size_t len = strlen(path);
  if (len == 0) {
    return path;
  }
  const char* ptr = path + len - 1;
  while (ptr != path) {
    if (*ptr == '/' || *ptr == '\\') {
      return ptr + 1;
    }
    --ptr;
  }
  // Path did not contain any path separator.
  return path;
}

// Determines a Component (used for colorization) by source file.
inline Component GetComponentBySourceFile(const char* base_name) {
  if (!strcmp(base_name, "pack.h") || !strcmp(base_name, "kernel.h")) {
    return Component::kBackEnd;
  } else if (!strcmp(base_name, "trmul.cc") ||
             !strcmp(base_name, "block_map.cc")) {
    return Component::kMiddleEnd;
  } else if (!strcmp(base_name, "thread_pool.cc")) {
    return Component::kThreadPool;
  } else {
    return Component::kFrontEnd;
  }
}

inline std::string EscapeText(TraceOutputFormat output_format,
                              const std::string& text) {
  if (output_format == TraceOutputFormat::kHtml) {
    std::string escaped_text;
    for (char c : text) {
      if (c == '<') {
        escaped_text += "&lt;";
      } else if (c == '>') {
        escaped_text += "&gt;";
      } else {
        escaped_text += c;
      }
    }
    return escaped_text;
  } else {
    return text;
  }
}

// Prints an entry from the trace to the destination trace file.
inline void Print(const ThreadTraceEntry& entry,
                  TraceOutputFormat output_format, FILE* file) {
  const char* base_name = GetBaseName(entry.source_file);
  Component component = GetComponentBySourceFile(base_name);
  const std::string& source_location =
      formatted("%s:%d", base_name, entry.source_line);
  const std::string& escaped_text = EscapeText(output_format, entry.text);
  fprintf(file, "%s%-32s%s%s%s\n", ColorSectionStart(output_format, component),
          source_location.c_str(), IndentString(entry.indent).c_str(),
          escaped_text.c_str(), ColorSectionEnd(output_format));
}

// Prints a thread's entire trace to the destination trace file.
inline void Print(const ThreadTrace& trace, TraceOutputFormat output_format,
                  FILE* file) {
  if (output_format == TraceOutputFormat::kHtml) {
    fprintf(file, "<html><body><pre>\n<span style=\"font-weight:bold\">\n");
  }
  fprintf(file, "Ruy trace for thread %d:\n", trace.thread_id());
  if (output_format == TraceOutputFormat::kHtml) {
    fprintf(file, "</span>\n");
  }
  for (const ThreadTraceEntry& entry : trace.entries()) {
    Print(entry, output_format, file);
  }
  fprintf(file, "\n");
  if (output_format == TraceOutputFormat::kHtml) {
    fprintf(file, "</pre></body></html>\n");
  }
}

// Holds all the threads' traces. This is a global singleton class.
// On exit, when the singleton is destroyed, the destructor prints out the
// traces.
class AllThreadTraces final {
 public:
  // Add a new ThreadTrace for the current thread. Should be called only once
  // on each thread.
  ThreadTrace* AddCurrentThread() {
    std::lock_guard<std::mutex> lock(mutex_);
    ThreadTrace* thread_trace = new ThreadTrace;
    thread_traces_.emplace_back(thread_trace);
    return thread_trace;
  }
  ~AllThreadTraces() {
    std::lock_guard<std::mutex> lock(mutex_);
    // Open the destination file.
    const char* file_env = getenv("RUY_TRACE_FILE");
    FILE* file = stdout;
    if (file_env) {
      file = fopen(file_env, "w");
      if (!file) {
        fprintf(stderr, "Failed to open %s for write\n", file_env);
        exit(1);
      }
    }
    // Sort the threads by Ruy Thread ID (not OS TID).
    auto output_format = GetOutputFormat();
    std::sort(std::begin(thread_traces_), std::end(thread_traces_),
              [](const auto& a, const auto& b) {
                return a->thread_id() < b->thread_id();
              });
    // Print all the threads' traces.
    for (const auto& trace : thread_traces_) {
      Print(*trace, output_format, file);
    }
    if (file_env) {
      fclose(file);
    }
  }
  static AllThreadTraces* Singleton() {
    static AllThreadTraces all_thread_traces;
    return &all_thread_traces;
  }

 private:
  std::vector<std::unique_ptr<ThreadTrace>> thread_traces_;
  std::mutex mutex_;
};

// Returns the thread-local ThreadTrace singleton, constructing it as needed.
inline ThreadTrace* ThreadLocalTrace() {
  static thread_local ThreadTrace* thread_local_trace =
      AllThreadTraces::Singleton()->AddCurrentThread();
  return thread_local_trace;
}

// RAII helper to trace a scope, e.g. a function scope.
class RuyTraceScope {
  const char* source_file_;
  int source_line_;
  const char* scope_name_;

 public:
  RuyTraceScope(const char* source_file, int source_line,
                const char* scope_name)
      : source_file_(source_file),
        source_line_(source_line),
        scope_name_(scope_name) {
    ThreadLocalTrace()->set_current_source_file(source_file_);
    ThreadLocalTrace()->set_current_source_line(source_line_);
    ThreadLocalTrace()->EnterScope(scope_name_);
  }
  ~RuyTraceScope() {
    ThreadLocalTrace()->set_current_source_file(source_file_);
    ThreadLocalTrace()->set_current_source_line(source_line_);
    ThreadLocalTrace()->LeaveScope(scope_name_);
  }
};

#define RUY_TRACE_SCOPE_NAME_IMPL(file, line, name) \
  RuyTraceScope ruy_trace_scope##line(file, line, name)
#define RUY_TRACE_SCOPE_NAME(name) \
  RUY_TRACE_SCOPE_NAME_IMPL(__FILE__, __LINE__, name)
#define RUY_TRACE_SCOPE \
  RUY_TRACE_SCOPE_NAME_IMPL(__FILE__, __LINE__, __FUNCTION__)

// Helpers to trace Ruy objects.

inline std::string str(Order o) {
  return o == Order::kRowMajor ? "row-major" : "column-major";
}

inline std::string str(Side s) { return s == Side::kLhs ? "LHS" : "RHS"; }

inline std::string str(const Layout& layout) {
  std::string s =
      formatted("%dx%d, %s", layout.rows(), layout.cols(), str(layout.order()));
  int inner_size =
      layout.order() == Order::kRowMajor ? layout.cols() : layout.rows();
  if (inner_size != layout.stride()) {
    s += formatted(", stride=%d", layout.stride());
  } else {
    s += formatted(", unstrided");
  }
  return s;
}

inline std::string str(const MatLayout& layout) {
  std::string s =
      formatted("%dx%d, %s", layout.rows, layout.cols, str(layout.order));
  int inner_size = layout.order == Order::kRowMajor ? layout.cols : layout.rows;
  if (inner_size != layout.stride) {
    s += formatted(", stride=%d", layout.stride);
  } else {
    s += formatted(", unstrided");
  }
  return s;
}

inline std::string str(const PMatLayout& layout) {
  std::string s =
      formatted("%dx%d, %s", layout.rows, layout.cols, str(layout.order));
  int inner_size = layout.order == Order::kRowMajor ? layout.cols : layout.rows;
  if (inner_size != layout.stride) {
    s += formatted(", stride=%d", layout.stride);
  } else {
    s += formatted(", unstrided");
  }
  s += formatted(", kernel blocks: %dx%d %s", layout.kernel.rows,
                 layout.kernel.cols, str(layout.kernel.order));
  return s;
}

template <typename T>
std::string str() {
  return "<unknown type>";
}
#define RUY_IMPL_STR_TYPE_STD(T)     \
  template <>                        \
  inline std::string str<std::T>() { \
    return #T;                       \
  }
#define RUY_IMPL_STR_TYPE(T)    \
  template <>                   \
  inline std::string str<T>() { \
    return #T;                  \
  }

RUY_IMPL_STR_TYPE(float)
RUY_IMPL_STR_TYPE(double)
RUY_IMPL_STR_TYPE_STD(int8_t)
RUY_IMPL_STR_TYPE_STD(uint8_t)
RUY_IMPL_STR_TYPE_STD(int16_t)
RUY_IMPL_STR_TYPE_STD(uint16_t)
RUY_IMPL_STR_TYPE_STD(int32_t)
RUY_IMPL_STR_TYPE_STD(uint32_t)
RUY_IMPL_STR_TYPE_STD(int64_t)
RUY_IMPL_STR_TYPE_STD(uint64_t)

template <typename T>
std::string str(const Matrix<T>& matrix) {
  std::string s = formatted("Matrix<%s>, %s", str<T>(), str(matrix.layout()));
  if (matrix.zero_point()) {
    s += formatted(", zero_point=%d", static_cast<int>(matrix.zero_point()));
  }
  if (matrix.cache_policy() != CachePolicy::kNeverCache) {
    s +=
        formatted(", cache_policy=%d", static_cast<int>(matrix.cache_policy()));
  }
  return s;
}

inline std::string str(const Type& type) {
  char c;
  if (type.is_floating_point) {
    c = 'f';
  } else if (type.is_signed) {
    c = 'i';
  } else {
    c = 'u';
  }
  return formatted("%c%d", c, type.size * 8);
}

inline std::string str(const EMat& mat) {
  std::string s =
      formatted("EMat, data_type=%s, %s", str(mat.data_type), str(mat.layout));
  if (mat.zero_point) {
    s += formatted(", zero_point=%d", static_cast<int>(mat.zero_point));
  }
  if (mat.cache_policy != CachePolicy::kNeverCache) {
    s += formatted(", cache_policy=%d", static_cast<int>(mat.cache_policy));
  }
  return s;
}

inline std::string str(const PEMat& mat) {
  std::string s =
      formatted("PEMat, data_type=%s, %s", str(mat.data_type), str(mat.layout));
  if (mat.zero_point) {
    s += formatted(", zero_point=%d", static_cast<int>(mat.zero_point));
  }
  return s;
}

inline std::string str(Path paths) {
  bool first = true;
  std::string s;
  for (int bit = 0; bit < 16; bit++) {
    Path cur_path = static_cast<Path>(1 << bit);
    if ((paths & cur_path) != Path::kNone) {
      if (!first) {
        s += " | ";
      }
      first = false;
      switch (cur_path) {
        case Path::kNone:
          continue;
#define RUY_HANDLE_PATH(p) \
  case Path::p:            \
    s += #p;               \
    break;
          RUY_HANDLE_PATH(kStandardCpp)
          RUY_HANDLE_PATH(kInternalStandardCppVariant1)
          RUY_HANDLE_PATH(kInternalStandardCppVariant2)
          RUY_HANDLE_PATH(kInternalStandardCppVariant3)
#if RUY_PLATFORM_ARM
          RUY_HANDLE_PATH(kNeon)
          RUY_HANDLE_PATH(kNeonDotprod)
#endif  // RUY_PLATFORM_ARM
#if RUY_PLATFORM_X86
          RUY_HANDLE_PATH(kAvx)
          RUY_HANDLE_PATH(kAvx2Fma)
          RUY_HANDLE_PATH(kAvx512)
#endif  // RUY_PLATFORM_X86
#undef RUY_HANDLE_PATH
        default:
          fprintf(stderr, "Unhandled Path value 0x%x\n",
                  static_cast<int>(cur_path));
          abort();
      }
    }
  }
  return s;
}

// Implementation of RUY_TRACE_INFO(X) macros.

#define RUY_TRACE_INFO_MUL                                            \
  ThreadLocalTrace()->Write("CompiledPaths: %s", str(CompiledPaths)); \
  ThreadLocalTrace()->Write("LHS: %s", str(lhs));                     \
  ThreadLocalTrace()->Write("RHS: %s", str(rhs));                     \
  ThreadLocalTrace()->Write("Destination: %s", str(*dst));

#define RUY_TRACE_INFO_CREATE_TRMUL_PARAMS_TRANSPOSING                      \
  ThreadLocalTrace()->Write("Canonicalizing to column-major destination:"); \
  ThreadLocalTrace()->Write(                                                \
      "Swapping LHS<->RHS and flipping all storage orders.");

#define RUY_TRACE_INFO_CREATE_TRMUL_PARAMS_ASSUMING_COLMAJOR_DST         \
  ThreadLocalTrace()->Write("Runtime-selected path: %s", str(the_path)); \
  ThreadLocalTrace()->Write("LHS: %s", str(params->src[Side::kLhs]));    \
  ThreadLocalTrace()->Write("RHS: %s", str(params->src[Side::kRhs]));    \
  ThreadLocalTrace()->Write("Destination: %s", str(params->dst));

#define RUY_TRACE_INFO_POPULATE_TRMUL_PARAMS                                \
  ThreadLocalTrace()->Write(                                                \
      "Here we have this Path as a template parameter: %s", str(ThePath));  \
  ThreadLocalTrace()->Write("PackedLhsScalar: %s", str<PackedLhsScalar>()); \
  ThreadLocalTrace()->Write("PackedRhsScalar: %s", str<PackedRhsScalar>()); \
  ThreadLocalTrace()->Write("Kernel function pointer: %p",                  \
                            params->run_kernel);                            \
  ThreadLocalTrace()->Write("Kernel LHS block layout: %dx%d %s",            \
                            LhsKernelLayout::kRows, LhsKernelLayout::kCols, \
                            str(LhsKernelLayout::kOrder));                  \
  ThreadLocalTrace()->Write("Kernel RHS block layout: %dx%d %s",            \
                            RhsKernelLayout::kRows, RhsKernelLayout::kCols, \
                            str(RhsKernelLayout::kOrder));                  \
  ThreadLocalTrace()->Write("Created packed matrices:");                    \
  ThreadLocalTrace()->Write("Packed LHS matrix: %s",                        \
                            str(params->packed_matrix[Side::kLhs]));        \
  ThreadLocalTrace()->Write("Packed RHS matrix: %s",                        \
                            str(params->packed_matrix[Side::kRhs]));        \
  ThreadLocalTrace()->Write("LHS packing function pointer: %p",             \
                            params->run_pack[Side::kLhs]);                  \
  ThreadLocalTrace()->Write("RHS packing function pointer: %p",             \
                            params->run_pack[Side::kRhs]);

#define RUY_TRACE_INFO_GET_RUNTIME_ENABLED_PATHS_USING_SET_VALUE        \
  ThreadLocalTrace()->Write("SetRuntimeEnabledPaths forcing paths: %s", \
                            str(*paths));

#define RUY_TRACE_INFO_GET_RUNTIME_ENABLED_PATHS_USING_ENV_VAR        \
  ThreadLocalTrace()->Write("Environment variable forcing paths: %s", \
                            str(*paths));

#define RUY_TRACE_INFO_GET_RUNTIME_ENABLED_PATHS_USING_DETECTION \
  ThreadLocalTrace()->Write(                                     \
      "Runtime-detected paths: %s",                              \
      str(*paths & ~kNonArchPathsIncludingInternalVariants));

#define RUY_TRACE_INFO_PREPARE_PACKED_MATRICES_SHOULD_CACHE            \
  ThreadLocalTrace()->Write(                                           \
      "Caching the packed %s matrix. Already in cache: %s", str(side), \
      action == PrepackedCache::Action::kInsertedNewEntry ? "no" : "yes");

#define RUY_TRACE_INFO_PREPARE_PACKED_MATRICES_NO_CACHE \
  ThreadLocalTrace()->Write("Not caching the packed %s matrix.", str(side));

#define RUY_TRACE_INFO_GET_TENTATIVE_THREAD_COUNT                          \
  ThreadLocalTrace()->Write(                                               \
      "tentative_thread_count=%d (determined based on shape %dx%dx%d and " \
      "max_num_threads=%d)",                                               \
      tentative_thread_count, rows, depth, cols, ctx->max_num_threads());

#define RUY_TRACE_INFO_GET_USE_SIMPLE_LOOP_RETURNS_TRUE                    \
  ThreadLocalTrace()->Write(                                               \
      "Choosing to use the simple loop code path in TrMul because of the " \
      "linear traversal and single thread.");

#define RUY_TRACE_INFO_GET_USE_SIMPLE_LOOP_RETURNS_FALSE                    \
  ThreadLocalTrace()->Write(                                                \
      "Choosing to use the general case code path in TrMul because of: %s", \
      tentative_thread_count > 1 ? "multi-threading"                        \
                                 : "non-linear traversal order");

#define RUY_TRACE_INFO_TRMUL_SIMPLE_LOOP \
  ThreadLocalTrace()->Write("Entering the simple loop code path of TrMul");

#define RUY_TRACE_INFO_TRMUL_GENERAL_CASE \
  ThreadLocalTrace()->Write("Entering the general case code path of TrMul");

#define RUY_TRACE_INFO_MAKE_BLOCK_MAP_START                                    \
  ThreadLocalTrace()->Write("Kernel block: %dx%d", kernel_rows, kernel_cols);  \
  ThreadLocalTrace()->Write(                                                   \
      "BlockMap shape: %dx%d (destination matrix shape rounded to next "       \
      "kernel blocks)",                                                        \
      rows, cols);                                                             \
  ThreadLocalTrace()->Write(                                                   \
      "Rectangularness log2: %dx%d (powers of two factors bringing the shape " \
      "closest to square)",                                                    \
      rows_rectangularness_log2, cols_rectangularness_log2);                   \
  ThreadLocalTrace()->Write("Accumulation depth: %d", depth);                  \
  ThreadLocalTrace()->Write("LHS scalar type size: %d", lhs_scalar_size);      \
  ThreadLocalTrace()->Write("RHS scalar type size: %d", rhs_scalar_size);      \
  ThreadLocalTrace()->Write("Tentative thread count: %d",                      \
                            tentative_thread_count);                           \
  ThreadLocalTrace()->Write(                                                   \
      "CPU cache params: local_cache_size=%d, last_level_cache_size=%d",       \
      cpu_cache_params.local_cache_size,                                       \
      cpu_cache_params.last_level_cache_size);                                 \
  ThreadLocalTrace()->Write(                                                   \
      "For the sizes below, when rows!=cols, we always retain the min of the " \
      "two.");                                                                 \
  ThreadLocalTrace()->Write("Kernel block size_log2: %d", kernel_size_log2);   \
  ThreadLocalTrace()->Write(                                                   \
      "BlockMap size_log2: %d (destination matrix shape rounded to next "      \
      "kernel blocks)",                                                        \
      size_log2);                                                              \
  ThreadLocalTrace()->Write(                                                   \
      "Now we will pick the optimal log2 of BlockMap block size");

#define RUY_TRACE_INFO_MAKE_BLOCK_MAP_EACH_TENTATIVE_BLOCK_SIZE           \
  ThreadLocalTrace()->Write(                                              \
      "For BlockMap block size_log2 %d, score=%d ("                       \
      "multithreading_score=%d + cache_locality_score=%d + "              \
      "kernel_amortization_score=%d)",                                    \
      block_size_log2, score, multithreading_score, cache_locality_score, \
      kernel_amortization_score);

#define RUY_TRACE_INFO_MAKE_BLOCK_MAP_END                                      \
  ThreadLocalTrace()->Write("Selecting BlockMap block size_log2: %d",          \
                            best_score_block_size_log2);                       \
  ThreadLocalTrace()->Write(                                                   \
      "BlockMap has %dx%d blocks, each of size between %dx%d and %dx%d.",      \
      1 << num_blocks_of_rows_log2, 1 << num_blocks_of_cols_log2,              \
      block_map->small_block_dims[Side::kLhs],                                 \
      block_map->small_block_dims[Side::kRhs],                                 \
      block_map->small_block_dims[Side::kLhs] +                                \
          block_map->kernel_dims[Side::kLhs],                                  \
      block_map->small_block_dims[Side::kRhs] +                                \
          block_map->kernel_dims[Side::kRhs]);                                 \
  ThreadLocalTrace()->Write(                                                   \
      "The first %d rows of blocks have %d rows, the remaining ones have %d "  \
      "rows ",                                                                 \
      block_map->large_blocks[Side::kLhs],                                     \
      block_map->small_block_dims[Side::kLhs] +                                \
          block_map->kernel_dims[Side::kLhs],                                  \
      block_map->small_block_dims[Side::kLhs]);                                \
  ThreadLocalTrace()->Write(                                                   \
      "The first %d columns of blocks have %d columns, the remaining ones "    \
      "have %d columns ",                                                      \
      block_map->large_blocks[Side::kRhs],                                     \
      block_map->small_block_dims[Side::kRhs] +                                \
          block_map->kernel_dims[Side::kLhs],                                  \
      block_map->small_block_dims[Side::kRhs]);                                \
  ThreadLocalTrace()->Write(                                                   \
      "Traversal order: %s",                                                   \
      block_map->traversal_order == BlockMapTraversalOrder::kLinear ? "linear" \
      : block_map->traversal_order == BlockMapTraversalOrder::kFractalZ        \
          ? "fractal Z-curve"                                                  \
      : block_map->traversal_order == BlockMapTraversalOrder::kFractalU        \
          ? "fractal U-curve"                                                  \
      : block_map->traversal_order == BlockMapTraversalOrder::kFractalHilbert  \
          ? "fractal Hilbert curve"                                            \
          : nullptr);                                                          \
  ThreadLocalTrace()->Write("Finalized thread count: %d",                      \
                            block_map->thread_count);

#define RUY_TRACE_SET_THEAD_ID(thread_id) \
  ThreadLocalTrace()->set_thread_id(thread_id);

#define RUY_TRACE_INFO_TRMUL_TASK_MAIN_LOOP_GOT_BLOCK_COORDS              \
  ThreadLocalTrace()->Write(                                              \
      "Block #%d is at position (%d, %d) in the BlockMap.", block_id,     \
      block[Side::kLhs], block[Side::kRhs]);                              \
  ThreadLocalTrace()->Write(                                              \
      "Block #%d has shape %dx%d and starts at position (%d, %d) in the " \
      "destination matrix.",                                              \
      block_id, end[Side::kLhs] - start[Side::kLhs],                      \
      end[Side::kRhs] - start[Side::kRhs], start[Side::kLhs],             \
      start[Side::kRhs]);                                                 \
  ThreadLocalTrace()->Write(                                              \
      "Block #%d depends on LHS panel #%d and RHS panel #%d.", block_id,  \
      block[Side::kLhs], block[Side::kRhs]);

#define RUY_TRACE_INFO_TRYPACK_PACKING                                  \
  ThreadLocalTrace()->Write(                                            \
      "%s panel #%d is not already packed. Packing it now.", str(side), \
      block);

#define RUY_TRACE_INFO_TRYPACK_ANOTHER_THREAD_PACKING                \
  if (!ThreadLocalTrace()->is_in_run_ahead_packing_loop()) {         \
    ThreadLocalTrace()->Write(                                       \
        "%s panel #%d is currently being packed by another thread.", \
        str(side), block);                                           \
  }

#define RUY_TRACE_INFO_TRYPACK_PREVIOUSLY_PACKED                          \
  if (!ThreadLocalTrace()->is_in_run_ahead_packing_loop()) {              \
    ThreadLocalTrace()->Write("%s panel #%d had previously been packed.", \
                              str(side), block);                          \
  }

#define RUY_TRACE_INFO_TRYPACK_PACKED_BY_ANOTHER_THREAD                  \
  ThreadLocalTrace()->Write(                                             \
      "%s panel #%d has just been packed by another thread.", str(side), \
      block);

#define RUY_TRACE_INFO_ENSURE_PACKED_ENTER_RUN_AHEAD                       \
  if (!ThreadLocalTrace()->is_in_run_ahead_packing_loop()) {               \
    ThreadLocalTrace()->set_is_in_run_ahead_packing_loop(true);            \
    ThreadLocalTrace()->Write(                                             \
        "We're blocked on other threads packing the panels that we need. " \
        "Packing some other panels while we wait...");                     \
  }

#define RUY_TRACE_INFO_ENSURE_PACKED_END                                  \
  if (ThreadLocalTrace()->is_in_run_ahead_packing_loop()) {               \
    ThreadLocalTrace()->set_is_in_run_ahead_packing_loop(false);          \
    ThreadLocalTrace()->Write(                                            \
        "Other threads have finished packing what we were waiting for."); \
  }

#define RUY_TRACE_INFO_RUN_PACK                                               \
  ThreadLocalTrace()->Write("Path: %s", str(ThePath));                        \
  ThreadLocalTrace()->Write("Packing panel consisting of columns [%d, %d)",   \
                            start_col, end_col);                              \
  ThreadLocalTrace()->Write("Source: columns [%d, %d) of %s", start_col,      \
                            end_col, str(src_matrix));                        \
  ThreadLocalTrace()->Write("Destination: columns [%d, %d) of %s", start_col, \
                            end_col, str(*packed_matrix));                    \
  if (end_col > src_matrix.layout.cols) {                                     \
    ThreadLocalTrace()->Write(                                                \
        "This runs past the last column of the source matrix. Padding as "    \
        "needed.");                                                           \
  }                                                                           \
  if (packed_matrix->layout.rows > src_matrix.layout.rows) {                  \
    ThreadLocalTrace()->Write(                                                \
        "The packed matrix has more rows than the source matrix due to "      \
        "rounding up to the kernel block size. Padding as needed.");          \
  }

#define RUY_TRACE_INFO_RUN_KERNEL                                              \
  {                                                                            \
    ThreadLocalTrace()->Write("Path: %s", str(KernelArgs<KernelType>::kPath)); \
    int lhs_cols = end[Side::kLhs] - start[Side::kLhs];                        \
    int rhs_cols = end[Side::kRhs] - start[Side::kRhs];                        \
    int kernel_lhs_cols = src[Side::kLhs].layout.kernel.cols;                  \
    int kernel_rhs_cols = src[Side::kRhs].layout.kernel.cols;                  \
    ThreadLocalTrace()->Write("LHS: columns [%d, %d) of %s",                   \
                              start[Side::kLhs], end[Side::kLhs],              \
                              str(src[Side::kLhs]));                           \
    ThreadLocalTrace()->Write("RHS: columns [%d, %d) of %s",                   \
                              start[Side::kRhs], end[Side::kRhs],              \
                              str(src[Side::kRhs]));                           \
    ThreadLocalTrace()->Write("Destination: block [%d, %d)x[%d, %d) of %s",    \
                              start[Side::kLhs], end[Side::kLhs],              \
                              start[Side::kRhs], end[Side::kRhs], str(*dst));  \
    if (end[Side::kLhs] > dst->layout.rows ||                                  \
        end[Side::kRhs] > dst->layout.cols) {                                  \
      ThreadLocalTrace()->Write(                                               \
          "This runs over the destination matrix boundaries. The kernel will " \
          "internally clamp stores to avoid overruns.");                       \
    }                                                                          \
    ThreadLocalTrace()->Write(                                                 \
        "The kernel's inner loop only produces a %dx%d block, so the "         \
        "kernel's outer loops will run %dx%d times.",                          \
        kernel_lhs_cols, kernel_rhs_cols, lhs_cols / kernel_lhs_cols,          \
        rhs_cols / kernel_rhs_cols);                                           \
  }

#define RUY_TRACE_INFO_THREAD_FUNC_IMPL_WAITING \
  ThreadLocalTrace()->Write("Waiting for a task...");

#define RUY_TRACE_INFO_THREADPOOL_EXECUTE_STARTING_TASK \
  ThreadLocalTrace()->Write("Sending task #%d to a worker thread...", i);

#define RUY_TRACE_INFO_THREADPOOL_EXECUTE_STARTING_TASK_ZERO_ON_CUR_THREAD \
  ThreadLocalTrace()->Write("Running task #0 on the current thread...");

#define RUY_TRACE_INFO_THREADPOOL_EXECUTE_WAITING_FOR_THREADS \
  ThreadLocalTrace()->Write("Waiting for worker threads to finish..");

#define RUY_TRACE_INFO(id)                                 \
  [=]() {                                                  \
    ThreadLocalTrace()->set_current_source_file(__FILE__); \
    ThreadLocalTrace()->set_current_source_line(__LINE__); \
    RUY_TRACE_INFO_##id                                    \
  }()

}  // namespace ruy

#else

// Vacuous implementation when RUY_TRACE is not defined.
#define RUY_TRACE_SCOPE_NAME(name)
#define RUY_TRACE_SCOPE
#define RUY_TRACE_SET_THEAD_ID(thread_id)
#define RUY_TRACE_INFO(id)

#endif

#endif  // RUY_RUY_TRACE_H_
