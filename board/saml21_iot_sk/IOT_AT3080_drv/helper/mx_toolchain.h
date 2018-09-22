/**
 ******************************************************************************
 * @file    mx_toolchain.h
 * @author  William Xu
 * @version V1.0.0
 * @date    18-Apr-2018
 * @brief   This file provide toolchain independent macros
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#ifndef _MX_TOOLCHAIN_H_
#define _MX_TOOLCHAIN_H_

/** MX_CONCAT
 *  Concatenate tokens together
 *
 *  @note
 *  Expands tokens before concatenation
 *
 *  @code
 *  // Creates a unique label based on the line number
 *  int MX_CONCAT(UNIQUE_LABEL_, __LINE__) = 1;
 *  @endcode
 */
#define MX_CONCAT(a, b) MX_CONCAT_(a, b)
#define MX_CONCAT_(a, b) a##b

/** MX_STRINGIFY
 *  Converts tokens into strings
 *
 *  @note
 *  Expands tokens before stringification
 *
 *  @code
 *  // Creates a string based on the parameters
 *  const char *c = MX_STRINGIFY(This is a ridiculous way to create a string)
 *  @endcode
 */
#define MX_STRINGIFY(a) MX_STRINGIFY_(a)
#define MX_STRINGIFY_(a) #a

// Warning for unsupported compilers
#if !defined(__GNUC__)   /* GCC        */ \
 && !defined(__CC_ARM)   /* ARMCC      */ \
 && !defined(__clang__)  /* LLVM/Clang */ \
 && !defined(__ICCARM__) /* IAR        */ \
 && !defined(__TI_ARM__) /* TI		  */
#warning "This compiler is not yet supported."
#endif


// Attributes

/** MX_PACKED
 *  Pack a structure, preventing any padding from being added between fields.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_PACKED(struct) foo {
 *      char x;
 *      int y;
 *  };
 *  @endcode
 */
#ifndef MX_PACKED
#if defined(__ICCARM__)
#define MX_PACKED(struct) __packed struct
#else
#define MX_PACKED(struct) struct __attribute__((packed))
#endif
#endif

/** MX_ALIGN(N)
 *  Declare a variable to be aligned on an N-byte boundary.
 *
 *  @note
 *  IAR does not support alignment greater than word size on the stack
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_ALIGN(16) char a;
 *  @endcode
 */
#ifndef MX_ALIGN
#if defined(__ICCARM__)
#define MX_ALIGN(N) _Pragma(MX_STRINGIFY(data_alignment=N))
#else
#define MX_ALIGN(N) __attribute__((aligned(N)))
#endif
#endif

/** MX_UNUSED
 *  Declare a function argument to be unused, suppressing compiler warnings
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  void foo(MX_UNUSED int arg) {
 *
 *  }
 *  @endcode
 */
#ifndef MX_UNUSED
#if defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_UNUSED __attribute__((__unused__))
#else
#define MX_UNUSED
#endif
#endif

/** MX_USED
 *  Inform the compiler that a static variable is to be retained in the object file, even if it is unreferenced.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_USED int foo;
 *
 *  @endcode
 */
#ifndef MX_USED
#if defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_USED __attribute__((used))
#elif defined(__ICCARM__)
#define MX_USED __root
#else
#define MX_USED
#endif
#endif

/** MX_WEAK
 *  Mark a function as being weak.
 *
 *  @note
 *  Functions should only be marked as weak in the source file. The header file
 *  should contain a regular function declaration to insure the function is emitted.
 *  A function marked weak will not be emitted if an alternative non-weak
 *  implementation is defined.
 *
 *  @note
 *  Weak functions are not friendly to making code re-usable, as they can only
 *  be overridden once (and if they are multiply overridden the linker will emit
 *  no warning). You should not normally use weak symbols as part of the API to
 *  re-usable modules.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_WEAK void foo() {
 *      // a weak implementation of foo that can be overriden by a definition
 *      // without  __weak
 *  }
 *  @endcode
 */
#ifndef MX_WEAK
#if defined(__ICCARM__)
#define MX_WEAK __weak
#else
#define MX_WEAK __attribute__((weak))
#endif
#endif

/** MX_PURE
 *  Hint to the compiler that a function depends only on parameters
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_PURE int foo(int arg){
 *      // no access to global variables
 *  }
 *  @endcode
 */
#ifndef MX_PURE
#if defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_PURE __attribute__((const))
#else
#define MX_PURE
#endif
#endif

/** MX_NOINLINE
 *  Declare a function that must not be inlined.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_NOINLINE void foo() {
 *
 *  }
 *  @endcode
 */
#ifndef MX_NOINLINE
#if defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_NOINLINE __attribute__((noinline))
#elif defined(__ICCARM__)
#define MX_NOINLINE _Pragma("inline=never")
#else
#define MX_NOINLINE
#endif
#endif

/** MX_FORCEINLINE
 *  Declare a function that must always be inlined. Failure to inline
 *  such a function will result in an error.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_FORCEINLINE void foo() {
 *
 *  }
 *  @endcode
 */
#ifndef MX_FORCEINLINE
#if defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_FORCEINLINE static inline __attribute__((always_inline))
#elif defined(__ICCARM__)
#define MX_FORCEINLINE _Pragma("inline=forced") static
#else
#define MX_FORCEINLINE static inline
#endif
#endif

/** MX_NORETURN
 *  Declare a function that will never return.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_NORETURN void foo() {
 *      // must never return
 *      while (1) {}
 *  }
 *  @endcode
 */
#ifndef MX_NORETURN
#if defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_NORETURN __attribute__((noreturn))
#elif defined(__ICCARM__)
#define MX_NORETURN __noreturn
#else
#define MX_NORETURN
#endif
#endif

/** MX_UNREACHABLE
 *  An unreachable statement. If the statement is reached,
 *  behaviour is undefined. Useful in situations where the compiler
 *  cannot deduce the unreachability of code.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  void foo(int arg) {
 *      switch (arg) {
 *          case 1: return 1;
 *          case 2: return 2;
 *          ...
 *      }
 *      MX_UNREACHABLE;
 *  }
 *  @endcode
 */
#ifndef MX_UNREACHABLE
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_UNREACHABLE __builtin_unreachable()
#else
#define MX_UNREACHABLE while (1)
#endif
#endif

/** MX_DEPRECATED("message string")
 *  Mark a function declaration as deprecated, if it used then a warning will be
 *  issued by the compiler possibly including the provided message. Note that not
 *  all compilers are able to display the message.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_DEPRECATED("don't foo any more, bar instead")
 *  void foo(int arg);
 *  @endcode
 */
#ifndef MX_DEPRECATED
#if defined(__CC_ARM)
#define MX_DEPRECATED(M) __attribute__((deprecated))
#elif defined(__GNUC__) || defined(__clang__) || defined(__TI_ARM__)
#define MX_DEPRECATED(M) __attribute__((deprecated(M)))
#else
#define MX_DEPRECATED(M)
#endif
#endif

/** MX_DEPRECATED_SINCE("version", "message string")
 *  Mark a function declaration as deprecated, noting that the declaration was
 *  deprecated on the specified version. If the function is used then a warning
 *  will be issued by the compiler possibly including the provided message.
 *  Note that not all compilers are able to display this message.
 *
 *  @code
 *  #include "mx_toolchain.h"
 *
 *  MX_DEPRECATED_SINCE("mbed-os-5.1", "don't foo any more, bar instead")
 *  void foo(int arg);
 *  @endcode
 */
#define MX_DEPRECATED_SINCE(D, M) MX_DEPRECATED(M " [since " D "]")

/** MX_CALLER_ADDR()
 * Returns the caller of the current function.
 *
 * @note
 * This macro is only implemented for GCC and ARMCC.
 *
 * @code
 * #include "mx_toolchain.h"
 *
 * printf("This function was called from %p", MX_CALLER_ADDR());
 * @endcode
 *
 * @return Address of the calling function
 */
#ifndef MX_CALLER_ADDR
#if (defined(__GNUC__) || defined(__clang__)) && !defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_CALLER_ADDR() __builtin_extract_return_addr(__builtin_return_address(0))
#elif defined(__CC_ARM)
#define MX_CALLER_ADDR() __builtin_return_address(0)
#else
#define MX_CALLER_ADDR() (NULL)
#endif
#endif

#ifndef MX_SECTION
#if (defined(__GNUC__) || defined(__clang__)) || defined(__CC_ARM) || defined(__TI_ARM__) || defined(__TI_ARM__)
#define MX_SECTION(name) __attribute__ ((section (name)))
#elif defined(__ICCARM__)
#define MX_SECTION(name) _Pragma(MX_STRINGIFY(location=name))
#else
#error "Missing MX_SECTION directive"
#endif
#endif

/**
 * Macro expanding to a string literal of the enclosing function name.
 *
 * The string returned takes into account language specificity and yield human
 * readable content.
 *
 * As an example, if the macro is used within a C++ function then the string
 * literal containing the function name will contain the complete signature of
 * the function - including template parameters - and namespace qualifications.
 */
#ifndef MX_PRETTY_FUNCTION
#define MX_PRETTY_FUNCTION __PRETTY_FUNCTION__
#endif

#ifndef MX_PRINTF
#if defined(__GNUC__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_PRINTF(format_idx, first_param_idx) __attribute__ ((__format__(__printf__, format_idx, first_param_idx)))
#else
#define MX_PRINTF(format_idx, first_param_idx)
#endif
#endif

#ifndef MX_PRINTF_METHOD
#if defined(__GNUC__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_PRINTF_METHOD(format_idx, first_param_idx) __attribute__ ((__format__(__printf__, format_idx+1, first_param_idx+1)))
#else
#define MX_PRINTF_METHOD(format_idx, first_param_idx)
#endif
#endif

#ifndef MX_SCANF
#if defined(__GNUC__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_SCANF(format_idx, first_param_idx) __attribute__ ((__format__(__scanf__, format_idx, first_param_idx)))
#else
#define MX_SCANF(format_idx, first_param_idx)
#endif
#endif

#ifndef MX_SCANF_METHOD
#if defined(__GNUC__) || defined(__CC_ARM) || defined(__TI_ARM__)
#define MX_SCANF_METHOD(format_idx, first_param_idx) __attribute__ ((__format__(__scanf__, format_idx+1, first_param_idx+1)))
#else
#define MX_SCANF_METHOD(format_idx, first_param_idx)
#endif
#endif

// FILEHANDLE declaration
#if defined(TOOLCHAIN_ARM)
#include <rt_sys.h>
#endif

#ifndef FILEHANDLE
typedef int FILEHANDLE;
#endif

// Backwards compatibility
#ifndef WEAK
#define WEAK MX_WEAK
#endif

#ifndef PACKED
#define PACKED MX_PACKED()
#endif

#ifndef EXTERN
#define EXTERN extern
#endif

#ifndef ALWAYS_INLINE
#define ALWAYS_INLINE    __attribute__((always_inline))
#endif

#endif

/** @}*/
/** @}*/
