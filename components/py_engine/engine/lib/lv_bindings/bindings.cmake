# This file is to be given as "make USER_C_MODULES=..." when building Micropython port

include(${CMAKE_CURRENT_LIST_DIR}/mkrules.cmake)

# lvgl bindings depend on lvgl itself, pull it in
include(${LVGL_DIR}/CMakeLists.txt)

# lvgl bindings target (the mpy module)
add_library(usermod_lv_bindings INTERFACE)
target_sources(usermod_lv_bindings INTERFACE ${LV_SRC})
target_include_directories(usermod_lv_bindings INTERFACE ${LV_INCLUDE})

target_link_libraries(usermod_lv_bindings INTERFACE lvgl_interface)

# make usermod (target declared by Micropython for all user compiled modules) link to bindings
# this way the bindings (and transitively lvgl_interface) get proper compilation flags
target_link_libraries(usermod INTERFACE usermod_lv_bindings)

# create targets for generated files
all_lv_bindings()
