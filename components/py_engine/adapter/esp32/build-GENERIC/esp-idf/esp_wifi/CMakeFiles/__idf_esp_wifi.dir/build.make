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
include esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.make

# Include the progress variables for this target.
include esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/progress.make

# Include the compile flags for this target's objects.
include esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/coexist.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/coexist.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/coexist.c > CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/coexist.c -o CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/lib_printf.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/lib_printf.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/lib_printf.c > CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/lib_printf.c -o CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/mesh_event.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/mesh_event.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/mesh_event.c > CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/mesh_event.c -o CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/phy_init.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/phy_init.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/phy_init.c > CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/phy_init.c -o CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/smartconfig.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/smartconfig.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/smartconfig.c > CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/smartconfig.c -o CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/smartconfig_ack.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/smartconfig_ack.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/smartconfig_ack.c > CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/smartconfig_ack.c -o CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_init.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_init.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_init.c > CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_init.c -o CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_default.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_default.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_default.c > CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_default.c -o CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_netif.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_netif.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_netif.c > CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/src/wifi_netif.c -o CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.s

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/flags.make
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj: /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/esp32/esp_adapter.c
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj -MF CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj.d -o CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj -c /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/esp32/esp_adapter.c

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.i"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/esp32/esp_adapter.c > CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.i

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.s"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && /Users/ethan.lcz1/.espressif/tools/xtensa-esp32-elf/esp-2020r3-8.4.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi/esp32/esp_adapter.c -o CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.s

# Object files for target __idf_esp_wifi
__idf_esp_wifi_OBJECTS = \
"CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj" \
"CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj"

# External object files for target __idf_esp_wifi
__idf_esp_wifi_EXTERNAL_OBJECTS =

esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/coexist.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/lib_printf.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/mesh_event.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/phy_init.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/smartconfig_ack.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_init.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_default.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/src/wifi_netif.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/esp32/esp_adapter.c.obj
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/build.make
esp-idf/esp_wifi/libesp_wifi.a: esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Linking CXX static library libesp_wifi.a"
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && $(CMAKE_COMMAND) -P CMakeFiles/__idf_esp_wifi.dir/cmake_clean_target.cmake
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/__idf_esp_wifi.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/build: esp-idf/esp_wifi/libesp_wifi.a
.PHONY : esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/build

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/clean:
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi && $(CMAKE_COMMAND) -P CMakeFiles/__idf_esp_wifi.dir/cmake_clean.cmake
.PHONY : esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/clean

esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/depend:
	cd /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32 /Users/ethan.lcz1/Desktop/git/aos33/hardware/chip/espressif_esp32/esp-idf/components/esp_wifi /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi /Users/ethan.lcz1/Desktop/git/aos33/components/py_engine/adapter/esp32/build-GENERIC/esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : esp-idf/esp_wifi/CMakeFiles/__idf_esp_wifi.dir/depend

