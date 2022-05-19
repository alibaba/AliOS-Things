# OpenThread Coding Conventions and Style

- [1 C and C++](#c-and-c)
  - [1.1 Standards](#standards)
  - [1.2 Conventions and Best Practices](#conventions-and-best-practices)
  - [1.3 Tightly-constrained Systems and Shared Infrastructure](#tightly-constrained-systems-and-shared-infrastructure)
  - [1.4 Format and Style](#format-and-style)
  - [1.5 Comments](#comments)
- [2 Python](#python)
  - [2.1 Standards](#standards)
  - [2.2 Conventions and Best Practices](#conventions-and-best-practices)
  - [2.3 Format and Style](#format-and-style)

# C and C++

## Standards

- C
  - OpenThread uses and enforces the ISO9899:1999 (aka ISO C99, C99) C language standard as the minimum.
- C++
  - OpenThread uses and enforces the ISO14882:2011 (aka ISO C++11, C++11) C++ language standard as the minimum.
- Extensions
  - Wherever possible, toolchain-specific (e.g GCC/GNU) extensions or the use of later standards shall be avoided or shall be leveraged through toolchain-compatibility preprocessor macros.

## Conventions and Best Practices

### Language Independent

- Inline functions should be used judiciously.
  - The use of code in headers and, more specifically, the use of the non-local scope inline functions should be avoided. Exception: Simple setters and getters are fine since the compiler can efficiently optimize these and make their overhead as low as a direct data member access.
- Return Statements
  - There should be one return statement per free function or method at the end of the free function or method.
- Non-local Goto
  - There should be no calls to the functions `setjmp` or `longjmp`.
- Local Goto
  - There should be no calls to the C/C++ keyword goto. Exception: The use of local gotos for the purposes of common error handling blocks and single points of function return at the bottom of a function.
- C Preprocessor
  - Use of the C preprocessor should be limited to file inclusion and simple macros.
  - Macros shall not be defined within a function or a block and should be defined at the top of a file.
  - All `#else`, `#elif`, and `#endif` preprocessor directives shall reside in the same file as the `#if` or `#ifdef` directive to which they are related.
  - All `#endif` directives equal to or greater than 20 lines away from the `#if` or `#ifdef` directive to which they are related shall be decorated by language comment indicating the conditional they are associated with.
  - Preprocessor `#include` directives in a file shall only be preceded by other preprocessor directives or comments.
  - Preprocessor `#include` directives shall use brace (“<”) and (“>”) style for all public headers, including C and C++ standard library, or other first- and third-party public library headers.
  - Preprocessor `#include` directives should use double quote (‘“‘) and (‘“‘) style for all private or relative headers.
  - Preprocessor `#include` directives should be grouped, ordered, or sorted as follows:
    - If the unit is a core/private header file, `"openthread-core-config.h"` should be the first header file included.
    - If the unit is a core/private `.c` or `.cpp` file:
      - If the unit has a corresponding header file, the unit's corresponding header file should be included before any other header file.
      - If the unit has no corresponding header file, then it should directly include `"openthread-core-config.h"` before any other header file.
    - C++ Standard Library headers
    - C Standard Library headers
    - Third-party library headers
    - First-party library headers
    - Private or local headers
    - Alphanumeric order within each subgroup
  - The preprocessor shall not be used to redefine reserved language keywords.
  - Unused code shall not be disabled by commenting it out with C- or C++-style comments or with preprocessor `#if 0 ... #endif` semantics.
  - Use of the preprocessor token concatenation operator '##' should be avoided.
  - The `undef` preprocessor directive should be avoided and shall never be used to undefine a symbol from a foreign module.
- Object Scope
  - Data objects shall be declared at the smallest possible level of scope.
  - No declaration in an inner scope shall hide or shadow a declaration in an outer scope. Compiler flags shall be set to flag and enforce this.
- Unbounded Recursion
  - There shall be no direct or indirect use of unbounded recursive function calls.
- Symmetric APIs
  - Wherever possible and appropriate, particularly around the management of resources, APIs should be symmetric. For example, if there is a free function or object method that allocates a resource, then there should be one that deallocates it. If there is a free function or object method that opens a file or network stream, then there should be one that closes it.
- Use C stdint.h or C++ cstdint for Plain Old Data Types
  - Standard, scalar data types defined in stdint.h (C) or cstdint (C++) should be used for basic signed and unsigned integer types, especially when size and serialization to non-volatile storage or across a network is concerned. Examples of these are: `uint8_t`, `int8_t`, etc.
- Constant Qualifiers
  - Read-only methods, global variables, stack variables, or data members are read-only should be qualified using the C or C++ `const` qualifier.
  - Pointers or references to read-only objects or storage, including but not limited to function parameters, should be qualified using the C or C++ `const` qualifier.
- Header Include Guard
  - All C and C++ headers shall use preprocessor header include guards.
  - The terminating endif preprocessor directive shall have a comment, C or C++ depending on the header type, containing the preprocessor symbol introduced by the ifndef directive starting the guard.
  - The symbol used for the guard should be the file name, converted to all uppercase, with any spaces (“ “) or dots (“.”) converted to underscores (“\_”).
- Function and Method Prototypes
  - All void functions or methods shall explicitly declare and specify the void type keyword.
- Unused parameters
  - All unused parameters shall be declared as such using the `OT_UNUSED_VARIABLE` macro at the top of a function or method before all local variable declarations.

### C

- C / C++ Linkage Wrappers
  - All header files intended to have C symbol linkage shall use “extern C” linkage wrappers.

### C++

- Prefer Passing Parameters by Reference to Pointer
  - Unlike C, C++ offers an alternate way to alias data over and above a pointer, the reference, indicated by the & symbol. Where appropriate, the reference should be preferred to the pointer.
- Passing Base Scalars
  - Size- and call frequency-based considerations should be made when passing scalars as to whether they should be passed by value or by constant reference; however, pass-by-value should generally be preferred.
- Eliminate Unnecessary Destructors
  - The creation of empty or useless destructors should be avoided. Empty or useless destructors should be removed.
- Default Parameters
  - When you declare C++ free functions and object methods, you should avoid or minimize using default parameters.
  - When you declare C++ virtual object methods, you shall avoid using default parameters.
- Global and Scoped Static Construction
  - There shall be no use of global, static or otherwise, object construction. The use of scoped static object construction should be avoided.
- C++-style Casts
  - Wherever possible and practical, C++ style casts should be used and preferred to the C style cast equivalent.
- Avoid `using namespace` Statements in Headers
  - The C++ `using namespace` statement should not be used outside of object scope inside header files.

## Tightly-constrained Systems and Shared Infrastructure

- Heap-based resource allocation should be avoided.
- There shall be no direct or indirect use of recursive function calls.
- The use of virtual functions should be avoided.
- The use of the C++ Standard Library shall be avoided.
- The use of the C++ Standard Template Library (STL) should be avoided or minimized.
- The use of the C++ templates should be avoided or minimized.
- Code shall not use exceptions.
- Code shall not use C++ runtime type information (RTTI), including facilities that rely upon it, such as `dynamic_cast` and `typeid`.

## Format and Style

- OpenThread uses `script/make-pretty` to reformat code and enforce code format and style. `script/make-pretty check` build target is included in OpenThread's continuous integration and must pass before a pull request is merged.

- `script/make-pretty` requires [clang-format v9.0.0](https://releases.llvm.org/download.html#9.0.0) for C/C++ and [yapf v0.31.0](https://github.com/google/yapf) for Python.

### File Names

- File names should match the names and types of what is described in the file. If a file contains many declarations and definitions, the author should choose the one that predominantly describes or that makes the most sense.
- File contents and names should be limited in the scope of what they contain. It may also be possible that there is too much stuff in one file and you need to break it up into multiple files.
- File names should be all lower case.
- File extensions shall be indicative and appropriate for the type and usage of the source or header file.

### Naming

- Names should be descriptive but not overly so and they should give some idea of scope and should be selected such that _wrong code looks wrong_.
- Names shall not give any idea of type, such as is done with System Hungarian notation.
- Case
  - C preprocessor symbols should be all uppercase.
  - All OpenThread class, namespace, structure, method, function, enumeration, and type names in the C/C++ language shall be in _upper camel case_. Exception: the top level OpenThread namespace 'ot'.
  - All OpenThread instantiated names of instances of classes, namespaces, structures, methods, functions, enumerations, and types as well as method and function parameters in the C++ language shall be in _lower camel case_.
- Symbol Qualification
  - All OpenThread C public data types and free functions should have `ot` prepended to their name.
  - All OpenThread C++ code should be in the ‘ot’ top-level namespace.
- Scope
  - All global data shall have a `g` prepended to the name to denote global scope.
  - All static data shall have a `s` prepended to the name to denote static scope.
  - All class or structure data members shall have a `m` prepended to the name to denote member scope.
  - All free function or method parameters should have an `a` prepended to the name to denote function parameter scope.
  - All variables that do not have such prefixes shall be assumed to be function local scope.

### White Space

- Indentation shall be 4 space characters.
- Conditionals shall always appear on a separate line from the code to execute as a result of the condition.
- Scoped Variable declarations
  - All scoped (i.e. stack) variable declarations should be placed together at the top of the enclosing scope in which they are used.
  - There shall be an empty line after all such variable declarations.
  - The names of all variable declarations should be left aligned.
- Data Member declarations
  - All data member declarations should be placed together.
  - The names of all data member declarations should be left aligned.
  - The data member declarations for C++ classes should be placed at the end or tail of the class.
- Braces
  - Braces should go on their own lines.
  - Statements should never be on the same line following a closing brace.
- Keywords
  - There should be a single space after language-reserved keywords (for, while, if, etc).

## Comments

- All code should use Doxygen to:
  - Detail what the various source and header files are and how they fit into the broader context.
  - Detail what the various C++ namespaces are.
  - Detail what the constants, C preprocessor definitions, and enumerations are.
  - Detail what the globals are and how they are to be used.
  - Detail what the free function and object / class methods are and how they are to be used, what their parameters are, and what their return values are.
  - Detail any other important technical information or theory of operation unique and relevant to the stack that is not otherwise captured in architecture, design, or protocol documentation.
- Every public, and ideally private, free function and class method should likewise have a prologue comment that:
  - Briefly describes what it is and what it does.
  - Describes in detail, optionally, what it is and what it does.
  - Describes the purpose, function, and influence of each parameter as well as whether it is an input, an output, or both.
  - Describes the return value, if present, and the expected range or constraints of it.

# Python

## Standards

- OpenThread uses and enforces Python 3.

## Conventions and Best Practices

- Run `pylint` over your code. `pylint` is a tool for finding bugs and style problems in Python source code. It finds problems that are typically caught by a compiler for less dynamic languages like C and C++. Because of the dynamic nature of Python, some warnings may be incorrect; however, spurious warnings should be fairly infrequent.

## Format and Style

- All code should adhere to [Google Python Style Guide](https://google.github.io/styleguide/pyguide.html) except maximum line length being 119.
