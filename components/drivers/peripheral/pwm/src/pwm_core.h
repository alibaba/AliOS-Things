/**
 * @file pwm.h
 * @copyright Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _AOS_PWM_CORE_H
#define _AOS_PWM_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup aos_pwm_core PWM
 *  aos pwm core layer API.
 *
 *  @{
 */

/* This struct define PWM main handle */
typedef struct {
    uint8_t      id;                /**< PWM controller's id */
    uint16_t     pwm_dev_handle;    /**< use &pwm_dev_handle as pwm device's handle */
    pwm_config_t config;            /**< PWM controller's config */
    aos_mutex_t  lock;              /**< lock for PWM controller's operations */
    void         *priv;             /**< priv data */
    uint32_t     magic;             /**< magic no. used for pwm_dev_t validation check */
} pwm_dev_t;

typedef struct pwm_dev_node {
    pwm_dev_handle *dev;       /**<  pointer to pwm user */
    dlist_t        node;       /**<  node in pwm controller's user list */
} pwm_dev_node_t;

/**
 * pwm controller's settings
 * all users in the same PWM controller are connected to the list named with user
 * each time when a pwm operation request is received, we check whether it eques with last pwm operation or not
 * if yes, skip user parameter setting, start/stop pwm directly
*/
typedef struct pwm_controller {
    uint32_t       id;         /**< PWM controller's id */
    aos_mutex_t    lock;       /**< used to lock all operations on current PWM controller */
    dlist_t        user;       /**< node in PWM controller's user list */
    pwm_dev_node_t *cur_dev;   /**< used to log last PWM operation's slave device's pointer if current PWM operation is the same with last one, skip slave config procedure */
    uint32_t       freq;       /**< used to log PWM controller's current frequence setting, if current i2c transaction equals with freq, skip freq setting procedure */
    float          duty_cycle; /**< used to log PWM controller's current duty cycle, if current PWM operation equals with duty_cycle, skip duty cycle setting procedure */
    csi_pwm_t      csi_dev;    /**< CSI PWM device */
} pwm_controller_t;



/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _AOS_PWM_CORE_H */