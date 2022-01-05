#! /bin/sh

# only continue if `pkgconfig cunit` fails
pkg-config cunit 2>/dev/null && exit 0

# Check if the system has a cunit.pc.
# If not, exit with success (the configure script will raise
# a more descriptive error).
CUNIT_PC=`dpkg -L libcunit1-dev 2>/dev/null|grep cunit.pc`
if test "x$CUNIT_PC" != "x"; then
    grep -q Version: $CUNIT_PC || (dpkg -l|sed -ne 's/^ii\s\+libcunit1-dev\s\+\([0-9]\+.[0-9]\+-[0-9]\+\).*$/Version: \1/p; T; q'|cat - $CUNIT_PC |sed -e 's/={exec_prefix/=${exec_prefix/' >`pwd`/cunit.pc)
fi

# output location of generated cunit.pc
echo PKG_CONFIG_PATH=`pwd`
