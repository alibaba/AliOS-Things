/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef SOUND_CONTROL_H
#define SOUND_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <aos/list.h>
#include <aos/kernel.h>
#ifndef HAAS_AUDIO_DEMO
#include <sys/ioctl.h>
#endif
#include "audio_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup a2sa_api a2sa
 *  @ingroup aos_components
 * @{
 */

/**
 * @}
 */

/** @defgroup a2sa_mixer_api mixer
 *  @ingroup a2sa_api
 * @{
 */

/* 音频mixer设备节点描述信息 */
typedef struct {
    int fd;                                  /**< 声卡mixer设备节点fd值，通过open()函数获取 */
    int card;                                /**< 声卡ID */
    char *name;                              /**< 声卡mixer设备节点名字，例如"/dev/controlC0" */
    struct audio_ctl_card_info card_info;    /**< 声卡基础信息，例如声卡驱动名字 */
    struct audio_ctl_elem_info *elem_info;   /**< 声卡配置信息元素地址 */
    unsigned int count;                      /**< 声卡配置信息元素个数 */
} aos_mixer_t;

/**
 * 打开ID为card的声卡的mixer设备节点
 *
 * @param[out]  mixer  保存mixer设备节点信息的二级指针
 * @param[in]  card   期望打开的声卡ID
 *
 * @return  0 on success, negative error on failure.
 */
int aos_mixer_open(aos_mixer_t **mixer, int card);

/**
 * 打印当前声卡mixer设备节点的所有属性信息
 *
 * @param[in]  mixer  保存mixer设备节点信息的指针
 *
 * @return  0 on success, negative error on failure.
 */
int aos_mixer_print_info(aos_mixer_t *mixer);

/**
 * 设置mixer设备节点中名为name的属性值为int value
 *
 * @param[in]  mixer  保存mixer设备节点信息的指针
 * @param[in]  name   期望设置的目标属性名字
 * @param[in]  value  期望设置到名字为name的目标属性的int值
 *
 * @return  0 on success, negative error on failure.
 */
int aos_mixer_set_int_value(aos_mixer_t *mixer, char *name, int value);

/**
 * 获取mixer设备节点中名为name的属性值，并保存在value指针中
 *
 * @param[in]  mixer  保存mixer设备节点信息的指针
 * @param[in]  name   期望获取的目标属性名字
 * @param[out]  *value 获取到的目标属性值保存在int *value中
 *
 * @return  0 on success, negative error on failure.
 */
int aos_mixer_get_int_value(aos_mixer_t *mixer, char *name, int *value);

/**
 * 关闭mixer设备节点
 *
 * @param[in]  mixer  期望关闭的mixer设备节点信息的指针
 *
 * @return  0 on success, negative error on failure.
 */
int aos_mixer_close(aos_mixer_t *mixer);

/**
 * 设置所有声卡中名为“Master Volume TX”的属性的值为volume
 *
 * @param[in]  volume  期望往声卡中配置的音量值
 *
 * @return  0 on success, negative error on failure.
 */
int aos_set_master_volume(int volume);

/**
 * 获取声卡中名为“Master Volume TX”的属性的值，并保存在volume指针中
 *
 * @param[out]  *volume  获取到声卡音量值后保存到volume指针中
 *
 * @return  0 on success, negative error on failure.
 */
int aos_get_master_volume(int *volume);

/**
 * 设置所有声卡为mute状态，对应的属性名为“Master Mute State”
 *
 * @param[in]  mute  1为mute状态，0为unmute状态
 *
 * @return  0 on success, negative error on failure.
 */
int aos_set_mute_state(int mute);

/**
 * 获取声卡的mute状态，并保存在*mute指针中
 *
 * @param[out]  *mute  返回1为mute状态，0为unmute状态
 *
 * @return  0 on success, negative error on failure.
 */
int aos_get_mute_state(int *mute);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* SOUND_CONTROL_H */