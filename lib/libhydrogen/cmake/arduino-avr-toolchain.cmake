# Cross-compilation file for the Arduino/AVR toolchain.

# To use, pass -DCMAKE_TOOLCHAIN_FILE=cmake/arduino-avr-toolchain.cmake in your CMake command line.
# You can specify the target device MCU identifier with -DHYDROGEN_ARDUINO_AVR_TARGET_DEVICE=XXX.

cmake_minimum_required(VERSION 3.12)

set(project_setting_prefix HYDROGEN)
function(set_project_setting setting_name setting_value)
  set("${project_setting_prefix}_${setting_name}" "${setting_value}" CACHE INTERNAL "")
endfunction()

set(setting_prefix "${project_setting_prefix}_ARDUINO_AVR")
function(get_setting setting_name setting_type setting_description)
  string(TOUPPER "${setting_prefix}_${setting_name}" setting_external_name)
  set("${setting_external_name}" "" CACHE "${setting_type}" "${setting_description}")
  set("${setting_name}" "${${setting_external_name}}" PARENT_SCOPE)
endfunction()

# Target device setting

get_setting(target_device STRING "Target Arduino device MCU identifier.")
if(NOT target_device)
  set(target_device atmega328p)
endif()

if("${target_device}" STREQUAL atmega328p)
  set_project_setting(TARGET_DEVICE ATMEGA328)
else()
  message(FATAL_ERROR "Unrecognized ${setting_prefix}_TARGET_DEVICE value ${target_device}")
endif()

# Find Arduino SDK home

get_setting(sdk_dir PATH "Arduino SDK home directory.")

# Try ARDUINO_SDK_PATH environment variable.
if(NOT sdk_dir)
  if(DEFINED ENV{ARDUINO_SDK_PATH})
    set(sdk_dir "$ENV{ARDUINO_SDK_PATH}")
  endif()
endif()

# Try some platform-specific guesses.
if(NOT sdk_dir)
  # Windows
  if(WIN32)
    list(APPEND arduino_home_dir_guesses "C:/Program Files (x86)/Arduino"
                                         "C:/Program Files/Arduino")
  endif()

  # macOS
  if(APPLE)
    list(APPEND arduino_home_dir_guesses "/Applications/Arduino.app/Contents/Java")
  endif()

  # Linux/Unix
  if(UNIX AND NOT APPLE)
    list(APPEND arduino_home_dir_guesses "/usr/share/arduino" "/usr/local/share/arduino")
  endif()

  if(DEFINED arduino_home_dir_guesses)
    foreach(arduino_home_dir_guess IN LISTS arduino_home_dir_guesses)
      if(IS_DIRECTORY "${arduino_home_dir_guess}")
        set(sdk_dir "${arduino_home_dir_guess}")
        break()
      endif()
    endforeach()
  endif()
endif()

if(NOT sdk_dir)
  message(FATAL_ERROR "Couldn't determine Arduino SDK home directory. "
                      "Try passing -D${setting_prefix}_SDK_DIR=... to the CMake command line, or "
                      "set the ARDUINO_SDK_PATH environment variable.")
endif()

# Locate toolchain programs

set(arduino_tools_dir "${sdk_dir}/hardware/tools/avr/bin")
set(program_prefix "${setting_prefix}_PROGRAM")
function(find_in_toolchain program_name)
  string(TOUPPER "${program_prefix}_${program_name}" program_external_name)
  string(REPLACE "_" "-" program_file_name "${program_name}")

  find_program("${program_external_name}" "${program_file_name}"
               PATHS "${arduino_tools_dir}"
               NO_DEFAULT_PATH)

  if("${${program_external_name}}" STREQUAL "${program_external_name}-NOTFOUND")
    message(FATAL_ERROR "Couldn't find program ${program_file_name} "
                        "in Arduino/AVR toolchain at ${arduino_tools_dir}")
  else()
    set("${program_name}" "${${program_external_name}}" PARENT_SCOPE)
  endif()
endfunction()

find_in_toolchain(avr_gcc)
find_in_toolchain(avr_gcc_ranlib)
find_in_toolchain(avr_gcc_ar)
find_in_toolchain(avr_gcc_nm)
find_in_toolchain(avr_strip)

# Configure CMake toolchain settings

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER "${avr_gcc}")
set(CMAKE_ASM_COMPILER "${avr_gcc}")
set(CMAKE_RANLIB "${avr_gcc_ranlib}")
set(CMAKE_AR "${avr_gcc_ar}")
set(CMAKE_NM "${avr_gcc_nm}")
set(CMAKE_STRIP "${avr_strip}")

set(CMAKE_C_OUTPUT_EXTENSION .o)
set(CMAKE_ASM_OUTPUT_EXTENSION .o)

# Set compile flags

string(CONCAT CMAKE_C_FLAGS " -mmcu=${target_device} -mcall-prologues -fno-exceptions"
                            " -ffunction-sections -fdata-sections -flto")

# Add include directories

include_directories(SYSTEM "${sdk_dir}/hardware/arduino/avr/cores/arduino"
                           "${sdk_dir}/hardware/arduino/avr/variants/standard"
                           "${sdk_dir}/hardware/arduino/cores/arduino"
                           "${sdk_dir}/hardware/arduino/variants/standard")
