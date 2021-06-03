cmake_minimum_required(VERSION 3.10)

project(libqrencode)

set(libqrencode_sources
  qrencode.c
  qrinput.c
  bitstream.c
  qrspec.c
  rsecc.c
  split.c
  mask.c
  mqrspec.c
  mmask.c
)

add_definitions(
  -DHAVE_CONFIG_H=1
)

# Add the static library
add_library(libqrencode STATIC ${libqrencode_sources})
target_include_directories(libqrencode INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(libqrencode PRIVATE ${PRODUCT_CONFIG_DIR} ${LVGL_DIR})

