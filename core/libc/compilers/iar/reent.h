/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

/* This header file provides the reentrancy.  */

/* The reentrant system calls here serve two purposes:

   1) Provide reentrant versions of the system calls the ANSI C library
      requires.
   2) Provide these system calls in a namespace clean way.

   It is intended that *all* system calls that the ANSI C library needs
   be declared here.  It documents them all in one place.  All library access
   to the system is via some form of these functions.

   The target may provide the needed syscalls by any of the following:

   1) Define the reentrant versions of the syscalls directly.
      (eg: _open_r, _close_r, etc.).  Please keep the namespace clean.
      When you do this, set "syscall_dir" to "syscalls" and add
      -DREENTRANT_SYSCALLS_PROVIDED to newlib_cflags in configure.host.

   2) Define namespace clean versions of the system calls by prefixing
      them with '_' (eg: _open, _close, etc.).  Technically, there won't be
      true reentrancy at the syscall level, but the library will be namespace
      clean.
      When you do this, set "syscall_dir" to "syscalls" in configure.host.

   3) Define or otherwise provide the regular versions of the syscalls
      (eg: open, close, etc.).  The library won't be reentrant nor namespace
      clean, but at least it will work.
      When you do this, add -DMISSING_SYSCALL_NAMES to newlib_cflags in
      configure.host.

   4) Define or otherwise provide the regular versions of the syscalls,
      and do not supply functional interfaces for any of the reentrant
      calls. With this method, the reentrant syscalls are redefined to
      directly call the regular system call without the reentrancy argument.
      When you do this, specify both -DREENTRANT_SYSCALLS_PROVIDED and 
      -DMISSING_SYSCALL_NAMES via newlib_cflags in configure.host and do
      not specify "syscall_dir".

   Stubs of the reentrant versions of the syscalls exist in the libc/reent
   source directory and are provided if REENTRANT_SYSCALLS_PROVIDED isn't 
   defined.  These stubs call the native system calls: _open, _close, etc. 
   if MISSING_SYSCALL_NAMES is *not* defined, otherwise they call the
   non-underscored versions: open, close, etc. when MISSING_SYSCALL_NAMES
   *is* defined.

   By default, newlib functions call the reentrant syscalls internally,
   passing a reentrancy structure as an argument.  This reentrancy structure
   contains data that is thread-specific.  For example, the errno value is
   kept in the reentrancy structure.  If multiple threads exist, each will
   keep a separate errno value which is intuitive since the application flow
   cannot check for failure reliably otherwise.

   The reentrant syscalls are either provided by the platform, by the 
   libc/reent stubs, or in the case of both MISSING_SYSCALL_NAMES and 
   REENTRANT_SYSCALLS_PROVIDED being defined, the calls are redefined to
   simply call the regular syscalls with no reentrancy struct argument.

   A single-threaded application does not need to worry about the reentrancy
   structure.  It is used internally.  

   A multi-threaded application needs either to manually manage reentrancy 
   structures or use dynamic reentrancy.
   
   Manually managing reentrancy structures entails calling special reentrant
   versions of newlib functions that have an additional reentrancy argument.
   For example, _printf_r.  By convention, the first argument is the
   reentrancy structure.  By default, the normal version of the function
   uses the default reentrancy structure: _REENT.  The reentrancy structure
   is passed internally, eventually to the reentrant syscalls themselves.
   How the structures are stored and accessed in this model is up to the
   application.

   Dynamic reentrancy is specified by the __DYNAMIC_REENT__ flag.  This
   flag denotes setting up a macro to replace _REENT with a function call
   to __getreent().  This function needs to be implemented by the platform
   and it is meant to return the reentrancy structure for the current
   thread.  When the regular C functions (e.g. printf) go to call internal
   routines with the default _REENT structure, they end up calling with 
   the reentrancy structure for the thread.  Thus, application code does not
   need to call the _r routines nor worry about reentrancy structures.  */

/* WARNING: All identifiers here must begin with an underscore.  This file is
   included by stdio.h and others and we therefore must only use identifiers
   in the namespace allotted to us.  */

#ifndef _REENT_H_
#ifdef __cplusplus
extern "C" {
#endif
#define _REENT_H_

#include <sys/reent.h>

#ifdef __cplusplus
}
#endif
#endif /* _REENT_H_ */
