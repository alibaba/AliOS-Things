# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Users/ethan.lcz1/.aliot/miniconda3/envs/_aos_env/lib/python3.8/site-packages/cmake/data/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Users/ethan.lcz1/.aliot/miniconda3/envs/_aos_env/lib/python3.8/site-packages/cmake/data/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC

# Include any dependencies generated for this target.
include esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/flags.make

esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj: esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/flags.make
esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_ipc/ipc.c
esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj: esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_ipc && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj -MF CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj.d -o CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_ipc/ipc.c

esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/ipc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_ipc.dir/ipc.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_ipc && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_ipc/ipc.c > CMakeFiles/__idf_esp_ipc.dir/ipc.c.i

esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/ipc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_ipc.dir/ipc.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_ipc && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_ipc/ipc.c -o CMakeFiles/__idf_esp_ipc.dir/ipc.c.s

# Object files for target __idf_esp_ipc
__idf_esp_ipc_OBJECTS = \
"CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj"

# External object files for target __idf_esp_ipc
__idf_esp_ipc_EXTERNAL_OBJECTS =

esp-idf/esp_ipc/libesp_ipc.a: esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/ipc.c.obj
esp-idf/esp_ipc/libesp_ipc.a: esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/build.make
esp-idf/esp_ipc/libesp_ipc.a: esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libesp_ipc.a"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_ipc && $(CMAKE_COMMAND) -P CMakeFiles/__idf_esp_ipc.dir/cmake_clean_target.cmake
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_ipc && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_esp_ipc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/build: esp-idf/esp_ipc/libesp_ipc.a
.PHONY : esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/build

esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/clean:
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_ipc && $(CMAKE_COMMAND) -P CMakeFiles/__idf_esp_ipc.dir/cmake_clean.cmake
.PHONY : esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/clean

esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/depend:
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32 /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_ipc /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_ipc /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/esp_ipc/CMakeFiles/__idf_esp_ipc.dir/depend

