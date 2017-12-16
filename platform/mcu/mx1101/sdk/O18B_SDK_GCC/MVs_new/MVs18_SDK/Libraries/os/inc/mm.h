////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:mm.h
//
//            maintainer: Halley
//
//		Description	:
//					As a smart memory mangement system on a embedded platform,MMM supplay
//					a set of basic memory management interface to user,such as allocation,
//					free and dection functionality.
//		Changelog	:
//					2012-02-21 transplant from demo SDK by Robert
///////////////////////////////////////////////////////////////////////////////
#ifndef		__MM_H__
#define		__MM_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

extern void* mmm_malloc(size_t size, unsigned char opt);
extern void* mmm_realloc(void* ptr, size_t size, unsigned char opt);
extern void mmm_free(void* buf);
extern int mmm_free_maxblk(unsigned char opt);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif	//__MM_H__

