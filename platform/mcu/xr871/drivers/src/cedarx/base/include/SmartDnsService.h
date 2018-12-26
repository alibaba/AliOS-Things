/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : SmartDnsService.h
 * Description : SmartDnsService
 * History :
 *
 */

#ifndef SMART_DNS_SERVICE_H
#define SMART_DNS_SERVICE_H

typedef void (*ResponeHook)(void * /* userhdr */, int /* ret */ , struct addrinfo * /* ret */);

enum SDSReturnValueE
{
    SDS_OK,
    SDS_PENDING,
    SDS_ERROR,
};

#ifdef __cplusplus
extern "C"
{
#endif

int SDSRequest(const char *hostname, int port, struct addrinfo **pAddr,
    void *userHdr, ResponeHook hook);

#ifdef __cplusplus
}
#endif

#endif
