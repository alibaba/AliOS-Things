cmake_minimum_required (VERSION 3.6)

project(DspRefLibs)

# Needed to find the config modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../..)




set(REFSRC src/BasicMathFunctions/abs.c
  src/BasicMathFunctions/add.c
  src/BasicMathFunctions/dot_prod.c
  src/BasicMathFunctions/mult.c
  src/BasicMathFunctions/negate.c
  src/BasicMathFunctions/offset.c
  src/BasicMathFunctions/scale.c
  src/BasicMathFunctions/shift.c
  src/BasicMathFunctions/sub.c
  src/ComplexMathFunctions/cmplx_conj.c
  src/ComplexMathFunctions/cmplx_dot_prod.c
  src/ComplexMathFunctions/cmplx_mag.c
  src/ComplexMathFunctions/cmplx_mag_squared.c
  src/ComplexMathFunctions/cmplx_mult_cmplx.c
  src/ComplexMathFunctions/cmplx_mult_real.c
  src/ControllerFunctions/pid.c
  src/ControllerFunctions/sin_cos.c
  src/FastMathFunctions/cos.c
  src/FastMathFunctions/sin.c
  src/FastMathFunctions/sqrt.c
  src/FilteringFunctions/biquad.c
  src/FilteringFunctions/conv.c
  src/FilteringFunctions/correlate.c
  src/FilteringFunctions/fir.c
  src/FilteringFunctions/fir_decimate.c
  src/FilteringFunctions/fir_interpolate.c
  src/FilteringFunctions/fir_lattice.c
  src/FilteringFunctions/fir_sparse.c
  src/FilteringFunctions/iir_lattice.c
  src/FilteringFunctions/lms.c
  src/HelperFunctions/mat_helper.c
  src/HelperFunctions/ref_helper.c
  src/Intrinsics/intrinsics.c
  src/MatrixFunctions/mat_add.c
  src/MatrixFunctions/mat_cmplx_mult.c
  src/MatrixFunctions/mat_inverse.c
  src/MatrixFunctions/mat_mult.c
  src/MatrixFunctions/mat_scale.c
  src/MatrixFunctions/mat_sub.c
  src/MatrixFunctions/mat_trans.c
  src/StatisticsFunctions/max.c
  src/StatisticsFunctions/mean.c
  src/StatisticsFunctions/min.c
  src/StatisticsFunctions/power.c
  src/StatisticsFunctions/rms.c
  src/StatisticsFunctions/std.c
  src/StatisticsFunctions/var.c
  src/SupportFunctions/copy.c
  src/SupportFunctions/fill.c
  src/SupportFunctions/fixed_to_fixed.c
  src/SupportFunctions/fixed_to_float.c
  src/SupportFunctions/float_to_fixed.c
  src/TransformFunctions/bitreversal.c
  src/TransformFunctions/cfft.c
  src/TransformFunctions/dct4.c
  src/TransformFunctions/rfft.c
  )

add_library(DspRefLibs STATIC ${REFSRC})

include(config)
configdsp(DspRefLibs ../../Source)

### Includes
target_include_directories(DspRefLibs PUBLIC "inc")
target_include_directories(DspRefLibs PUBLIC "../../Include")


