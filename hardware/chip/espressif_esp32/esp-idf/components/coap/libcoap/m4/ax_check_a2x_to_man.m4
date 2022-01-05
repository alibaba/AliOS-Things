#
# SYNOPSIS
#
#   AX_CHECK_A2X_TO_MANPAGE([action-if-positive [, action-if-negative]])
#
# DESCRIPTION
#
#   This macro checks if an installed a2x can be used to create manual
#   pages.
#
#   Examples:
#
#     AX_CHECK_A2X_TO_MANPAGE
#     or
#     AX_CHECK_A2X_TO_MANPAGE([], [AC_MSG_ERROR([docbook-xml is missing])])
#
# LICENSE
#
#   Copyright (c) 2018 Olaf Bergmann <bergmann@tzi.org>
#
#   This file is part of the CoAP library libcoap. Please see README for terms
#   of use.

AC_DEFUN([AX_CHECK_A2X_TO_MANPAGE],
[
    AC_CACHE_CHECK([manpage creation with a2x], [ac_cv_a2x_man],
    [
            if test "x$A2X" = "x"; then
                 AC_MSG_NOTICE([a2x not in path, skipping test])
                 ac_cv_a2x_man=no
            else
                cat <<EOF >conftestman.txt
foo(7)
======

NAME
----
foo - manual

AUTHORS
-------
author
EOF
             $A2X --verbose --doctype manpage --format manpage conftestman.txt >conftest.out 2>&1
             if test "$?" = 0; then
                ac_cv_a2x_man=yes
                $1
             else
                ac_cv_a2x_man=no
                cat conftest.out >&AS_MESSAGE_LOG_FD 2>&1
                $2
             fi
             rm -f conftestman.txt conftest.out foo.7
        fi
    ])
    HAVE_A2X_MAN="$ac_cv_a2x_man"
])
