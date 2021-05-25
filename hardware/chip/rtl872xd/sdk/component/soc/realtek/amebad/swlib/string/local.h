#include <_ansi.h>
//#include <../libc/rom/ctype/local.h>

/* internal function to compute width of wide char. */
int _EXFUN (__wcwidth, (wint_t));

/* Defined in locale/locale.c.  Returns a value != 0 if the current
   language is assumed to use CJK fonts. */
int __locale_cjk_lang (void);

/*
   Taken from glibc:
   Add the compiler optimization to inhibit loop transformation to library
   calls.  This is used to avoid recursive calls in memset and memmove
   default implementations.
*/
#ifdef _HAVE_CC_INHIBIT_LOOP_TO_LIBCALL
# define __inhibit_loop_to_libcall \
  __attribute__ ((__optimize__ ("-fno-tree-loop-distribute-patterns")))
#else
# define __inhibit_loop_to_libcall
#endif


