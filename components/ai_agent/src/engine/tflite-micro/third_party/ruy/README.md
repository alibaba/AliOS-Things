# The ruy matrix multiplication library

This is not an officially supported Google product.

ruy is a matrix multiplication library. Its focus is to cover the matrix
multiplication needs of neural network inference engines. Its initial user has
been TensorFlow Lite, where it is used by default on the ARM CPU architecture.

ruy supports both floating-point and 8bit-integer-quantized matrices.

## Efficiency

ruy is designed to achieve high performance not just on very large sizes, as
is the focus of many established libraries, but on whatever are the actual sizes
and shapes of matrices most critical in current TensorFlow Lite applications.
This often means quite small sizes, e.g. 100x100 or even 50x50, and all sorts of
rectangular shapes. It's not as fast as completely specialized code for each
shape, but it aims to offer a good compromise of speed across all shapes and a
small binary size.

## Documentation

Some documentation will eventually be available in the doc/ directory, see
[doc/README.md](doc/README.md).

