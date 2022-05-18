#
#  Copyright (c) 2016, The OpenThread Authors.
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

#----------------------------------------
#
# This library on the face of it, appears to be identical
# for both the MTD and FTD variants, however ...
#
# The source code here includes numerous OpenThread internal headers.
# Due to the "domino-effect" other internal headers are included.
#
# For example:
#     cli.cpp includes:
#            src/core/common/instance.hpp
# Which Includes:
#            src/core/therad/thread_netif.hpp
# Which Includes:
#            src/core/meshcop/dataset_manager.hpp
# Which Includes:
#            src/core/threadnetwork_data_leader.hpp
#
# That header (and others) is either an MTD or FTD class flavor.
#
# The FTD flavor has many private components (class variables).
# The MTD flavor has no private components.
#
# Bottom line: The Class(structs) are thus different in the downstream
# libs. At this level (in the CLI, and likewise in the NCP) they are
# functionally identical.
#
# ORIGINALLY (historical note about how things are/where built):
#
#     The difference between MTD and FTD was a define..  -DOPENTHREAD_MTD
#     There was no "FTD" define, it is/was assumed that FTD is the default.
#
#     Historically this library -DOPENTHREAD_MTD was not defined/set.
#     (it is set via a commandline define in 'lib-openthread')
#
#     Thus, the existing(previous) way *THIS* library was compiled is
#     exactly the FTD path. Meaning the "cli" library always sees
#     the FTD varients of various headers/classes.
#
#     The same is true of the "ncp" library.
#
# HOWEVER there are two varients of the CLI application, CLI-MTD
# and CLI-FTD (and likewise, two varients of the ncp application)
# These applications link against two different OpenThread libraries.
#
# Which flavor, you get depends upon which library: "mtd" or "ftd" is linked.
#
# Which on the surface appear to link fine against the MTD/FTD library.
#
# In this description/example we focus on the  "nework_data_leader"
# header file. The FTD varient has many private variables, functions
# and other things of "FTD" (ie: full) implementation items.
#
# In contrast the MTD is generaly stubbed out with stub-functions
# inlined in the header that return "error not implemented" or similar.
#
# Thus it works... here ... With this file and this example.
#
# The unknown:
#    What about the other files?
#    What about the other c++ classes?
#    Is this true always? Is this robust?
#    Or is there a hidden "got-ya" that will snag the next person?
#
# This also fails static analisys, checks.
#    Application - with MTD vrs FTD class.
#    Library #1  (cli-lib) with FTD selected.
#    Library #2  (openthread) with two different class flavors.
#
# The static analisys tools will say: "NOPE" different classes!
# Perhaps this will change if/when nothing is implemented in the 'mtd-header'
#
# Additionally, tools that perform "whole program optimization" will
# throw errors becuase the data structures differ greatly.
#
# Hence, CLI library (and NCP) must exist in two flavors.
#
# Unless and until these libraries do not "accidently" suck in
# a "flavored" header file somewhere.

lib_LIBRARIES                       = $(NULL)

if OPENTHREAD_ENABLE_FTD
lib_LIBRARIES                      += libopenthread-cli-ftd.a
endif

if OPENTHREAD_ENABLE_MTD
lib_LIBRARIES                      += libopenthread-cli-mtd.a
endif

if OPENTHREAD_ENABLE_RADIO_CLI
lib_LIBRARIES                      += libopenthread-cli-radio.a
endif

CPPFLAGS_COMMON =                     \
    -I$(top_srcdir)/include           \
    -I$(top_srcdir)/src               \
    -I$(top_srcdir)/src/core          \
    $(OPENTHREAD_TARGET_DEFINES)      \
    $(NULL)

libopenthread_cli_ftd_a_CPPFLAGS =    \
    -DOPENTHREAD_MTD=0                \
    -DOPENTHREAD_FTD=1                \
    -DOPENTHREAD_RADIO=0              \
    $(CPPFLAGS_COMMON)                \
    $(NULL)

libopenthread_cli_mtd_a_CPPFLAGS =    \
    -DOPENTHREAD_MTD=1                \
    -DOPENTHREAD_FTD=0                \
    -DOPENTHREAD_RADIO=0              \
    $(CPPFLAGS_COMMON)                \
    $(NULL)

libopenthread_cli_radio_a_CPPFLAGS =  \
    -DOPENTHREAD_MTD=0                \
    -DOPENTHREAD_FTD=0                \
    -DOPENTHREAD_RADIO=1              \
    -DOPENTHREAD_RADIO_CLI=1          \
    $(CPPFLAGS_COMMON)                \
    $(NULL)

SOURCES_COMMON =                      \
    cli.cpp                           \
    cli_coap.cpp                      \
    cli_coap_secure.cpp               \
    cli_commissioner.cpp              \
    cli_dataset.cpp                   \
    cli_history.cpp                   \
    cli_joiner.cpp                    \
    cli_network_data.cpp              \
    cli_output.cpp                    \
    cli_srp_client.cpp                \
    cli_srp_server.cpp                \
    cli_tcp.cpp                       \
    cli_udp.cpp                       \
    $(NULL)

libopenthread_cli_ftd_a_SOURCES =     \
    $(SOURCES_COMMON)                 \
    $(NULL)

libopenthread_cli_mtd_a_SOURCES =     \
    $(SOURCES_COMMON)                 \
    $(NULL)

libopenthread_cli_radio_a_SOURCES =   \
    cli.cpp                           \
    cli_output.cpp                    \
    $(NULL)

noinst_HEADERS                      = \
    cli.hpp                           \
    cli_coap.hpp                      \
    cli_coap_secure.hpp               \
    cli_commissioner.hpp              \
    cli_config.h                      \
    cli_dataset.hpp                   \
    cli_history.hpp                   \
    cli_joiner.hpp                    \
    cli_network_data.hpp              \
    cli_output.hpp                    \
    cli_srp_client.hpp                \
    cli_srp_server.hpp                \
    cli_tcp.hpp                       \
    cli_udp.hpp                       \
    x509_cert_key.hpp                 \
    $(NULL)

if OPENTHREAD_BUILD_COVERAGE
CLEANFILES                          = $(wildcard *.gcda *.gcno)
endif # OPENTHREAD_BUILD_COVERAGE

include $(abs_top_nlbuild_autotools_dir)/automake/post.am
