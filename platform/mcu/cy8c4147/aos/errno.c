
#include "errno.h"

static int errno_main;

void
set_errno(int err_code)
{
    errno_main = err_code;
}
