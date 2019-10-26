C++ multithreading instructions：
When using multithreaded related classes such as mutex in the C++ library, you need to configure the _GLIBCXX_HAS_GTHREADS macro and adapt to multithreaded related interfaces.
This directory contains two adapt-related files (verified on GCC 5.4) that need to be replaced in the toolchain which used. When using a later version of GCC compiler, refer to the files in this directory.
At the same time, when using c functions encapsulated by c++, such as STD ::to_string, you also need to configure c++config.h, such as defining the _GLIBCXX_USE_C99 macro.

Compile options configuration example:
GLOBAL_LDFLAGS += -lstdc++
GLOBAL_CXXFLAGS += -std=c++11 -D_GLIBCXX_HAS_GTHREADS