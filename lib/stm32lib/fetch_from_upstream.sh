#!/bin/bash
#
# Fetch new code for an entire MCU series from an upstream STM repository.

if [ $# -ne 2 ]; then
    echo "usage: $0 <mcu> <src>"
    echo ""
    echo "eg: $0 STM32WB path/to/STM32CubeWB"
    echo "where STM32CubeWB is from https://github.com/STMicroelectronics/STM32CubeWB.git"
    exit 1
fi

mcu=$1
gitsrc=$2

cmsis=CMSIS/${mcu}xx
hal=${mcu}xx_HAL_Driver

if [ ! -d $cmsis ]; then
    echo "WARNING: $cmsis is not an existing directory, assuming a new MCU series"
    mkdir -p $cmsis
fi

if [ ! -d $hal ]; then
    echo "WARNING: $hal is not an existing directory, assuming a new MCU series"
    mkdir -p $hal
fi

# CMSIS: remove any old files and copy across the new ones.
echo "Fetching CMSIS to $cmsis"
rm -rf $cmsis/Include
rm -rf $cmsis/Source
cp -r $gitsrc/Drivers/CMSIS/Device/ST/${mcu}xx/Include $cmsis/
cp -r $gitsrc/Drivers/CMSIS/Device/ST/${mcu}xx/Source $cmsis/
rm -rf CMSIS/${mcu}xx/Source/Templates/*/linker

# HAL: remove any old files and copy across the new ones.
echo "Fetching HAL to $hal"
rm -rf $hal/Inc
rm -rf $hal/Src
cp -r $gitsrc/Drivers/${mcu}xx_HAL_Driver/Inc $hal/
cp -r $gitsrc/Drivers/${mcu}xx_HAL_Driver/Src $hal/

# Make sure tr and exand don't expect UTF-8, because these files aren't
export LC_CTYPE=C

for dir in $cmsis $hal; do
    # Process the new source code to:
    # - remove trailing white-space
    # - convert to unix line-endings
    # - expand tabs with 4 spaces
    # - convert non-ascii chars to ascii equivalent (should only be in comments)
    echo "Processing source code in $dir"
    for file in $(find $dir -name "*.[chs]"); do
        chmod 644 $file
        cat $file | awk "{sub(/[\t ]*$/,\"\")}1" | expand -t 4 | tr \\200\\205\\211\\221\\222\\223\\224\\226\\231\\244\\261\\265\\302\\327\\342 \'??\'\'\"\"\\-\'??u?x\' > tmp$$
        /bin/mv tmp$$ $file
    done
done

