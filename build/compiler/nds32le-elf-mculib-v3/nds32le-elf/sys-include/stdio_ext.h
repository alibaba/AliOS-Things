/*
 * stdio_ext.h
 *
 * Definitions for I/O internal operations, originally from Solaris.
 */

#ifndef _STDIO_EXT_H_
#define _STDIO_EXT_H_

#ifdef __rtems__
#error "<stdio_ext.h> not supported"
#endif

#include <stdio.h>

_BEGIN_STD_C

void	 _EXFUN(__fpurge,(FILE *));

/* TODO:

   void _flushlbf (void);
   int __fsetlocking (FILE *__fp, int __type);
*/

#ifdef  __GNUC__

_ELIDABLE_INLINE inline size_t
__fbufsize (FILE *__fp) { return (size_t) __fp->_bf._size; }

_ELIDABLE_INLINE inline int
__freading (FILE *__fp) { return (__fp->_flags & __SRD) != 0; }

_ELIDABLE_INLINE inline int
__fwriting (FILE *__fp) { return (__fp->_flags & __SWR) != 0; }

_ELIDABLE_INLINE inline int
__freadable (FILE *__fp) { return (__fp->_flags & (__SRD | __SRW)) != 0; }

_ELIDABLE_INLINE inline int
__fwriteable (FILE *__fp) { return (__fp->_flags & (__SWR | __SRW)) != 0; }

_ELIDABLE_INLINE inline int
__flbf (FILE *__fp) { return (__fp->_flags & __SLBF) != 0; }

_ELIDABLE_INLINE inline size_t
__fpending (FILE *__fp) { return __fp->_p - __fp->_bf._base; }

#else

#define __fbufsize(__fp) ((size_t) (__fp)->_bf._size)
#define __freading(__fp) (((__fp)->_flags & __SRD) != 0)
#define __fwriting(__fp) (((__fp)->_flags & __SWR) != 0)
#define __freadable(__fp) (((__fp)->_flags & (__SRD | __SRW)) != 0)
#define __fwriteable(__fp) (((__fp)->_flags & (__SWR | __SRW)) != 0)
#define __flbf(__fp) (((__fp)->_flags & __SLBF) != 0)
#define __fpending(__fp) ((size_t) ((__fp)->_p - (__fp)->_bf._base))

#endif /* __GNUC__ */

_END_STD_C

#endif /* _STDIO_EXT_H_ */
