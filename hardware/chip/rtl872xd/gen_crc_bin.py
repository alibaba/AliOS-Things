#!/usr/bin/env python3

import sys
import os
import shutil
import subprocess
import platform

homePath = os.path.expanduser("~")
toolchainPath = os.path.join(homePath, ".aliot", "arm-none-eabi", "main", "bin")
objcopy = os.path.join(toolchainPath, "arm-none-eabi-objcopy")
nm = os.path.join(toolchainPath, "arm-none-eabi-nm")
strip = os.path.join(toolchainPath, "arm-none-eabi-strip")
ccSize = os.path.join(toolchainPath, "arm-none-eabi-size")
objdump = os.path.join(toolchainPath, "arm-none-eabi-objdump")

outputDIR = os.getcwd()
amebazDIR = os.path.join(outputDIR, "..", "..", "hardware", "chip", "rtl872xd")
binDIR = os.path.join(amebazDIR, "bin")

allBinOutputFile = os.path.join(outputDIR, "binary", "all.bin")
binOutputFile = os.path.join(outputDIR, "binary", "burn.bin")

km0BootFile = os.path.join(binDIR, "km0_boot_all.bin")
km0BootOffset = "0x0"
km02BootFile = os.path.join(binDIR, "km0_boot_all_2nd.bin")
km4BootFile = os.path.join(binDIR, "km4_boot_all.bin")
km4BootOffset = "0x4000"
ateFile = os.path.join(binDIR, "ate.bin")
ateOffset = "0x188000"
otaOffset = "0x6000"

buildBinPy = os.path.join(amebazDIR, "release", "auto_build_tool", "build_bin.py")
releaseDIR = os.path.join(amebazDIR, "release")

"""
Concatenate multiple binary files
"""
def concatBinFile(FileLists, outputFilePath):
    outputFile = open(outputFilePath, "ab")
    for i in FileLists:
        tmpFile = open(i, "rb")
        fileContent = tmpFile.read()
        outputFile.write(fileContent)
        tmpFile.close()
    outputFile.close()

"""
Generate the final km0_km4_bin file.
rawBinFile e.g. /usr/xxxx/alios/solutions/helloworld_demo/out/helloworld_demo@haas200.bin
"""
def gen_km0_km4_bin(rawBinFile):
    rawBinFileArray = rawBinFile.split(".bin")
    rawElfFile = rawBinFileArray[0] + ".elf"

    print("========= linker img2_ns start =========")
    imageTargetFolder = os.path.join(outputDIR, "binary")
    if os.path.exists(imageTargetFolder):
        shutil.rmtree(imageTargetFolder, True)
    os.makedirs(imageTargetFolder, 0o755)
    targetImg2AXF = os.path.join(imageTargetFolder, "target_img2.axf")
    shutil.copyfile(rawElfFile, targetImg2AXF)

    targetImg2Map = os.path.join(imageTargetFolder, "target_img2.map")
    subprocess.call(nm + " " + targetImg2AXF + " | " + "sort " + "> " + targetImg2Map,
    shell=True)
    targetImg2Asm = os.path.join(imageTargetFolder, "target_img2.asm")
    subprocess.call(objdump + " -d " + targetImg2AXF + " > " + targetImg2Asm,
    shell=True)

    targetPureImg2AXF = os.path.join(imageTargetFolder, "target_pure_img2.axf")
    shutil.copyfile(targetImg2AXF, targetPureImg2AXF)
    subprocess.call([strip, targetPureImg2AXF])
    targetPureImg2Map = os.path.join(imageTargetFolder, "target_pure_img2.map")
    subprocess.call(nm + " " + targetPureImg2AXF + " | sort > " + targetPureImg2Map,
    shell=True)

    ram2Bin = os.path.join(imageTargetFolder, "ram_2.bin")
    subprocess.call([objcopy, "-j", ".ram_image2.entry", "-j", ".ram_image2.text", "-j",
    ".ram_image2.data", "-Obinary", targetPureImg2AXF, ram2Bin])

    xipImage2Bin = os.path.join(imageTargetFolder, "xip_image2.bin")
    subprocess.call([objcopy, "-j", ".xip_image2.text", "-Obinary", targetPureImg2AXF,
    xipImage2Bin])

    psram2Bin = os.path.join(imageTargetFolder, "psram_2.bin")
    subprocess.call([objcopy, "-j", ".psram_image2.text", "-j", ".psram_image2.data", "-Obinary",
    targetPureImg2AXF, psram2Bin])

    # for bluetooth trace
    appTrace = os.path.join(imageTargetFolder, "APP.trace")
    subprocess.call([objcopy, "-j", ".bluetooth_trace.text", "-Obinary",
    targetPureImg2AXF, appTrace])

    print("========== Image Info HEX ==========")
    subprocess.call([ccSize, "-A", "--radix=16", targetImg2AXF])
    subprocess.call([ccSize, "-t", "--radix=16", targetImg2AXF])
    print("========== Image Info HEX ==========")

    print("========== Image Info DEC ==========")
    subprocess.call([ccSize, "-A", "--radix=10", targetImg2AXF])
    subprocess.call([ccSize, "-t", "--radix=10", targetImg2AXF])
    print("========== Image Info DEC ==========")

    print("========== linker img2_ns end ==========")

    print("========== Image manipulating start ==========")
    prependHeaderPy = os.path.join(amebazDIR, "prepend_header.py")
    subprocess.call(["python", prependHeaderPy, ram2Bin, "__ram_image2_text_start__",
    targetImg2Map])
    subprocess.call(["python", prependHeaderPy, xipImage2Bin, "__flash_text_start__",
    targetImg2Map])
    subprocess.call(["python", prependHeaderPy, psram2Bin, "__psram_image2_text_start__",
    targetImg2Map])

    xipImage2PrependBinPath = os.path.join(imageTargetFolder, "xip_image2_prepend.bin")
    ram2PrependBinPath = os.path.join(imageTargetFolder, "ram_2_prepend.bin")
    psram2PrependBinPath = os.path.join(imageTargetFolder, "psram_2_prepend.bin")
    km4Image2AllBinPath = os.path.join(imageTargetFolder, "km4_image2_all.bin")

    concatBinFile([xipImage2PrependBinPath, ram2PrependBinPath, psram2PrependBinPath],
    km4Image2AllBinPath)
    padPy = os.path.join(amebazDIR, "pad.py")
    subprocess.call(["python", padPy, km4Image2AllBinPath])

    km0Image2AllBinPath = os.path.join(binDIR, "km0_image2_all.bin")
    km0Km4Image2BinPath = os.path.join(imageTargetFolder, "km0_km4_image2.bin")
    concatBinFile([km0Image2AllBinPath, km4Image2AllBinPath], km0Km4Image2BinPath)
    shutil.copyfile(km0Km4Image2BinPath, binOutputFile)
    print("========== Image manipulating end ==========")

    print("========== Generate all.bin ==========")
    try:
        os.remove(allBinOutputFile)
    except OSError:
        pass

    genCommonBinOutputFilePy = os.path.join(amebazDIR, "gen_common_bin_output_file.py")
    subprocess.call(["python", genCommonBinOutputFilePy, "-o", allBinOutputFile, "-f",
    km0BootOffset, km0BootFile])
    subprocess.call(["python", genCommonBinOutputFilePy, "-o", allBinOutputFile, "-f",
    km4BootOffset, km4BootFile])
    subprocess.call(["python", genCommonBinOutputFilePy, "-o", allBinOutputFile, "-f",
    otaOffset, km0Km4Image2BinPath])

    if not os.path.exists(ateFile):
        subprocess.call(["python", genCommonBinOutputFilePy, "-o", allBinOutputFile, "-f",
        ateOffset, ateFile])

    shutil.copyfile(km02BootFile, os.path.join(imageTargetFolder, "km0_boot_all_2nd.bin"))

    if platform.system() == "Windows":
        subprocess.call([os.path.join(amebazDIR, "tools", "genfs.bat")])
    else:
        subprocess.call([os.path.join(amebazDIR, "tools", "genfs.sh")])

    shutil.copyfile(os.path.join(amebazDIR, "prebuild", "littlefs.bin"),
    os.path.join(imageTargetFolder, "littlefs.bin"))
    subprocess.call(["python", buildBinPy, "--target=" + km0Km4Image2BinPath])
    ymodemBurnAllBin = os.path.join(imageTargetFolder, "ymodem_burn_all.bin")
    shutil.copyfile(km0Km4Image2BinPath, ymodemBurnAllBin)

    stupidBinFile = os.path.join(releaseDIR, "auto_build_tool", "stupid.bin")
    ymodemBurnAllBinFileHdl = open(ymodemBurnAllBin, "r+b")
    stupidBinFileHdl = open(stupidBinFile, "rb")
    stupidBinFileContent = stupidBinFileHdl.read()
    stupidBinFileHdl.close()
    ymodemBurnAllBinFileHdl.seek(0)
    ymodemBurnAllBinFileHdl.write(stupidBinFileContent)
    ymodemBurnAllBinFileHdl.close()
    shutil.copyfile(ymodemBurnAllBin, os.path.join(releaseDIR, "write_flash_gui", "ota_bin",
    "ymodem_burn_all.bin"))

    otaBinDir = os.path.join(releaseDIR, "write_flash_gui", "ota_bin")
    shutil.copyfile(os.path.join(otaBinDir, "ota_rtos_ota_all.bin"),
    os.path.join(imageTargetFolder, "ota_burn_all.bin"))
    shutil.copyfile(os.path.join(otaBinDir, "ota_rtos_ota_xz.bin.xz"),
    os.path.join(imageTargetFolder, "ota_burn_xz.bin"))
    shutil.copyfile(os.path.join(otaBinDir, "ota_rtos_ota_xz.bin.xz"),
    os.path.join(otaBinDir, "ymodem_burn_xz.bin"))
    shutil.copyfile(os.path.join(otaBinDir, "ymodem_burn_xz.bin"),
    os.path.join(imageTargetFolder, "ymodem_burn_xz.bin"))

    print("========== Generate all.bin end ==========")


if __name__ == "__main__":
    # e.g. args: --target="/usr/xxxx/alios/solutions/helloworld_demo/out/helloworld_demo@haas200.bin"
    args = sys.argv[1].split("=")
    gen_km0_km4_bin(args[1])
