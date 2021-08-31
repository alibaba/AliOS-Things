#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#include "ruy/context.h"
#include "ruy/matrix.h"
#include "ruy/mul_params.h"
#include "ruy/ruy.h"

template <typename... Dst>
void read_cmdline_args(bool help, int argc, char* argv[], const char* name,
                       const char* format, const char* default_value,
                       const char* allowed_values, Dst... dst) {
  if (help) {
    fprintf(stderr, "%-20s %-12s %-16s %s\n", name, format, default_value,
            allowed_values ? allowed_values : "");
    return;
  }
  const char* value = default_value;
  for (int i = 1; i < argc; i++) {
    if (std::strstr(argv[i], name) == argv[i]) {
      const char* equal_sign = std::strchr(argv[i], '=');
      if (equal_sign == argv[i] + std::strlen(name)) {
        value = equal_sign + 1;
      }
      break;
    }
  }
  if (allowed_values) {
    if (!std::strstr(allowed_values, value)) {
      fprintf(stderr, "Illegal value %s. The legal values are %s.\n", value,
              allowed_values);
      exit(1);
    }
  }
  if (sizeof...(Dst) != sscanf(value, format, dst...)) {
    fprintf(stderr, "Failed to parse %s\n", value);
    exit(1);
  }
}

struct Params {
  char types[100];
  int m, k, n;  // matmul shape m*k*n
  int paths;
  int num_threads;
  int repeat;
  int lhs_cache_policy;
  int rhs_cache_policy;
  int lhs_stride;
  int rhs_stride;
  int dst_stride;
  int lhs_zero_point;
  int rhs_zero_point;
  int dst_zero_point;
  char lhs_order[100];
  char rhs_order[100];
  char dst_order[100];
};

template <typename LhsType, typename RhsType, typename DstType>
void run(const Params& params) {
  using AccumType =
      typename std::conditional<std::is_floating_point<DstType>::value, DstType,
                                std::int32_t>::type;

  ruy::Matrix<LhsType> lhs;
  ruy::Matrix<RhsType> rhs;
  ruy::Matrix<DstType> dst;

  auto parse_order = [](const char* name) {
    if (!std::strcmp(name, "row-major")) {
      return ruy::Order::kRowMajor;
    } else if (!std::strcmp(name, "column-major")) {
      return ruy::Order::kColMajor;
    } else {
      fprintf(stderr, "Failed to parse %s\n", name);
      exit(1);
    }
  };

  auto make_layout = [](int rows, int cols, int stride, ruy::Order order,
                        ruy::Layout* layout) {
    layout->set_rows(rows);
    layout->set_cols(cols);
    layout->set_order(order);
    int base_stride = order == ruy::Order::kRowMajor ? cols : rows;
    layout->set_stride(stride ? stride : base_stride);
  };

  make_layout(params.m, params.k, params.lhs_stride,
              parse_order(params.lhs_order), lhs.mutable_layout());
  make_layout(params.k, params.n, params.rhs_stride,
              parse_order(params.rhs_order), rhs.mutable_layout());
  make_layout(params.m, params.n, params.dst_stride,
              parse_order(params.dst_order), dst.mutable_layout());

  lhs.set_zero_point(params.lhs_zero_point);
  rhs.set_zero_point(params.rhs_zero_point);
  dst.set_zero_point(params.dst_zero_point);

  lhs.set_cache_policy(static_cast<ruy::CachePolicy>(params.lhs_cache_policy));
  rhs.set_cache_policy(static_cast<ruy::CachePolicy>(params.rhs_cache_policy));

  auto flat_size = [](const ruy::Layout& layout) {
    int outer_size =
        layout.order() == ruy::Order::kRowMajor ? layout.rows() : layout.cols();
    return outer_size * layout.stride();
  };

  std::vector<LhsType> lhs_buf(flat_size(lhs.layout()));
  std::vector<RhsType> rhs_buf(flat_size(rhs.layout()));
  std::vector<DstType> dst_buf(flat_size(dst.layout()));

  lhs.set_data(lhs_buf.data());
  rhs.set_data(rhs_buf.data());
  dst.set_data(dst_buf.data());

  ruy::Context context;
  context.set_max_num_threads(params.num_threads);
  context.set_runtime_enabled_paths(static_cast<ruy::Path>(params.paths));

  ruy::MulParams<AccumType, DstType> mul_params;
  // Here an actual application might set some mul_params fields.
  // Quantization multipliers, bias-vector, clamp bounds, etc.

  for (int r = 0; r < params.repeat; r++) {
    ruy::Mul(lhs, rhs, mul_params, &context, &dst);
  }
}

int main(int argc, char* argv[]) {
  bool help = argc == 1 || (argc == 2 && !strcmp(argv[1], "--help"));
  if (help) {
    fprintf(stderr, "Command-line flags (all in the form --flag=value):\n");
    fprintf(stderr, "%-20s %-12s %-16s %s\n", "flag", "format", "default",
            "allowed");
  }
  Params params;
  const char* allowed_types =
      "f32xf32->f32, i8xi8->i8, i8xi8->i16, i8xi8->i32, u8xu8->i16, u8xi8->u8";
  const char* allowed_orders = "row-major, column-major";
  read_cmdline_args(help, argc, argv, "--types", "%s", "f32xf32->f32",
                    allowed_types, &params.types);
  read_cmdline_args(help, argc, argv, "--shape", "%dx%dx%d", "100x100x100",
                    nullptr, &params.m, &params.k, &params.n);
  read_cmdline_args(help, argc, argv, "--paths", "%x", "0", nullptr,
                    &params.paths);
  read_cmdline_args(help, argc, argv, "--num_threads", "%d", "1", nullptr,
                    &params.num_threads);
  read_cmdline_args(help, argc, argv, "--repeat", "%d", "1", nullptr,
                    &params.repeat);
  read_cmdline_args(help, argc, argv, "--lhs_cache_policy", "%d", "0",
                    "0, 1, 2, 3", &params.lhs_cache_policy);
  read_cmdline_args(help, argc, argv, "--rhs_cache_policy", "%d", "0",
                    "0, 1, 2, 3", &params.rhs_cache_policy);
  read_cmdline_args(help, argc, argv, "--lhs_stride", "%d", "0", nullptr,
                    &params.lhs_stride);
  read_cmdline_args(help, argc, argv, "--rhs_stride", "%d", "0", nullptr,
                    &params.rhs_stride);
  read_cmdline_args(help, argc, argv, "--dst_stride", "%d", "0", nullptr,
                    &params.dst_stride);
  read_cmdline_args(help, argc, argv, "--lhs_zero_point", "%d", "0", nullptr,
                    &params.lhs_zero_point);
  read_cmdline_args(help, argc, argv, "--rhs_zero_point", "%d", "0", nullptr,
                    &params.rhs_zero_point);
  read_cmdline_args(help, argc, argv, "--dst_zero_point", "%d", "0", nullptr,
                    &params.dst_zero_point);
  read_cmdline_args(help, argc, argv, "--lhs_order", "%s", "row-major",
                    allowed_orders, &params.lhs_order);
  read_cmdline_args(help, argc, argv, "--rhs_order", "%s", "row-major",
                    allowed_orders, &params.rhs_order);
  read_cmdline_args(help, argc, argv, "--rhs_order", "%s", "row-major",
                    allowed_orders, &params.dst_order);

  if (help) {
    exit(1);
  }

  if (!strcmp(params.types, "f32xf32->f32")) {
    run<float, float, float>(params);
  } else if (!strcmp(params.types, "i8xi8->i8")) {
    run<std::int8_t, std::int8_t, std::int8_t>(params);
  } else if (!strcmp(params.types, "i8xi8->i16")) {
    run<std::int8_t, std::int8_t, std::int16_t>(params);
  } else if (!strcmp(params.types, "i8xi8->i32")) {
    run<std::int8_t, std::int8_t, std::int32_t>(params);
  } else if (!strcmp(params.types, "u8xu8->i16")) {
    run<std::uint8_t, std::uint8_t, std::int16_t>(params);
  } else if (!strcmp(params.types, "u8xi8->u8")) {
    run<std::uint8_t, std::int8_t, std::uint8_t>(params);
  } else {
    fprintf(stderr, "Unknown types: %s\n", params.types);
    exit(1);
  }
}
