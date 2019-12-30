/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UAGENT_H
#define UAGENT_H

#include "uagent_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register service routine from customer into uagent for delegate
 * Update the cb if the same mod and func alreay registered before.
 *
 * @param[in]  mod         Module type, reference @ua_mod_t
 * @param[in]  mod_name    Name of module, only user module fill this, pre-define module use NULL.
 * @param[in]  version     Version of module, usually comes from aos.mk
 * @param[in]  func        Service Name
 * @param[in]  func_name   Name of service
 * @param[in]  cb          Service Routine
 * @param[in]  arg         Argument of the function.
 *
 * @return 0: Success, -EINVAL: invalid parameter, -EIO: Service mapping table
 *             has no enough room for new service
 */
int uagent_register(const ua_mod_t mod, const char *mod_name, char *version,
                    const ua_func_t func, const char *func_name,
                    on_customer_service cb, void *arg);

/**
 * Un-register service routine
 *
 * @param[in]  mod   Module type, reference @ua_mod_t
 * @param[in]  func  Service Name
 *
 * @return 0: Success, -1: Fail
 */
int uagent_unregister(const ua_mod_t mod, const ua_func_t func);

/**
 * Request other's module's service.
 * Remark: Customer can call any service(in case of the service exist),
 * even this customer doesn't offer any service
 *
 * @param[in]  src       Source module type, reference @ua_mod_t
 * @param[in]  target    Target module type, reference @ua_mod_t
 * @param[in]  func      Function type, include pre-define func and customer's define
 * @param[in]  len       Argument string's length
 * @param[in]  info_str  Argument string
 *
 * @return 0: Success, -EINVAL: invalid parameter, -EIO: Service mapping table
 *             has no enough room for new service
 */
int uagent_request_service(const ua_mod_t src, const ua_mod_t target,
                           const ua_func_t func, const unsigned short len,
                           const void *info_str);

/**
 * Publish customer's info.
 * Remark: Customer can call any service(in case of the service exist), even this
 * customer doesn't offer any service
 *
 *
 * @param[in]  mod       Source module type, reference @ua_mod_t
 * @param[in]  type      Type indicates info_str, not necessary
 * @param[in]  len       Argument string's length
 * @param[in]  info_str  Argument string
 * @param[in]  policy    Values are constructed by a bitwise-inclusive OR of flags
 * from the @send_policy_bit_ctrl_t
 *
 * @return 0: Success, -EINVAL: invalid parameter, -EIO: Service mapping table
 *             has no enough room for new service
 */
int uagent_send(const ua_mod_t mod, const ua_func_t type,
                const unsigned short len, const void *info_str,
                const ua_send_policy_t policy);

/**
 * Initial uagent function
 *
 * @return 0: Success
 */
int uagent_init(void);

/**
 * Start uagent ext comm service
 * Remark: Event return 0 doesn't indicates the connection already build
 *
 * @param[in]  pk  Product Key
 * @param[in]  dn  Device Name
 *
 * @return 0: Success, -1 fail
 */
int uagent_ext_comm_start(const char *pk, const char *dn);

#ifdef __cplusplus
}
#endif

#endif /* UAGENT_H */

