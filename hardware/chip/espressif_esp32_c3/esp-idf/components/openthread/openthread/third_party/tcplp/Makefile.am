#
#  Copyright (c) 2021, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#

include $(abs_top_nlbuild_autotools_dir)/automake/pre.am

EXTRA_DIST                                    = \
    lib/bitmap.h                                \
    lib/cbuf.h                                  \
    lib/lbuf.h                                  \
    tcplp.h                                     \
    $(NULL)

lib_LIBRARIES                                 = \
    libtcplp.a                                  \
    $(NULL)

# Do not enable -Wsign-compare for TCPlp
override CFLAGS                              := $(filter-out -Wsign-compare,$(CFLAGS))
override CFLAGS                              := $(CFLAGS) -Wno-sign-compare

# Do not enable -Wunused-parameter for TCPlp
override CFLAGS                              := $(filter-out -Wunused-parameter,$(CFLAGS))
override CFLAGS                              := $(CFLAGS) -Wno-unused-parameter

# Do not enable -Waddress-of-packed-member for TCPlp
override CFLAGS                              := $(filter-out -Waddress-of-packed-member,$(CFLAGS))
override CFLAGS                              := $(CFLAGS) -Wno-address-of-packed-member

# Do not enable -pedantic-errors for TCPlp
override CFLAGS                              := $(filter-out -pedantic-errors,$(CFLAGS))

TCPLP_SRCDIR                                  = $(top_srcdir)/third_party/tcplp

libtcplp_a_CPPFLAGS                           = \
    -I$(top_srcdir)/include                     \
    -I$(top_srcdir)/src/core                    \
    -I$(TCPLP_SRCDIR)/bsdtcp                    \
    -I$(TCPLP_SRCDIR)/lib                       \
    $(TCPLP_CPPFLAGS)                           \
    $(NULL)

libtcplp_a_SOURCES                            = \
    lib/bitmap.c                                \
    lib/cbuf.c                                  \
    lib/lbuf.c                                  \
    $(NULL)

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
