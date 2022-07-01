# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/jinouxiang/Documents/work/esp/esp-idf/components/bootloader/subproject"
  "/Users/jinouxiang/Documents/work/test_download/components/py_engine/ports/esp32/build/bootloader"
  "/Users/jinouxiang/Documents/work/test_download/components/py_engine/ports/esp32/build/bootloader-prefix"
  "/Users/jinouxiang/Documents/work/test_download/components/py_engine/ports/esp32/build/bootloader-prefix/tmp"
  "/Users/jinouxiang/Documents/work/test_download/components/py_engine/ports/esp32/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/jinouxiang/Documents/work/test_download/components/py_engine/ports/esp32/build/bootloader-prefix/src"
  "/Users/jinouxiang/Documents/work/test_download/components/py_engine/ports/esp32/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/jinouxiang/Documents/work/test_download/components/py_engine/ports/esp32/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
