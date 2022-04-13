NAME := cmake_comp

# component information
$(NAME)_MBINS_TYPE := share
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := cmake_comp

# extra makefile
$(NAME)_SELF_BUIlD_COMP_targets := cmake_test/build/lib/libhello.a
$(NAME)_SELF_BUIlD_COMP_scripts := call_cmake.sh

# component public API header file folder
GLOBAL_INCLUDES += ./cmake_test/lib

# add mandatory depends depend_comp_name sample
#$(NAME)_COMPONENTS += depend_comp_name

# add optional depends optional_comp_name sample
# CMAKE_COMP_CONDITIION1 will be set to y or n in Config.in of this component
#$(NAME)_COMPONENTS-$(CMAKE_COMP_CONDITIION1) += optional_comp_name

