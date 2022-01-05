#!/bin/sh -e

# uncomment the set command for debugging
#set -x

# function to check for needed helper tools
check_helper() {
#echo "Checking for $1 ..."
TOOL=`which "$1" || echo none`

if [ "$TOOL" = "none" ]; then
    echo
    echo "Couldn't find '$1'!"
    RET=1
else
    RET=0
fi
}

PROJECT="libcoap"

AUTOGEN_FILES="
INSTALL
aclocal.m4 ar-lib
coap_config.h coap_config.h.in* compile config.guess config.h* config.log config.status config.sub configure
depcomp
doc/Doxyfile doc/doxyfile.stamp doc/doxygen_sqlite3.db doc/Makefile doc/Makefile.in
examples/*.o  examples/coap-client examples/coap-server examples/coap-rd
examples/Makefile examples/Makefile.in
include/coap2/coap.h
install-sh
libcoap-*.pc libtool ltmain.sh
man/coap*.[357] man/coap*.txt man/Makefile man/Makefile.in
missing
Makefile Makefile.in
stamp-h1 src/.dirstamp libcoap*.la* src/*.*o
tests/*.o tests/Makefile tests/Makefile.in tests/testdriver
tests/oss-fuzz/Makefile.ci
m4/libtool.m4 m4/lt~obsolete.m4 m4/ltoptions.m4 m4/ltsugar.m4 m4/ltversion.m4
"

AUTOGEN_DIRS="
.deps
.libs autom4te.cache/
doc/html/
examples/.deps/ examples/.libs
src/.deps/ src/.libs/
tests/.deps/
"

# checking for cleaner argument
echo
if [ "$1" = "--clean" ]; then
    echo "removing autogerated files ..."
    rm -rf $AUTOGEN_FILES $AUTOGEN_DIRS
    echo "done"
    exit
else
    echo "[HINT] You can run 'autogen.sh --clean' to remove all generated files by the autotools."
    echo
fi

# checking for autoreconf
check_helper autoconf
if [ "$RET" = "1" ]; then
    echo "You probably need to install the package 'autoconf'."
    ERROR=1
else
    echo "Found 'autoconf'."
fi

# checking for aclocal
check_helper aclocal
if [ "$RET" = "1" ]; then
    echo "You probably need to install the package 'automake'."
    ERROR=1
else
    echo "Found 'aclocal'."
fi

# checking for pkg-config
check_helper pkg-config
if [ "$RET" = "1" ]; then
    echo "You probably need to install the package 'pkg-config|pkgconf'."
    ERROR=1
else
    echo "Found 'pkg-config'."
fi

# checking for libtool
# The libtool helper maybe installed as 'libtoolize', checking for 'libtool' first.
check_helper libtool
if [ "$RET" = "1" ]; then
    # O.k. libtool not found, searching for libtoolize.
    check_helper libtoolize
    if [ "$RET" = "1" ]; then
        echo "You probably need to install the package 'libtool'."
        # That's bad, we found nothing!
        ERROR=1
    else
        echo "Found 'libtoolize'."
        break
    fi
else
    echo "Found 'libtool'."
fi

# exit if one tool isn't available
if [ "$ERROR" = "1" ]; then
    echo
    echo "One or more needed tools are missing, exiting ..."
    echo "Please install the needed software packages and restart 'autogen.sh' again."
    echo
    exit 1
fi

echo
echo "  --->  Found all needed tools! That's fine."
echo

# countinue otherwise
test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

# Creating the directory m4 before calling autoreconf to
# not struggle with old versions of aclocal.
mkdir -p $srcdir/m4

echo "Generating needed autotools files for $PROJECT by running autoreconf ..."
autoreconf --force --install --verbose "$srcdir"

echo
echo "You can now run 'configure --help' to see possible configuration options."
echo "Otherwise process the configure script to create the makefiles and generated helper files."
echo
