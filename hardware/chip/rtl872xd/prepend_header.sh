#!/bin/bash 

################
# Library
################
Usage() {
    echo "Usage: $0 [Image Name] [Start Symbol Name] [Symbols List File]"
}

# Parameter:
#     value, width, dest
function MakeFixedWidthHeaderString() {
    local __value=$1
    local __width=$2
    local __dest=$3
    local __reorder=$4
    local __header_raw
    local __header_raw_reorder
    local __header_array

    if [[ "$__dest" ]]; then

        __header_raw=$(printf "%0""$__width""x" $__value)
        # echo $__header_raw

        if [ "$__reorder" -eq 1 ]; then
                # 20000680 to 80060020
                for (( i=$__width; i > 0; i-=2 ))
                do
                    __header_raw_reorder+=$(echo ${__header_raw:(i-2):2})
                done
        else
                __header_raw_reorder=$__header_raw
        fi
        #echo $__header_raw_reorder

        __header_array=($(echo $__header_raw_reorder | sed 's/\(.\)/\1 /g'))

        for (( i=0; i < $__width; i+=2))
        do
            eval $__dest+='\\x'"${__header_array[$i]}${__header_array[$i+1]}"
        done
    fi
}

################
# Main
################
if [ "$#" -lt 3 ]; then
    Usage
    exit 1
fi

# Get Parameters
IMAGE_FILENAME=$1
IMAGE_SECTION_START_NAME=$2
SYMBOL_LIST=$3

# Constant Variables
PATTERN_1=0x99999696
PATTERN_2=0x3FCC66FC
RSVD=0xFFFFFFFFFFFFFFFF
IMG2SIGN=0x3831393538373131
IMAGE_LEN=$(du -b $IMAGE_FILENAME | cut -f 1)
IMAGE_ADDR="0x$(grep $IMAGE_SECTION_START_NAME $SYMBOL_LIST | awk '{print $1}')"
IMAGE_FILENAME_PREPEND="${IMAGE_FILENAME%.*}"'_prepend.'"${IMAGE_FILENAME##*.}"

IMAGE_FILENAME_NEW=$(basename $IMAGE_FILENAME)

HEADER_FINAL=''
if [ "$IMAGE_FILENAME_NEW" == "ram_1.bin" ]; then
    MakeFixedWidthHeaderString $PATTERN_1   8  HEADER_FINAL 0
    MakeFixedWidthHeaderString $PATTERN_2   8  HEADER_FINAL 0
elif [ "$IMAGE_FILENAME_NEW" == "xip_boot.bin" ]; then
    MakeFixedWidthHeaderString $PATTERN_1   8  HEADER_FINAL 0
    MakeFixedWidthHeaderString $PATTERN_2   8  HEADER_FINAL 0
else
    MakeFixedWidthHeaderString $IMG2SIGN    16  HEADER_FINAL 0
fi

MakeFixedWidthHeaderString $IMAGE_LEN   8  HEADER_FINAL 1
MakeFixedWidthHeaderString $IMAGE_ADDR  8  HEADER_FINAL 1

MakeFixedWidthHeaderString $RSVD  16  HEADER_FINAL 0
MakeFixedWidthHeaderString $RSVD  16  HEADER_FINAL 0
# echo $HEADER_FINAL

echo -n -e $HEADER_FINAL | cat - $IMAGE_FILENAME > $IMAGE_FILENAME_PREPEND


