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
include esp-idf/cbor/CMakeFiles/__idf_cbor.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/cbor/CMakeFiles/__idf_cbor.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborencoder_close_container_checked.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborencoder_close_container_checked.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborencoder_close_container_checked.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborencoder_close_container_checked.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborencoder.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborencoder.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborencoder.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborencoder.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborerrorstrings.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborerrorstrings.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborerrorstrings.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborerrorstrings.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborparser_dup_string.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborparser_dup_string.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborparser_dup_string.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborparser_dup_string.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborparser.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborparser.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborparser.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborparser.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborpretty_stdio.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborpretty_stdio.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborpretty_stdio.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborpretty_stdio.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborpretty.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborpretty.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborpretty.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborpretty.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cbortojson.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cbortojson.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cbortojson.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cbortojson.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborvalidation.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborvalidation.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborvalidation.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/cborvalidation.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.s

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/flags.make
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/open_memstream.c
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj -MF CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj.d -o CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/open_memstream.c

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/open_memstream.c > CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.i

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor/tinycbor/src/open_memstream.c -o CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.s

# Object files for target __idf_cbor
__idf_cbor_OBJECTS = \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj" \
"CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj"

# External object files for target __idf_cbor
__idf_cbor_EXTERNAL_OBJECTS =

esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder_close_container_checked.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborencoder.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborerrorstrings.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser_dup_string.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborparser.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty_stdio.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborpretty.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cbortojson.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/cborvalidation.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/tinycbor/src/open_memstream.c.obj
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/build.make
esp-idf/cbor/libcbor.a: esp-idf/cbor/CMakeFiles/__idf_cbor.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX static library libcbor.a"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && $(CMAKE_COMMAND) -P CMakeFiles/__idf_cbor.dir/cmake_clean_target.cmake
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_cbor.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/cbor/CMakeFiles/__idf_cbor.dir/build: esp-idf/cbor/libcbor.a
.PHONY : esp-idf/cbor/CMakeFiles/__idf_cbor.dir/build

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/clean:
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor && $(CMAKE_COMMAND) -P CMakeFiles/__idf_cbor.dir/cmake_clean.cmake
.PHONY : esp-idf/cbor/CMakeFiles/__idf_cbor.dir/clean

esp-idf/cbor/CMakeFiles/__idf_cbor.dir/depend:
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32 /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/cbor /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/cbor/CMakeFiles/__idf_cbor.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/cbor/CMakeFiles/__idf_cbor.dir/depend

