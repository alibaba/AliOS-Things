# libqrencode 4.0.2 - QR Code encoding library [![Build Status](https://travis-ci.org/fukuchi/libqrencode.png?branch=4.0)](https://travis-ci.org/fukuchi/libqrencode)

**Attention:** This repository contains the development version of libqrencode. See <https://fukuchi.org/works/qrencode/> for the official stable releases. At this moment, the latest stable release is version 4.0.2.

GENERAL INFORMATION
===================
Libqrencode is a library for encoding data in a QR Code symbol, a 2D symbology
that can be scanned by handy terminals such as a mobile phone with CCD. The
capacity of QR Code is up to 7000 digits or 4000 characters and has high
robustness.

Libqrencode accepts a string or a list of data chunks then encodes in a QR Code
symbol as a bitmap array. While other QR Code applications generate an image
file, using libqrencode allows applications to render QR Code symbols from raw
bitmap data directly. This library also contains a command-line utility outputs
a QR Code symbol as a PNG image.


SPECIFICATION
=============
Libqrencode supports QR Code model 2, described in JIS (Japanese Industrial
Standards) X0510:2004 or ISO/IEC 18004. Most of features in the specification
are implemented such as:

- Numeric, alphabet, Japanese kanji (Shift-JIS) or any 8 bit code can be
  embedded
- Optimized encoding of a string
- Structured-append of symbols
- Micro QR Code (experimental)

Currently the following features are not supported:

- ECI and FNC1 mode
- QR Code model 1 (deprecated)


INSTALL
=======

Requirements
------------
While the command-line utility and some test programs use libpng or SDL 2.0,
the libqrencode library itself has no dependencies. You can skip compiling
tests and/or tools if you want not to install programs using SDL or PNG.

Compile & install
-----------------
If there is no "configure" script in the source code directory, run
"autogen.sh" at first to generate it - this is mandatory if you downloaded the
source from GitHub. Some additional software is needed to complete this
process. For example, in Ubuntu, the following packages are needed:

- autoconf
- automake
- autotools-dev
- libtool
- pkg-config
- libpng12-dev

You can skip this process if you have "configure" script already (typically
when you downloaded the source tarball from fukuchi.org.)

Now you are ready to compile the library and tool. Type the following commands:

```
./configure
make
sudo make install
sudo ldconfig
```

This compiles and installs the library and header file to the appropriate
directories: by default, /usr/local/lib and /usr/local/include. You can change
the destination directory by passing some options to the configure script.
Run "./configure --help" to see the list of options.

It also installs a command line tool "qrencode" to /usr/local/bin. If you want
not to build it, give "--without-tools" option to the configure script.

If the configure script does not work well, try to use CMake.

```
cmake .
make
```

When you want to build the test programs, give "--with-tests" option to
configure, or "-DWITH\_TESTS=YES" to cmake.


USAGE
=====
Basic usages of this library are written in the header file (qrencode.h).
You can generate a manual of the library by using Doxygen.


WARNINGS
========
The library is distributed WITHOUT ANY WARRANTY.

Micro QR Code support is EXPERIMENTAL.

Be careful to use the command line tool (qrencode) if it is used by a web
application (e.g. CGI script). For example, giving "-s" option with a large
number to qrencode may cause DoS. The parameters should be checked by the
application.


LICENSING INFORMATION
=====================
Copyright (C) 2006-2017 Kentaro Fukuchi

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA 02110-1301 USA


CONTACT
=======
Visit the homepage at:

https://fukuchi.org/works/qrencode/

for new releases. The git repository is available at:

https://github.com/fukuchi/libqrencode

Please mail any bug reports, suggestions, comments, and questions to:

Kentaro Fukuchi <kentaro@fukuchi.org>

or submit issues to:

https://github.com/fukuchi/libqrencode/issues


ACKNOWLEDGMENTS
===============
QR Code is registered trademarks of DENSO WAVE INCORPORATED in JAPAN and other
countries.

Reed-Solomon encoder included in this library is originally taken from FEC
library developed by Phil Karn (KA9Q) and distributed under the terms of the
GNU LGPL, then rewritten by Kentaro Fukuchi.
Copyright (C) 2002, 2003, 2004, 2006 Phil Karn, KA9Q

* NANKI Haruo           - improved lower-case characters encoding
* Katsumi Saito         - SPEC file
* Philippe Delcroix     - improved mask evaluation
* Yusuke Mihara         - structured-append support
* David Dahl            - DPI and SVG support patch
* Adam Shepherd         - bug fix patch of the mask evaluation
* Josef Eisl (@zapster) - EPS support patch
* Colin (@moshen)       - ANSI support patch
* Ralf Ertzinger        - ASCII support patch
* Yutaka Niibe (@gniibe)- various bug fix patches
* Dan Storm (@Repox)    - SVG support patch
* Lennart Poettering (@mezcalero)
                        - improved text art patch
* Yann Droneaud         - improved input validation patch
* Viona                 - bug fix patch for string splitting
* Daniel Dörrhöfer (@d4ndo)
                        - RLE option, some bug fixes, Travis configuration
* Greg Hart             - PNG32 support patch
* @siggi-heltau         - bug fix patch
* Tobias Klauser (@tklauser)
                        - bug fix patch, XPM support patch
* Robert Petersen (@ripetersen)
                        - added ability to read input data from a file
* @Oblomov              - improved SVG support patch
* @mgorny               - reverse mappings of UTF8 and ANSIUTF8, build script
                          fixes
* @EckoEdc              - MinGW support patch
* Sebastian Buchwald (@UniQP)
                        - Various code cleanups
* André Klitzing (@misery)
                        - CMake support
* Alexey Nikolaev (@aleksey-nikolaev)
                        - improved CMake support
* Vilppu Vuorinen (@vilppuvuorinen)
                        - improved CMake support
* @vanillahsu           - bug fix patch
* @Ation                - bug fix patch
* Shigeyuki Hirai, Paul Janssens, wangsai, Gavan Fantom, Matthew Baker,
  Rob Ryan, Fred Steinhaeuser, Terry Burton, @chisj, @vlad417, Petr,
  Hassan Hajji, Emmanuel Blot, ßlúèÇhîp, Heiko Becker, Gavin Andresen,
  David Binderman, @ralgozino, Sean McMurray, Vlad Bespalov (@win32asm),
  Antenore Gatta, Yoshimichi Inoue, Sunil Maganally, Norman Gray,
  Danomi Manchego, @minus7, Ian Sweet, @qianchenglenger, Ronald Michaels,
  Yuji Ueno, Jakub Wilk, @KangLin, @c-273, @thebunnyrules
                        - bug report / suggestion / typo fixes
