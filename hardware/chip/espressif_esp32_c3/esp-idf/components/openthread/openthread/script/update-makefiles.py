#!/usr/bin/env python3
#
#  Copyright (c) 2021, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

# This script updates different make/build files (CMakeLists.txt, BUILD.gn,
# Andriod.mk, Andriod.bp, auto-make) in OpenThread repo based on the
# current files present in `./src/core/` & `./include/openthread/`
# folders. This script MUST be called from openthread root folder.

import os

#----------------------------------------------------------------------------------------------
# Helper functions


def get_file_list(path, extension):
    """Get a sorted list of full file names (with path) in a given `path` folder having a given `extension`"""
    return sorted([
        "{}/{}".format(dir_path, file_name)[2:]
        for dir_path, dir_names, file_names in os.walk(path)
        for file_name in file_names
        if file_name.endswith(extension)
    ])


def read_txt_file(file_name):
    """Read the content of a text file with name `file_name` and return content as a list of lines"""
    with open(file_name, 'r') as file:
        lines = file.readlines()
    return lines


def write_txt_file(file_name, lines):
    """Write a text file with name `file_name` with the content given as a list of `lines`"""
    with open(file_name, 'w') as file:
        file.writelines(lines)


def update_build_file(file_name, start_string, end_string, new_list, search_string=None):
    """
    Update the file given by `file_name` by replacing the list after the first occurrence of `start_string` up to
    `end_string` with the `new_list`. If `search_string` is given, then the search for `start_string` only starts
    after seeing the `search_string` line in the file.
    """
    STATE_SEARCH = 1
    STATE_MATCH_START = 2
    STATE_MATCH_END = 3
    STATE_DONE = 4

    lines = read_txt_file(file_name)
    new_lines = []
    state = STATE_SEARCH if search_string else STATE_MATCH_START

    for line in lines:
        if state == STATE_SEARCH:
            new_lines.append(line)
            if line.startswith(search_string):
                state = STATE_MATCH_START
        elif state == STATE_MATCH_START:
            new_lines.append(line)
            if line.startswith(start_string):
                new_lines.extend(new_list)
                state = STATE_MATCH_END
        elif state == STATE_MATCH_END:
            if line.startswith(end_string):
                new_lines.append(line)
                state = STATE_DONE
        else:
            new_lines.append(line)

    if state != STATE_DONE:
        raise RuntimeError('failed to update build file: {}'.format(file_name))

    if new_lines != lines:
        write_txt_file(file_name, new_lines)


#----------------------------------------------------------------------------------------------

# Get the list of hpp/h/cpp files in different folders.

src_core_path = "./src/core"
core_h_files = get_file_list(src_core_path, '.h')
core_hpp_files = get_file_list(src_core_path, '.hpp')
core_cpp_files = get_file_list(src_core_path, '.cpp')
core_cpp_files = [item for item in core_cpp_files if not item.endswith("extension_example.cpp")]
core_hpp_cpp_files = sorted(core_hpp_files + core_cpp_files)
core_h_hpp_files = sorted(core_h_files + core_hpp_files)

include_path = "./include/openthread"
include_h_files = get_file_list(include_path, '.h')
include_ot_h_files = [name[8:] for name in include_h_files if not name.startswith("include/openthread/platform")]
include_platform_h_files = [name[8:] for name in include_h_files if name.startswith("include/openthread/platform")]

#----------------------------------------------------------------------------------------------
# Update CMakeLists.txt files

core_cmakelist_txt_file = "./src/core/CMakeLists.txt"

formatted_list = ["    {}\n".format(file_name[9:]) for file_name in core_cpp_files]
update_build_file(core_cmakelist_txt_file, "set(COMMON_SOURCES\n", ")\n", formatted_list)

print("Updated " + core_cmakelist_txt_file)

#----------------------------------------------------------------------------------------------
# Update Build.gn files

core_build_gn_file = "./src/core/BUILD.gn"

formatted_list = ["  \"{}\",\n".format(file_name[9:]) for file_name in core_hpp_cpp_files]
update_build_file(core_build_gn_file, "openthread_core_files = [\n", "]\n", formatted_list)

formatted_list = ["    \"{}\",\n".format(file_name[9:]) for file_name in core_h_files]
update_build_file(core_build_gn_file, "  public = [\n", "  ]\n", formatted_list)

print("Updated " + core_build_gn_file)

include_build_gn_file = "./include/openthread/BUILD.gn"

formatted_list = ["    \"{}\",\n".format(file_name[19:]) for file_name in include_h_files]
update_build_file(include_build_gn_file, "  public = [\n", "  ]\n", formatted_list)

print("Updated " + include_build_gn_file)

#----------------------------------------------------------------------------------------------
# Update Android.mk file

android_mk_file = "./Android.mk"

formatted_list = ["    {:<63} \\\n".format(file_name) for file_name in core_cpp_files]
start_string = "LOCAL_SRC_FILES                                                  := \\\n"
end_string = "    src/lib/hdlc/hdlc.cpp"
update_build_file(android_mk_file, start_string, end_string, formatted_list)

print("Updated " + android_mk_file)

#----------------------------------------------------------------------------------------------
# Update Makefile.am files

core_makefile_am_file = "./src/core/Makefile.am"

formatted_list = ["    {:<45} \\\n".format(file_name[9:]) for file_name in core_cpp_files]
start_string = "SOURCES_COMMON                                  = \\\n"
end_string = "    $(NULL)\n"
update_build_file(core_makefile_am_file, start_string, end_string, formatted_list)

formatted_list = ["    {:<45} \\\n".format(file_name[9:]) for file_name in core_h_hpp_files]
start_string = "HEADERS_COMMON                                  = \\\n"
end_string = "    $(NULL)\n"
update_build_file(core_makefile_am_file, start_string, end_string, formatted_list)

print("Updated " + core_makefile_am_file)

include_makefile_am_file = "./include/Makefile.am"

formatted_list = ["    {:<37} \\\n".format(file_name) for file_name in include_ot_h_files]
start_string = "openthread_headers                      = \\\n"
end_string = "    $(NULL)\n"
update_build_file(include_makefile_am_file, start_string, end_string, formatted_list)

formatted_list = ["    {:<37} \\\n".format(file_name) for file_name in include_platform_h_files]
start_string = "ot_platform_headers                     = \\\n"
end_string = "    $(NULL)\n"
update_build_file(include_makefile_am_file, start_string, end_string, formatted_list)

print("Updated " + include_makefile_am_file)
