/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_CONFIG_H__
#define __UVOICE_CONFIG_H__


/* cache type select
 * 0 - no cache
 * 1 - cache to file (fs required)
 * 2 - cache to memory
 */
#define PLAYER_CACHE_TYPE		2

#ifdef __os_linux__
/* set file path for cache type 1 */
#define PLAYER_CACHE_FILE_PATH		"./media_cache"
#else
/* set file path for cache type 1 */
#define PLAYER_CACHE_FILE_PATH		"/sdcard/media_cache"
#endif

/* set memory size (KB) for cache type 2 */
#ifdef __os_linux__
#define PLAYER_CACHE_MEM_SIZE		80//10240
#else

#ifdef UVOICE_ON_BK7251
#define PLAYER_CACHE_MEM_SIZE		120
#else

#ifdef MUSICBOX_APP
#define PLAYER_CACHE_MEM_SIZE		50
#else
#define PLAYER_CACHE_MEM_SIZE		40
#endif

#endif
#endif /* __os_linux__ */

#ifdef __os_linux__
/* select directory for music download */
#define PLAYER_SOURCE_DLOAD_DIR		"../../../../../Music"

/* select directory for music list file */
#define PLAYER_SOURCE_LIST_DIR		"../../../../../Music"
#define PLAYER_SOURCE_LIST_NAME		"source.list"

/* select directory for temporary use */
#define PLAYER_LOCAL_TEMP_DIR		"./temp"

#else

/* select directory for music download */
#define PLAYER_SOURCE_DLOAD_DIR		"/sdcard/music"

/* select directory for music list file */
#define PLAYER_SOURCE_LIST_DIR		"/sdcard/music"
#define PLAYER_SOURCE_LIST_NAME		"source.list"

/* select directory for temporary use */
#define PLAYER_LOCAL_TEMP_DIR		"/sdcard/temp"
#endif


/* 0 - disable stere, 1 - enable stere */
#if defined(STORYPLAYER_APP) || defined(VOICELOUDER_APP)
#define PLAYER_STERE_ENABLE			0
#else
#define PLAYER_STERE_ENABLE			1
#endif


#endif /* __UVOICE_CONFIG_H__ */
