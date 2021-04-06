/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */
 
#ifndef __UVOICE_MLIST_H__
#define __UVOICE_MLIST_H__

/** @defgroup uvoice_mlist_api uvoice_mlist
 *  @ingroup uvoice_aos_api
 * @{
 */

/**
 * 显示播放列表
 *
 * @retrun 成功返回0，失败返回非0.
 */
int mlist_source_show(void);

/**
 * 扫描本地音频文件，更新播放列表
 *
 * @retrun 成功返回0，失败返回非0.
 */
int mlist_source_scan(void);

/**
 * 获取播放列表
 *
 * @param[in]      index         播放列表中音频文件序号
 * @param[out]     path          音频文件路径
 * @param[in]      len           音频文件路径长度
 * @retrun 成功返回0，失败返回非0.
 */
int mlist_source_get(int index, char *path, int len);

/**
 * 删除播放列表中某个音频文件
 *
 * @param[in]      index         播放列表序号为index的音频文件
 * @retrun 成功返回0，失败返回非0.
 */
int mlist_source_del(int index);

/**
 * 获取播放列表中正在播放音频文件的列表序号
 *
 * @param[out]      index         播放列表中音频文件序号
 * @retrun 成功返回0，失败返回非0.
 */
int mlist_index_get(int *index);

/**
 * 设置当前正在播放音频文件的列表序号
 *
 * @param[in]      index         播放列表序号
 * @retrun 成功返回0，失败返回非0.
 */
int mlist_index_set(int index);

/**
 * @}
 */

#endif /* __UVOICE_MLIST_H__ */

