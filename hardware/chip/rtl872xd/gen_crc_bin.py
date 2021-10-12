#!/usr/bin/env python3
# -*- coding:utf-8 -*-
#
# Copyright (C) 2021 Alibaba Group Holding Limited

"""
Generate the bin files including ota bin and the final bin files for chip rtl872xd.
"""

import sys
import os
import shutil
import subprocess
import platform


home_path = os.path.expanduser("~")
toolchain_path = os.path.join(home_path, ".aliot", "arm-ali-aoseabi", "main", "bin")
objcopy = os.path.join(toolchain_path, "arm-ali-aoseabi-objcopy")
nm = os.path.join(toolchain_path, "arm-ali-aoseabi-nm")
strip = os.path.join(toolchain_path, "arm-ali-aoseabi-strip")
cc_size = os.path.join(toolchain_path, "arm-ali-aoseabi-size")
objdump = os.path.join(toolchain_path, "arm-ali-aoseabi-objdump")

output_dir = os.getcwd()
amebaz_dir = os.path.join(output_dir, "..", "..", "hardware", "chip", "rtl872xd")
bin_dir = os.path.join(amebaz_dir, "bin")

all_bin_output_file = os.path.join(output_dir, "binary", "all.bin")
bin_output_file = os.path.join(output_dir, "binary", "burn.bin")

km0_boot_file = os.path.join(bin_dir, "km0_boot_all.bin")
km02_boot_file = os.path.join(bin_dir, "km0_boot_all_2nd.bin")
km4_boot_file = os.path.join(bin_dir, "km4_boot_all.bin")
ate_file = os.path.join(bin_dir, "ate.bin")
KM0_BOOT_OFFSET = "0x0"
KM4_BOOT_OFFSET = "0x4000"
ATE_OFFSET = "0x188000"
OTA_OFFSET = "0x6000"

build_bin_py = os.path.join(amebaz_dir, "release", "auto_build_tool", "build_bin.py")
release_dir = os.path.join(amebaz_dir, "release")


def concatenate_bin_file(file_lists, output_file_path):
    """Concatenate multiple binary files.

    Concatenate binary files in file_lists array into the output_file_path.

    Args:
        file_lists: The input files tuple.
        output_file_path: The output file containing all input files.

    Returns: None
    """
    output_file = open(output_file_path, "ab")
    for i in file_lists:
        tmpFile = open(i, "rb")
        fileContent = tmpFile.read()
        output_file.write(fileContent)
        tmpFile.close()
    output_file.close()


def gen_km0_km4_bin(raw_bin_file):
    """Generate the final bin files.

    Generate the bin files including ota bin and the final km0_km4_image2.bin files.

    Args:
        raw_bin_file: The compiled raw bin file, e.g. /usr/xxxx/alios/solutions/helloworld_demo/out/
                      helloworld_demo@haas200.bin

    Returns: None
    """

    raw_bin_file_array = raw_bin_file.split(".bin")
    raw_elf_file = raw_bin_file_array[0] + ".elf"

    print("========= linker img2_ns start =========")
    image_target_folder = os.path.join(output_dir, "binary")
    if os.path.exists(image_target_folder):
        shutil.rmtree(image_target_folder, True)
    os.makedirs(image_target_folder, 0o755)
    target_img2_axf = os.path.join(image_target_folder, "target_img2.axf")
    shutil.copyfile(raw_elf_file, target_img2_axf)

    target_img2_map = os.path.join(image_target_folder, "target_img2.map")
    subprocess.call(nm + " " + target_img2_axf + " | " + "sort " + "> " + target_img2_map,
                    shell=True)
    target_img2_asm = os.path.join(image_target_folder, "target_img2.asm")
    subprocess.call(objdump + " -d " + target_img2_axf + " > " + target_img2_asm,
                    shell=True)

    target_pure_img2_axf = os.path.join(image_target_folder, "target_pure_img2.axf")
    shutil.copyfile(target_img2_axf, target_pure_img2_axf)
    subprocess.call([strip, target_pure_img2_axf])
    target_pure_img2_map = os.path.join(image_target_folder, "target_pure_img2.map")
    subprocess.call(nm + " " + target_pure_img2_axf + " | sort > " + target_pure_img2_map,
                    shell=True)

    ram2_bin = os.path.join(image_target_folder, "ram_2.bin")
    subprocess.call([objcopy, "-j", ".ram_image2.entry", "-j", ".ram_image2.text", "-j",
                    ".ram_image2.data", "-Obinary", target_pure_img2_axf, ram2_bin])

    xip_image2_bin = os.path.join(image_target_folder, "xip_image2.bin")
    subprocess.call([objcopy, "-j", ".xip_image2.text", "-Obinary", target_pure_img2_axf,
                    xip_image2_bin])

    psram2_bin = os.path.join(image_target_folder, "psram_2.bin")
    subprocess.call([objcopy, "-j", ".psram_image2.text", "-j", ".psram_image2.data", "-Obinary",
                    target_pure_img2_axf, psram2_bin])

    print("========== Image Info HEX ==========")
    subprocess.call([cc_size, "-A", "--radix=16", target_img2_axf])
    subprocess.call([cc_size, "-t", "--radix=16", target_img2_axf])
    print("========== Image Info HEX ==========")

    print("========== Image Info DEC ==========")
    subprocess.call([cc_size, "-A", "--radix=10", target_img2_axf])
    subprocess.call([cc_size, "-t", "--radix=10", target_img2_axf])
    print("========== Image Info DEC ==========")

    print("========== linker img2_ns end ==========")

    print("========== Image manipulating start ==========")
    prepend_header_py = os.path.join(amebaz_dir, "prepend_header.py")
    subprocess.call(["python", prepend_header_py, ram2_bin, "__ram_image2_text_start__",
                    target_img2_map])
    subprocess.call(["python", prepend_header_py, xip_image2_bin, "__flash_text_start__",
                    target_img2_map])
    subprocess.call(["python", prepend_header_py, psram2_bin, "__psram_image2_text_start__",
                    target_img2_map])

    xip_image2_prepend_bin_path = os.path.join(image_target_folder, "xip_image2_prepend.bin")
    ram2_prepend_bin_path = os.path.join(image_target_folder, "ram_2_prepend.bin")
    psram2_prepend_bin_path = os.path.join(image_target_folder, "psram_2_prepend.bin")
    km4_image2_all_bin_path = os.path.join(image_target_folder, "km4_image2_all.bin")

    concatenate_bin_file([xip_image2_prepend_bin_path, ram2_prepend_bin_path, psram2_prepend_bin_path],
                          km4_image2_all_bin_path)
    padPy = os.path.join(amebaz_dir, "pad.py")
    subprocess.call(["python", padPy, km4_image2_all_bin_path])

    km0_image2_all_bin_path = os.path.join(bin_dir, "km0_image2_all.bin")
    km0_km4_image2_bin_path = os.path.join(image_target_folder, "km0_km4_image2.bin")
    concatenate_bin_file([km0_image2_all_bin_path, km4_image2_all_bin_path], km0_km4_image2_bin_path)
    shutil.copyfile(km0_km4_image2_bin_path, bin_output_file)
    print("========== Image manipulating end ==========")

    print("========== Generate littlefs, ota, ymodem bins ==========")
    shutil.copyfile(km02_boot_file, os.path.join(image_target_folder, "km0_boot_all_2nd.bin"))
    if platform.system() == "Windows":
        subprocess.call([os.path.join(amebaz_dir, "tools", "genfs.bat")])
    else:
        subprocess.call([os.path.join(amebaz_dir, "tools", "genfs.sh")])

    shutil.copyfile(os.path.join(amebaz_dir, "prebuild", "littlefs.bin"),
    os.path.join(image_target_folder, "littlefs.bin"))
    subprocess.call(["python", build_bin_py, "--target=" + km0_km4_image2_bin_path])
    ymodem_burn_all_bin = os.path.join(image_target_folder, "ymodem_burn_all.bin")
    shutil.copyfile(km0_km4_image2_bin_path, ymodem_burn_all_bin)

    stupid_bin_file = os.path.join(release_dir, "auto_build_tool", "stupid.bin")
    ymodem_burn_all_bin_file_hdl = open(ymodem_burn_all_bin, "r+b")
    stupid_bin_file_hdl = open(stupid_bin_file, "rb")
    stupid_bin_file_content = stupid_bin_file_hdl.read()
    stupid_bin_file_hdl.close()
    ymodem_burn_all_bin_file_hdl.seek(0)
    ymodem_burn_all_bin_file_hdl.write(stupid_bin_file_content)
    ymodem_burn_all_bin_file_hdl.close()
    shutil.copyfile(ymodem_burn_all_bin, os.path.join(release_dir, "write_flash_gui", "ota_bin",
                    "ymodem_burn_all.bin"))

    ota_bin_dir = os.path.join(release_dir, "write_flash_gui", "ota_bin")
    shutil.copyfile(os.path.join(ota_bin_dir, "ota_rtos_ota_all.bin"),
    os.path.join(image_target_folder, "ota_burn_all.bin"))
    shutil.copyfile(os.path.join(ota_bin_dir, "ota_rtos_ota_xz.bin.xz"),
    os.path.join(image_target_folder, "ota_burn_xz.bin"))
    shutil.copyfile(os.path.join(ota_bin_dir, "ota_rtos_ota_xz.bin.xz"),
    os.path.join(ota_bin_dir, "ymodem_burn_xz.bin"))
    shutil.copyfile(os.path.join(ota_bin_dir, "ymodem_burn_xz.bin"),
    os.path.join(image_target_folder, "ymodem_burn_xz.bin"))

    print("========== Generate littlefs, ota, ymodem bins end ==========")


if __name__ == "__main__":
    # e.g. args: --target=
    # "/usr/xxxx/alios/solutions/helloworld_demo/out/helloworld_demo@haas200.bin"
    args = sys.argv[1].split("=")
    gen_km0_km4_bin(args[1])
