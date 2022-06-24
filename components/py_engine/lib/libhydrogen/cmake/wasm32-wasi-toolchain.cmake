# Cross-compilation file for WebAssembly with WASI.

# To use, pass -DCMAKE_TOOLCHAIN_FILE=cmake/wasm32-wasi-toolchain.cmake in your CMake command line.
# You'll also need to specify a sysroot directory with -DHYDROGEN_WASM32_WASI_SYSROOT_DIR=XXX.

cmake_minimum_required(VERSION 3.12)

set(project_setting_prefix HYDROGEN)
set(setting_prefix "${project_setting_prefix}_WASM32_WASI")
function(get_setting setting_name setting_type setting_description)
  string(TOUPPER "${setting_prefix}_${setting_name}" setting_external_name)
  set("${setting_external_name}" "" CACHE "${setting_type}" "${setting_description}")
  set("${setting_name}" "${${setting_external_name}}" PARENT_SCOPE)
endfunction()

# Sysroot setting

get_setting(sysroot_dir STRING "Directory containing the wasm32-wasi sysroot.")

# Locate toolchain programs

set(program_prefix "${setting_prefix}_PROGRAM")
function(find_in_toolchain program_name)
  string(TOUPPER "${program_prefix}_${program_name}" program_external_name)
  string(REPLACE "_" "-" program_file_name "${program_name}")

  find_program("${program_external_name}" "${program_file_name}")

  if("${${program_external_name}}" STREQUAL "${program_external_name}-NOTFOUND")
    message(FATAL_ERROR "Couldn't find toolchain program ${program_file_name}")
  else()
    set("${program_name}" "${${program_external_name}}" PARENT_SCOPE)
  endif()
endfunction()

find_in_toolchain(clang)
find_in_toolchain(llvm_ranlib)
find_in_toolchain(llvm_ar)
find_in_toolchain(llvm_nm)
find_in_toolchain(llvm_strip)

# Configure CMake toolchain settings

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER "${clang}")
set(CMAKE_ASM_COMPILER "${clang}")
set(CMAKE_RANLIB "${llvm_ranlib}")
set(CMAKE_AR "${llvm_ar}")
set(CMAKE_NM "${llvm_nm}")
set(CMAKE_STRIP "${llvm_strip}")

set(CMAKE_C_OUTPUT_EXTENSION .o)
set(CMAKE_ASM_OUTPUT_EXTENSION .o)

# Set compile flags

string(CONCAT CMAKE_C_FLAGS " -DED25519_NONDETERMINISTIC=1 --target=wasm32-wasi"
                            " --sysroot=${sysroot_dir} -Wl,--stack-first")
