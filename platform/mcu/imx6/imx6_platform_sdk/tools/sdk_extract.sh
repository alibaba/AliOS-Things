#!/bin/bash

# Copyright (c) 2012 Freescale Semiconductor, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# o Redistributions of source code must retain the above copyright notice, this list
#   of conditions and the following disclaimer.
#
# o Redistributions in binary form must reproduce the above copyright notice, this
#   list of conditions and the following disclaimer in the documentation and/or
#   other materials provided with the distribution.
#
# o Neither the name of Freescale Semiconductor, Inc. nor the names of its
#   contributors may be used to endorse or promote products derived from this
#   software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Shell script to perform SDK extraction.
#
# Usage: <this_script> <options> <version>
#
# Note: this script must be run from the SDK root!


print_usage () {
    echo "Usage: $0 [-au] <version>";
    echo "         -a        Archive the extraction in zipfiles.";
    echo "         -u        Only update the destination extraction tree,";
    echo "                   don't delete it first.";
    echo "      <version>    Name of the output directory,"
    echo "                   ie: 4.300.800";
    echo
    echo "Note: this script must be run from the SDK root, not the tools dir!"
}

# check arguments
if [ $# == 0 ]; then
    print_usage
    exit 1
fi

bUpdate=0
bArchive=0

ARCHIVE=a
UPDATE=u

# Memorize the options for later use
orgOpts=$1

while getopts "acu" Option
do
  case $Option in
    a ) bArchive=$ARCHIVE
        ;;
    u ) bUpdate=$UPDATE
        ;;
  esac
done
# Shift out the used-up command-line arguments.
shift $(($OPTIND - 1))

# setup path variables
Version=$1
SdkRootDir=.
ProjectDir=${SdkRootDir}
ReleaseDirName=${Version}_release
ReleaseDir=${ProjectDir}/${ReleaseDirName}
SwDirName=iMX6_Platform_SDK
DestDir=${ReleaseDir}/${SwDirName}
#DestDir=${SwDir}/SOCFirmware
#ExcludesFile=sdk_excludes.tmp
#IncludesFile=sdk_includes.tmp
ExcludesFile=$(mktemp -t sdk_excludes.XXXXXXXXXX)
#IncludesFile=$(mktemp -t sdk_includes.XXXXXXXXXX)

echo SDK Root: ${SdkRootDir}
echo Release directory: ${ReleaseDir}
echo rsync dest directory: ${DestDir}

# ------------------------------------------------------------
# Prepare the release directory.
# ------------------------------------------------------------
if [ $bUpdate == 0 ]; then
    # delete pre-existing release directory
    if [ -e "$ReleaseDir" ]; then
        echo "Removing previous release directory..."
        rm -rf $ReleaseDir

        if [ -e "$ReleaseDir" ]; then
            echo "Failed to delete previous release directory"
            exit 1
        fi
    fi
fi

# create release directory
echo "Creating directories ..."
mkdir -p $DestDir

# ------------------------------------------------------------
#				Excludes
#   + => include
#   - => exclude
# ------------------------------------------------------------

echo "Writing exclusions list to $ExcludesFile..."
cat > "$ExcludesFile" <<***DONE-EXCLUDES***

# Exclude the extraction directory.
#- ${ReleaseDirName}/

# First, explicitly include only the dirs we want.
+ /apps
+ /board
+ /doc
+ /mk
+ /sdk
+ /tools
+ /lwip

# Files in root to include.
+ /Makefile
+ /Doxyfile
+ /README.pdf
+ /LICENSE.txt

# Now exclude all other root files and directories.
- /*

# Remove documentation source files and doxygen output.
- doc/*.docx
- doc/html
- lwip/html

# include prebuilt .o files in the gpu_demo
+ /apps/gpu_demo/src/*.o

# excludes based on file extension
- *.o
- *.map
- *.dnm
- *.dle
- *.dla
- *.siz
- *.d
- *.lst
- *.dba
- *.bak
- *.contrib
- *.keep
- *.elf
- *.1
- *.2
- *.*~
- *.$$$
- .DS_Store
- *.pyc
- *.tmp
- ._*
- *.orig

- .git
- .gitignore

# exclude the output directory
- /output

# Exclude mx53 stuff
- sdma_script_code_mx53.h

# Exclude perfmon
- perfmon_imx.h
- **/perfmon/

# Exclude ports for other chips in lwip contrib
- /lwip/contrib/ports/blackfin
- /lwip/contrib/ports/c674x

# Exclude the USB host stack since it isn't functional yet.
- /sdk/common/usb_stack/Host

# Exclude Silicon Image sources
- sdk/common/hdmi_transmitter

# Exclude some stuff in tools
- tools/sdk_extract.sh
- tools/makesdktags
- tools/doxygen_config/FreescaleNDAfooter.htm


***DONE-EXCLUDES***

# ------------------------------------------------------------
#				Includes
# ------------------------------------------------------------

# echo "Writing inclusions list to $IncludesFile..."
# cat > "$IncludesFile" <<***DONE-INCLUDES***
# 
# #### documentation
# docs
# 
# # Root dir files
# 
# 
# ***DONE-INCLUDES***


# ------------------------------------------------------------
#				Copy
# ------------------------------------------------------------

# copy sources to destination
#
# rsync options are:
# -a  archive, which implies:
#     -l   links
#     -p   preserve permissions
#     -t   preserve times
#     -g   preserve group
#     -o   preserve owner
#     -D   preserve devices
# -r  recursive
# -c  skip based on checksum
# -v  verbose
#
echo "Starting rsync..."
#rsync -v -arcv --ignore-existing --exclude-from=$ExcludesFile --files-from=- $SdkRootDir/ $DestDir/ < $IncludesFile
rsync -v -arcv --ignore-existing --exclude-from=${ExcludesFile} ${SdkRootDir}/ ${DestDir}/

rsync_result=$?
if [ $rsync_result != 0 ]; then
echo "rsync failed with return code ${rsync_result}"
    exit $rsync_result
fi

# ------------------------------------------------------------
#				Archive
# ------------------------------------------------------------

#
# Zip up the release.
#
if [ $bArchive == $ARCHIVE ]; then

	# ------------------------------------------------------------
	#  Archive SDK
	# ------------------------------------------------------------

    pushd $ReleaseDir

    # construct archive file names
    tarball_sdk="platform_sdk_${Version}_release.tgz"
    zip_sdk="platform_sdk_${Version}_release.zip"

    # tar/gz everything
	echo "Archiving release into $tarball_sdk..."
	tar -czf $tarball_sdk $SwDirName

    # zip everything
	echo "Archiving release into $zip_sdk..."
    zip -q9r $zip_sdk $SwDirName

    popd

fi   # if [ $bArchive == $ARCHIVE ]

# ------------------------------------------------------------
#				Script Done
# ------------------------------------------------------------
date
