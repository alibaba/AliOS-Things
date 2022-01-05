#! /bin/sh

SILENT="--disable-silent-rules"

if test "x$TESTS" = "xyes" -o "x$TESTS" = "xtrue" ; then
    WITH_TESTS="`scripts/fix-cunit.sh` --enable-tests"
    test -f `pwd`/cunit.pc && echo cat `pwd`/cunit.pc
fi

TEST_LD_LIBRARY_PATH=
case "x${TLS}" in
    xno)       WITH_TLS="--disable-dtls"
               ;;
    xopenssl)  WITH_TLS="--with-openssl"
               ;;
    xgnutls)   WITH_TLS="--with-gnutls"
               ;;
    xtinydtls) WITH_TLS="--with-tinydtls"
               # Need this as libtinydtls.so has not been installed
               # as a part of the travis build
               TEST_LD_LIBRARY_PATH="ext/tinydtls"
               ;;
    *)         WITH_TLS="--with-gnutls"
               ;;
esac

case "x${DOCS}" in
    xyes)      WITH_DOCS="--enable-documentation"
               ;;
    *)         WITH_DOCS="--disable-documentation"
               ;;
esac

config() {
    echo "./configure $SILENT $*"
    ./configure $SILENT $* || cat config.log
}

case "${PLATFORM}" in
    contiki) config "--disable-tests $WITH_DOCS --disable-examples $WITH_TLS" && \
               make -C examples/contiki
             ;;
    lwip)    config "--disable-tests $WITH_DOCS --disable-examples $WITH_TLS" && \
               make -C examples/lwip
             ;;
    posix|*) config "$WITH_TESTS $WITH_DOCS --enable-examples $WITH_TLS" && \
               make && make check
             ;;
esac

err=$?
if test $err = 0 -a -n "$WITH_TESTS" ; then
    EXEC_FILE=tests/testdriver
    # then run valgrind on the actual executable
    LD_LIBRARY_PATH=$TEST_LD_LIBRARY_PATH libtool --mode=execute valgrind --track-origins=yes --leak-check=yes --show-reachable=yes --error-exitcode=123 --quiet --suppressions=tests/valgrind_suppression $EXEC_FILE
    err=$?
fi

# invoke OSS-Fuzz syntax checks
if test $err = 0 -a -n "$WITH_TESTS" ; then
    make -C tests/oss-fuzz -f Makefile.ci check clean
    err=$?
fi

exit $err
