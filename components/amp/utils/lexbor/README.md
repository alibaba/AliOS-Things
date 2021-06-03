# Lexbor

[![Build Status](https://travis-ci.org/lexbor/lexbor.svg?branch=master)](https://travis-ci.org/lexbor/lexbor)
<a href="http://www.facebook.com/sharer.php?u=https%3A%2F%2Fgithub.com%2Flexbor%2Flexbor" target="_blank"><img alt="" height=20 src="http://lexbor.com/img/facebool_share_button.png"></a>
<a href="https://twitter.com/intent/tweet?text=Development%20of%20an%20open%20source%20HTML%20Renderer%20library...&url=https%3A%2F%2Fgithub.com%2Flexbor%2Flexbor&hashtags=lexbor" target="_blank"><img alt="" height=20 src="http://lexbor.com/img/twitter_share_button.png"></a>

The `lexbor` project is being developed using the `C` language, without dependencies.

## Features
* [Modules](https://github.com/lexbor/lexbor/tree/master/source/lexbor).
* [Single or separate](https://github.com/lexbor/lexbor#single-or-separately) libraries for each module.
* No outside dependencies.
* Easy to port to any platform.
* C99 support.
* Speed.

### HTML Module
* Full conformance with the [HTML5 specification](https://html.spec.whatwg.org/multipage/).
* Manipulation of [elements](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/html/element_create.c) and [attributes](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/html/element_attributes.c): add, change, delete and other.
* Supports fragment parsing (for [innerHTML](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/html/element_innerHTML.c)).
* Supports parsing [by chunks](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/html/document_parse_chunk.c).
* Passes all tree construction tests.
* [Tested](https://github.com/lexbor/warc_test) by 200+ million HTML pages with [ASAN](https://clang.llvm.org/docs/AddressSanitizer.html).
* Two way for parsing HTML: [by Document](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/html/document_parse.c), [by Parser](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/html/parse.c).
* Supports [determining encoding](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/html/encoding.c) by byte stream.
* Fast.

### CSS Module
* Full conformance with the [CSS Syntax](https://drafts.csswg.org/css-syntax-3/) module.
* Supports parsing [by chunks](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/css/syntax/tokenizer_chunks.c).
* Please, see [roadmap](https://lexbor.com/roadmap/#css) of CSS Modules support.

### Encoding Module
* Full conformance with the [Encoding specification](https://encoding.spec.whatwg.org/).
* Supports`40 encodings` for encode/decode.
* Supports [single](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/encoding/single/from_to.c) and [buffering](https://github.com/lexbor/lexbor/blob/master/examples/lexbor/encoding/buffer/from_to.c) encode/decode.
* Fast.

## Build and Installation

### Binary packages

Binaries are available for:

* [CentOS](https://lexbor.com/download/#centos) 6, 7, 8
* [Debian](https://lexbor.com/download/#debian) 8, 9, 10
* [Fedora](https://lexbor.com/download/#fedora) 28, 29, 30, 31
* [RHEL](https://lexbor.com/download/#rhel) 7, 8
* [Ubuntu](https://lexbor.com/download/#ubuntu) 14.04, 16.04, 18.04, 18.10, 19.04, 19.10

Currently for `x86_64` architecture.
If you need any other architecture, please, write to [support@lexbor.com](mailto:support@lexbor.com).

### Source code

For building and installing Lexbor library from source code, use [CMake](https://cmake.org/) (open-source, cross-platform build system).

```bash
cmake . -DLEXBOR_BUILD_TESTS=ON -DLEXBOR_BUILD_EXAMPLES=ON -DLEXBOR_BUILD_SEPARATELY=ON
make
make test
```

Please, see more information in [documentation](https://lexbor.com/docs/lexbor/#source_code).

## Single or separately

### Single
* liblexbor — this is a single library that includes all modules.

### Separately
* liblexbor-{module name} — libraries for each module.

You only need an HTML parser? Use `liblexbor-html`.

Separate modules may depend on each other.
For example, dependencies for `liblexbor-html`: `liblexbor-core`, `liblexbor-dom`, `liblexbor-tag`, `liblexbor-ns`.

The `liblexbor-html` library already contains all the pointers to the required dependencies. Just include it in the assembly: `gcc program.c -llexbor-html`.

## External Bindings and Wrappers

You can create a binding or wrapper for the `lexbor` and place the link here!

## Documentation

Available on [lexbor.com](https://lexbor.com) in [Documentation](https://lexbor.com/docs/lexbor/) section.

## Roadmap

Please, see [roadmap](https://lexbor.com/roadmap/) on [lexbor.com](https://lexbor.com).

## Getting Help

* IRC: [#lexbor on `irc.freenode.net <http://freenode.net>`](http://webchat.freenode.net?channels=%23lexbor)
* E-mail [support@lexbor.com](mailto:support@lexbor.com)

## AUTHOR

Alexander Borisov <borisov@lexbor.com>

## COPYRIGHT AND LICENSE

   Lexbor.

   Copyright 2018-2020 Alexander Borisov

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.


Please, see [LICENSE](https://github.com/lexbor/lexbor/blob/master/LICENSE) file.
