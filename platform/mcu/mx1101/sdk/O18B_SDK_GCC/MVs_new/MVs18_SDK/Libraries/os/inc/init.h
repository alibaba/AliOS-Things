/*---------------------------------------------------------------
 * drafter	:	huangyucai
 * file		:	init.h
 * date		:	2011-06-09
 * desc		:	define the initialization action for specific chip,
 *				some parts code or method  borrow from LINUX head file,
 *				this deinfine in source file and reference in linker file
 *@maintainer Halley
 *
 * change log:
 *				add notification list chain for system frequency change,etc.
 *----------------------------------------------------------------*/
#ifndef		__INIT_H__
#define		__INIT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#define		__used	/*__attribute__((__used__))*/
#define		__attribute_used__	__used			/* deprecated */

#define		__init_start	__used __attribute__((section(".mach0.init")))
#define		__init			__used __attribute__((section(".mach.init")))
#define		__init_end		__used __attribute__((section(".mach9.init")))

/*
 * Used for initialization calls..
 */
typedef int (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#define __define_initcall(level, fn, id) \
	const initcall_t __F##id##_##fn __attribute_used__ \
			__attribute__((__section__(".mach" level ".init"), nocommon)) = (fn)

/*
 * define different initialize import level
 */
#define early_initcall(fn)		__define_initcall("1", fn, 1)
#define core_initcall(fn)		__define_initcall("2", fn, 2)
#define postcore_initcall(fn)	__define_initcall("3", fn, 3)
#define arch_initcall(fn)		__define_initcall("4", fn, 4)
#define subsys_initcall(fn)		__define_initcall("5", fn, 5)
#define fs_initcall(fn)			__define_initcall("6", fn, 6)
#define device_initcall(fn)		__define_initcall("7", fn, 7)
#define late_initcall(fn)		__define_initcall("8", fn, 8)

/*
 * notify change list
 */
#define		__notify_start	__used __attribute__((section(".notify0.init")))
#define		__notify		__used __attribute__((section(".notify.init")))
#define		__nofify_end	__used __attribute__((section(".notify4.init")))

/*
 * define the notify calls
 */
typedef int (*notifycall_t)(unsigned int arg0, unsigned int arg1);

#define __notify_initcall(level, fn, id) \
	const notifycall_t __notifycall##id##_##fn __attribute_used__ \
			__attribute__((__section__(".notify" level ".init"), nocommon)) = ((notifycall_t)fn)

#define early_notifycall(fn)		__notify_initcall("1", fn, 1)
#define fast_notifycall(fn)			__notify_initcall("2", fn, 2)
#define normal_notifycall(fn)		__notify_initcall("3", fn, 3)

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif		//INIT_H
