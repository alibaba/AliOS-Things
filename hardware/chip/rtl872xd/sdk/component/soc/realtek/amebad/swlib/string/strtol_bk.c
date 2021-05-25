#include "basic_types.h"
#include <stdarg.h>
#include <stddef.h>             /* Compiler defns such as size_t, NULL etc. */
#include "strproc.h"
#include "section_config.h"
#include "diag.h"
#include "ameba_soc.h"

/**
  * _strtol - convert a string to a signed long
  * @cp: The start of the string
  * @endp: A pointer to the end of the parsed string will be placed here
  * @base: The number base to use
  *
  * This function is obsolete. Please use kstrtol instead.
  */
HAL_ROM_TEXT_SECTION
_LONG_CALL_ long _strtol(const char *cp, char **endp, unsigned int base)
 {
         if (*cp == '-')
                 return -_strtoul(cp + 1, endp, base);
 
         return _strtoul(cp, endp, base);
 }
