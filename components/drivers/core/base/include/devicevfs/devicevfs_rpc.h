/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef _DEVICEVFS_RPC_H_
#define _DEVICEVFS_RPC_H_

/**
 *
 * open RPC message handler, called when OPEN rpc message arrived in main rpc service's handler
 *
 * @param parg - pointer to u_fops_arg_u
 * @param crpc_handle - rpc client's handle,
 * response should be send to rpc client no matter this operation succeed or not
 *
 * @return 0 for success; negative for failure
 */
int u_device_rpc_open(u_fops_arg_u *parg, int crpc_handle);

/**
 * close RPC message handler, called when CLOSE rpc message arrived in main rpc service's handler
 *
 * @param parg - pointer to u_fops_arg_u
 * @param crpc_handle - rpc client's handle,
 * response should be send to rpc client no matter this operation succeed or not
 *
 * @return 0 for success; negative for failure
 */
int u_device_rpc_close(u_fops_arg_u *parg, int crpc_handle);

#endif //_DEVICEVFS_RPC_H_
