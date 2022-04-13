#!/bin/sh

PART_INFO_FILE=$1
XIP_CONFIG_FILE=$2
LD_SCRIPT_IN=$3
LD_SCRIPT_OUT=$4
LD_SCRIPT_XIP=$5

ADDRESS=`cat $PART_INFO_FILE | sed 's/[[:space:]]//g' | grep "name:prim" | awk -F'address:' '{printf "%s\n", $2}' | awk -F',' '{printf "%s\n", $1}'`
SIZE=`cat $PART_INFO_FILE | sed 's/[[:space:]]//g' | grep "name:prim" | awk -F'size:' '{printf "%s\n", $2}' | awk -F',' '{printf "%s\n", $1}'`

# echo $ADDRESS $SIZE

##operate the XIP LD file
if [ "$LD_SCRIPT_XIP" = "y" ]; then
    EX_PRE_STRING="*(EXCLUDE_FILE("
    EX_BAK_STRING=") *.text*)"
    IN_PRE_STRING="*lib"
    IN_BAK_STRING=".a:(*.text*)"
    VALID_FLAG=`cat $XIP_CONFIG_FILE | grep "LIB_RUN_IN_EXTERNAL_MEDIUM=" | awk -F'LIB_RUN_IN_EXTERNAL_MEDIUM=' '{printf "%s\n", $1}'`
    LIB_FILE=`cat $XIP_CONFIG_FILE | grep "LIB_RUN_IN_EXTERNAL_MEDIUM=" | awk -F'LIB_RUN_IN_EXTERNAL_MEDIUM=' '{printf "%s\n", $2}'`
    LAST_LIB=`echo "$LIB_FILE" | awk -F' ' '{printf "%s\n", $NF}'`

    ##LIB_RUN_IN_EXTERNAL_MEDIUM is invalid
    if [ "$LIB_FILE" == "" ] || [ "$VALID_FLAG" == "#" ]; then
        EXCLUDE_FILE="*(.text)\n  *(.text*)\n  *(.text.*)"
        cat $LD_SCRIPT_IN | sed 's/EXCLUDE_FILE_USER_DEFINE/'"$EXCLUDE_FILE"'/g' > $LD_SCRIPT_OUT
    #    line=`grep -n ".ram_text" /tmp/1.a | head -1 | cut -d ":" -f 1`
    #    line_end=`expr $line + 4`
    #    sed ''$line','$line_end'd' /tmp/1.a > $LD_SCRIPT_OUT
        sed -i -e '/USER_DEFINE_REGION/d' $LD_SCRIPT_OUT
    else
        ##LIB_RUN_IN_EXTERNAL_MEDIUM is valid
        for lib in ${LIB_FILE[@]}
        do
            if [ "$LAST_LIB" = "$lib" ]; then
                EXCLUDE_FILE+="*lib"$lib".a"
                INCLUDE_FILE+=$IN_PRE_STRING$lib$IN_BAK_STRING
            else
                EXCLUDE_FILE+="*lib"$lib".a "
                INCLUDE_FILE+=$IN_PRE_STRING$lib$IN_BAK_STRING$"\n  "
            fi
        done

        EXCLUDE_FILE=$EX_PRE_STRING$EXCLUDE_FILE$EX_BAK_STRING

        cat $LD_SCRIPT_IN | sed 's/PART_ADDR_REE/'"$ADDRESS"'/g' | sed 's/PART_SIZE_REE/'"$SIZE"'/g' > $LD_SCRIPT_OUT
        cat $LD_SCRIPT_IN | sed 's/EXCLUDE_FILE_USER_DEFINE/'"$EXCLUDE_FILE"'/g' | sed 's/USER_DEFINE_REGION/'"$INCLUDE_FILE"'/g' > $LD_SCRIPT_OUT
    fi
else
##operate the SDRAM LD file
    IN_PRE_STRING="*lib"
    IN_BAK_STRING=".a:(*.text*)"
    VALID_FLAG=`cat $XIP_CONFIG_FILE | grep "LIB_RUN_IN_EXTERNAL_MEDIUM=" | awk -F'LIB_RUN_IN_EXTERNAL_MEDIUM=' '{printf "%s\n", $1}'`
    LIB_FILE=`cat $XIP_CONFIG_FILE | grep "LIB_RUN_IN_EXTERNAL_MEDIUM=" | awk -F'LIB_RUN_IN_EXTERNAL_MEDIUM=' '{printf "%s\n", $2}'`
    LAST_LIB=`echo "$LIB_FILE" | awk -F' ' '{printf "%s\n", $NF}'`

    ##LIB_RUN_IN_EXTERNAL_MEDIUM is invalid
    if [ "$LIB_FILE" = "" ] || [ "$VALID_FLAG" = "#" ]; then
        cat $LD_SCRIPT_IN | sed 's/EXCLUDE_FILE_USER_DEFINE/'"$EXCLUDE_FILE"'/g' > $LD_SCRIPT_OUT
        line=`grep -n "._flash_code" $LD_SCRIPT_OUT | head -1 | cut -d ":" -f 1`
        line_start=`expr $line + 2`
        line_end=`expr $line + 3`
        sed -i -e ''$line_start','$line_end'd' $LD_SCRIPT_OUT
        sed -i "s/ AT > REGION_USERDEFINE//g" $LD_SCRIPT_OUT
        sed -i -e '/USER_DEFINE_REGION/d' $LD_SCRIPT_OUT
        sed -i -e '/\.ram\.code\*/d' $LD_SCRIPT_OUT
    else
    ##LIB_RUN_IN_EXTERNAL_MEDIUM is valid
        for lib in ${LIB_FILE[@]}
        do
            if [ "$LAST_LIB" = "$lib" ]; then
                INCLUDE_FILE+=$IN_PRE_STRING$lib$IN_BAK_STRING
            else
                INCLUDE_FILE+=$IN_PRE_STRING$lib$IN_BAK_STRING$"\n  "
            fi
        done

        cat $LD_SCRIPT_IN | sed 's/PART_ADDR_REE/'"$ADDRESS"'/g' | sed 's/PART_SIZE_REE/'"$SIZE"'/g' > $LD_SCRIPT_OUT
        cat $LD_SCRIPT_IN | sed 's/USER_DEFINE_REGION/'"$INCLUDE_FILE"'/g' > $LD_SCRIPT_OUT
    fi
fi
