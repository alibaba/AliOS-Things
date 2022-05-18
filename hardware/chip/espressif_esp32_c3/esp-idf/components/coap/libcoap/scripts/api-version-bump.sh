#! /bin/bash

if ! test -f configure.ac; then
    >&2 echo "$(basename $0): script must be called from the package's top directory"
    exit 1
fi

declare $(sed -ne '/^LIBCOAP_API_VERSION=[0-9]\+/{p; q}' configure.ac)
NEW_VERSION=${1:-$LIBCOAP_API_VERSION}

if test "x$NEW_VERSION" = "x$LIBCOAP_API_VERSION"; then
    >&2 echo "no version change requested, exiting (current version is $LIBCOAP_API_VERSION)"
    exit 1
fi

echo $(basename $0): increase version $LIBCOAP_API_VERSION to $NEW_VERSION

# command for move operations on version-controlled files and directories
MV="git mv"
move() {
    source=$1
    dest=$2
    test -e "$1" && $MV "$1" "$2"
}

move include/coap$LIBCOAP_API_VERSION include/coap$NEW_VERSION
move libcoap-$LIBCOAP_API_VERSION.pc.in libcoap-$NEW_VERSION.pc.in
move libcoap-$LIBCOAP_API_VERSION.map libcoap-$NEW_VERSION.map
move libcoap-$LIBCOAP_API_VERSION.sym libcoap-$NEW_VERSION.sym

# sed pattern for include path prefix substitution
pat='\(#\s*include ["<]coap\)'$LIBCOAP_API_VERSION/

find \( -name \*.h -o -name \*.c \) \
     -exec grep -q "^$pat" {} \; -print | \
    (while read fn ; do test -f ${fn}.in || sed -i "s,^$pat,\1$NEW_VERSION/,"  $fn ; done )

# examples-code-check.c generates new files with include statements
sed -i "s,$pat,\1$NEW_VERSION/," man/examples-code-check.c

pathpat='\(include/coap\)'$LIBCOAP_API_VERSION

# autogen.sh
for fn in autogen.sh .gitignore build-env/Dockerfile.* ; do
    sed -i "s,$pathpat,\1$NEW_VERSION,g" $fn
done

# Adjust LIBCOAP_API_VERSION in CMakeLists.txt
sed -i "s/^\(set(LIBCOAP_API_VERSION \+\)$LIBCOAP_API_VERSION\( *)\)/\1$NEW_VERSION\2/" CMakeLists.txt

# Adjust LibCoAPIncludeDir in win32/libcoap.props
sed -i "s/\(<LibCoAPIncludeDir>include\\\coap\)$LIBCOAP_API_VERSION/\1$NEW_VERSION/" win32/libcoap.props

# Finally, increase LIBCOAP_API_VERSION in configure.ac and re-run autoconf
sed -i "s/^\(LIBCOAP_API_VERSION=\)$LIBCOAP_API_VERSION/\1$NEW_VERSION/" configure.ac && autoreconf
