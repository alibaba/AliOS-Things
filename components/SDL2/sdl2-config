#!/bin/sh

prefix=/usr/local
exec_prefix=${prefix}
exec_prefix_set=no
libdir=${exec_prefix}/lib

#usage="\
#Usage: $0 [--prefix[=DIR]] [--exec-prefix[=DIR]] [--version] [--cflags] [--libs]"
usage="\
Usage: $0 [--prefix[=DIR]] [--exec-prefix[=DIR]] [--version] [--cflags] [--libs] [--static-libs]"

if test $# -eq 0; then
      echo "${usage}" 1>&2
      exit 1
fi

while test $# -gt 0; do
  case "$1" in
  -*=*) optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
  *) optarg= ;;
  esac

  case $1 in
    --prefix=*)
      prefix=$optarg
      if test $exec_prefix_set = no ; then
        exec_prefix=$optarg
      fi
      ;;
    --prefix)
      echo $prefix
      ;;
    --exec-prefix=*)
      exec_prefix=$optarg
      exec_prefix_set=yes
      ;;
    --exec-prefix)
      echo $exec_prefix
      ;;
    --version)
      echo 2.0.13
      ;;
    --cflags)
      echo -I${prefix}/include/SDL2  -D_REENTRANT
      ;;
    --libs)
      echo -L${exec_prefix}/lib -Wl,-rpath,${libdir} -Wl,--enable-new-dtags -lSDL2
      ;;
    --static-libs)
#    --libs|--static-libs)
      echo -L${exec_prefix}/lib -lSDL2  -Wl,--no-undefined -lm -ldl -lpthread -lrt
      ;;
    *)
      echo "${usage}" 1>&2
      exit 1
      ;;
  esac
  shift
done
